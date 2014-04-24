#include "hessdalen_weather.h"

static m_node_t *temp_list;
static m_node_t *humi_list;
static m_node_t *pressure_list;
static m_node_t *wind_speed_list;
static m_node_t *wind_dir_list;

static node_t *final_values;

static tm *datetime;
static uint8_t measured;

void start_watchdog(uint32_t ms)
{
	NutWatchDogStart(ms, 0);
}

void restart_watchdog(void)
{
	NutWatchDogRestart();
}

void read_sensors(void)
{
	printf("Read sensors: Current time: %d:%d:%d\n", datetime->tm_hour, datetime->tm_min, datetime->tm_sec);

	double temp, humi, dew, wspeed, wdirection;
	int32_t pressure;
	m_node_t *sht10_temp, *sht10_humi, *bmp180_pressure, *wind_speed, *wind_dir;
	char dt[20];

	sprintf(dt, "%d-%d-%d %d:%d:%d", (datetime->tm_year + 1900), (datetime->tm_mon + 1), datetime->tm_mday, datetime->tm_hour, datetime->tm_min, datetime->tm_sec);

	sht10_measure(&temp, &humi, &dew);
	bmp180_read_data(&pressure);
	wind_data_read(&wspeed, &wdirection);

	sht10_temp = malloc(sizeof(m_node_t));
	sht10_temp->datetime = dt;
	sht10_temp->value = temp;

	sht10_humi = malloc(sizeof(m_node_t));
	sht10_humi->datetime = dt;
	sht10_humi->value = humi;

	bmp180_pressure = malloc(sizeof(m_node_t));
	bmp180_pressure->datetime = dt;
	bmp180_pressure->value = (double) pressure;

	wind_speed = malloc(sizeof(m_node_t));
	wind_speed->datetime = dt;
	wind_speed->value = wspeed;

	wind_dir = malloc(sizeof(m_node_t));
	wind_dir->datetime = dt;
	wind_dir->value = wdirection;

	m_push(temp_list, sht10_temp);
	m_push(humi_list, sht10_humi);
	m_push(pressure_list, bmp180_pressure);
	m_push(wind_speed_list, wind_speed);
	m_push(wind_dir_list, wind_dir);
}

void prepare_wind_data(values_t *ret_wind)
{
	m_node_t *wspeed_current = NULL, *wdir_current = NULL;
	uint16_t wspeed_num = 0;
	double wspeed_sum = 0.0;
	double wspeed_max, wspeed_min, wdir_max, wspeed, wdir;

	char *wspeed_max_dt, *wspeed_min_dt;
	values_t *new_wind = malloc(sizeof(values_t));
	m_node_t *last_wind = malloc(sizeof(m_node_t));
	
	m_get_last(wind_speed_list, last_wind);

	new_wind->value = "wind";
	new_wind->now = last_wind->value;

	free(last_wind);
	
	m_pop(&wind_speed_list, wspeed_current);
	m_pop(&wind_dir_list, wdir_current);

	wspeed_max = wspeed_min = wspeed_current->value;
	wspeed_max_dt = wspeed_min_dt = wspeed_current->datetime;
	wdir_max = wdir_current->value;

	while((wspeed_current != NULL) && (wdir_current != NULL)) {
		wspeed_num++;
		
		wspeed = wspeed_current->value;
		wdir = wdir_current->value;

		if(wspeed > wspeed_max) {
			wspeed_max = wspeed;
			wspeed_max_dt = wspeed_current->datetime;
			wdir_max = wdir_current->value;
		}
		if(wspeed < wspeed_min) {
			wspeed_min = wspeed;
			wspeed_min_dt = wspeed_current->datetime;
		}
		wspeed_sum += wspeed;
		
		free(wspeed_current);
		free(wdir_current);

		m_pop(&wind_speed_list, wspeed_current);
		m_pop(&wind_dir_list, wdir_current);
	}
	wspeed_sum -= wspeed_max;
	wspeed_sum -= wspeed_min;

	if(wspeed_num > 0) {
		new_wind->avg = ((double) wspeed_sum) / ((double) wspeed_num);
		new_wind->max = wspeed_max;
		new_wind->min = wspeed_min;
		new_wind->time_max = wspeed_max_dt;
		new_wind->time_min = wspeed_min_dt;
		new_wind->max_dir = wdir_max;
	}

	ret_wind = new_wind;
}

void prepare_bmp180_data(values_t *ret_pressure)
{
	m_node_t *pressure_current = NULL;
	uint16_t pressure_num = 0;
	int32_t pressure_sum = 0;
	int32_t pressure_max, pressure_min, pressure;

	char *pressure_max_dt, *pressure_min_dt;
	values_t *new_pressure = malloc(sizeof(values_t));
	m_node_t *last_pressure = malloc(sizeof(m_node_t));

	m_get_last(pressure_list, last_pressure);
	
	new_pressure->value = "pressure";
	new_pressure->now = last_pressure->value;
	
	free(last_pressure);

	m_pop(&pressure_list, pressure_current);

	pressure_max = pressure_min = (int32_t) pressure_current->value;
	pressure_max_dt = pressure_min_dt = pressure_current->datetime;
	
	while(pressure_current != NULL) {
		pressure_num++;
		
		pressure = (int32_t) pressure_current->value;

		if(pressure > pressure_max) {
			pressure_max = pressure;
			pressure_max_dt = pressure_current->datetime;
		}
		if(pressure < pressure_min) {
			pressure_min = pressure;
			pressure_min_dt = pressure_current->datetime;
		}

		pressure_sum += pressure;

		free(pressure_current);

		m_pop(&pressure_list, pressure_current);
	}

	pressure_sum -= pressure_max;
	pressure_sum -= pressure_min;

	if(pressure_num > 0) {
		new_pressure->avg = ((double) pressure_sum) / ((double) pressure_num);
		new_pressure->max = pressure_max;
		new_pressure->min = pressure_min;
		new_pressure->time_max = pressure_max_dt;
		new_pressure->time_min = pressure_min_dt;
	}

	ret_pressure = new_pressure;
}

void prepare_sht10_data(values_t *ret_temp, values_t *ret_humi)
{
	m_node_t *temp_current = NULL; 
	m_node_t *humi_current = NULL;
	uint16_t temp_num = 0, humi_num = 0;
	double temp_sum = 0.0, humi_sum = 0.0, temp_max, temp_min, humi_max, humi_min, temp, humi;

	char *temp_max_dt, *humi_max_dt, *temp_min_dt, *humi_min_dt;
	values_t *new_temp = malloc(sizeof(values_t)), *new_humi = malloc(sizeof(values_t));
	m_node_t *last_temp = malloc(sizeof(m_node_t)), *last_humi = malloc(sizeof(m_node_t));

	m_get_last(temp_list, last_temp);
	m_get_last(humi_list, last_humi);

	new_temp->value = "temp";
	new_temp->now = last_temp->value;

	new_humi->value = "humidity";
	new_humi->now = last_humi->value;

	free(last_temp);
	free(last_humi);

	m_pop(&temp_list, temp_current);
	m_pop(&humi_list, humi_current);

	temp_max = temp_min = temp_current->value;
	temp_max_dt = temp_min_dt = temp_current->datetime;

	humi_max = humi_min = humi_current->value;
	humi_max_dt = humi_min_dt = humi_current->datetime;

	while((temp_current != NULL) && (humi_current != NULL)) {
		temp_num++;
		humi_num++;
		
		temp = temp_current->value;
		humi = humi_current->value;

		if(temp > temp_max) {
			temp_max = temp;
			temp_max_dt = temp_current->datetime;
		}
		if(temp < temp_min) {
			temp_min = temp;
			temp_min_dt = temp_current->datetime;
		}
		if(humi > humi_max) {
			humi_max = humi;
			humi_max_dt = humi_current->datetime;
		}
		if(humi < humi_min) {
			humi_min = humi;
			humi_min_dt = humi_current->datetime;
		}

		temp_sum += temp;
		humi_sum += humi;

		free(temp_current);
		free(humi_current);

		m_pop(&temp_list, temp_current);
		m_pop(&humi_list, temp_current);
	}

	temp_sum -= temp_max;
	temp_sum -= temp_min;
	humi_sum -= humi_max;
	humi_sum -= humi_min;
	
	if(temp_num > 0) {
		new_temp->avg = ((double) temp_sum) / ((double) temp_num);
		new_temp->max = temp_max;
		new_temp->min = temp_min;
		new_temp->time_max = temp_max_dt;
		new_temp->time_min = temp_min_dt;
	}

	if(humi_num > 0) {
		new_humi->avg = ((double) humi_sum) / ((double) humi_num);
		new_humi->max = humi_max;
		new_humi->min = humi_min;
		new_humi->time_max = humi_max_dt;
		new_humi->time_min = humi_min_dt;
	}

	ret_temp = new_temp;
	ret_humi = new_humi;
}

void prepare_data(void)
{
	values_t *temp = NULL, *humi = NULL, *pressure = NULL, *wind = NULL;
	node_t *final = malloc(sizeof(node_t));
	final->datetime = malloc(sizeof(char) * 20);
	
	sprintf(final->datetime, "%d-%d-%d %d:%d:%d", (datetime->tm_year + 1900), (datetime->tm_mon + 1), datetime->tm_mday, datetime->tm_hour, datetime->tm_min, datetime->tm_sec);
	
	final->station_id = STATION_ID;

	prepare_sht10_data(temp, humi);
	prepare_bmp180_data(pressure);
	prepare_wind_data(wind);
	
	final->temp = temp;
	final->humi = humi;
	final->pressure = pressure;
	final->wind = wind;

	push(final_values, final);
}

void send_data(void)
{
	node_t *current = NULL;
	values_t *temp, *humi, *pressure, *wind;

	char *json_root, *json_temp, *json_humi, *json_pressure, *json_wind, *json;

	pop(&final_values, current);

	while(current != NULL) {
		temp = current->temp;
		humi = current->humi;
		pressure = current->pressure;
		wind = current->wind;

		get_json_string_root(current->datetime, current->station_id, json_root);

		get_json_string(temp->value, temp->avg, temp->now, temp->max, temp->time_max, temp->min, temp->time_min, json_temp);
		get_json_string(humi->value, humi->avg, humi->now, humi->max, humi->time_max, humi->min, humi->time_min, json_humi);
		get_json_string(pressure->value, pressure->avg, pressure->now, pressure->max, pressure->time_max, pressure->min, pressure->time_min, json_pressure);
		get_json_wstring(wind->avg, wind->now, wind->max, wind->time_max, wind->max_dir, wind->min, wind->time_min, json_wind);

		get_json(json_root, json_temp, json_humi, json_pressure, json_wind, json);
		
		send_json(json);

		free(temp);
		free(humi);
		free(pressure);
		free(wind);
		free(current);

		pop(&final_values, current);
	}
}

void wait_for_whole_min(void) {
	for(;;) {
		datetime = get_current_time();
		if(datetime->tm_sec == 0)
			break;

		NutSleep(100);
	}
	puts("WHOLE MIN");
}

void wait_30_sec(void) {
	for(;;) {
		datetime = get_current_time();
		if(measured == 1) { //Venter slik at vi ikke gjør flere målinger hvert 30.sek.
			while(1) {
				datetime = get_current_time();
				if((datetime->tm_sec % 30) != 0)
					break;
			}
			measured = 0;
		}

		if((datetime->tm_sec % 30) == 0)
			break;
		NutSleep(100);
		restart_watchdog();
	} 
}

void configure_debug(uint32_t baud)
{
	NutRegisterDevice(&DEV_DEBUG, 0, 0);
	freopen(DEV_DEBUG_NAME, "w", stdout);
	_ioctl(_fileno(stdout), UART_SETSPEED, &baud);
}

int main(void)
{
	start_watchdog(5000); //Starter watchdog med nedtelling på 5 sec.
	uint32_t baud = 115200;
	datetime = malloc(sizeof(tm));

	temp_list = malloc(sizeof(m_node_t)); // Gjør klar lister for midlertidig lagring av målinger.
	humi_list = malloc(sizeof(m_node_t));
	pressure_list = malloc(sizeof(m_node_t));
	wind_speed_list = malloc(sizeof(m_node_t));
	wind_dir_list = malloc(sizeof(m_node_t));
	
	final_values = malloc(sizeof(node_t)); // Liste for lagring av ferdig utregnede verdier.

	configure_debug(baud); // Setter output til serieutgang.
	configure_network(); // Initialiserer ethernet.
	
	set_time_ntp(); // Setter klokken.
	adc_init(); // Initialiserer ADC.
	bmp180_init(); // Initialiserer BMP180.

	puts("Project Hessdalen weather station");

	wait_for_whole_min(); // Venter på helt minutt før vi begynner å måle.

	for (;;) { //Hovedløkke.
		read_sensors();
		restart_watchdog();
		measured = 1;

		if(((datetime->tm_min % 5) == 0) && (datetime->tm_sec == 0)) { //Regner ut gjennomsnitt og max/min hvert 5. min.
			prepare_data();
			restart_watchdog();

			if((datetime->tm_min == 0) && (datetime->tm_sec == 0)) { //Sender data hver hele time.
				send_data();
				restart_watchdog();
			}
		}
		wait_30_sec(); //Gjør ny måling hvert 30. sekund.
	}

	return 0;
}
