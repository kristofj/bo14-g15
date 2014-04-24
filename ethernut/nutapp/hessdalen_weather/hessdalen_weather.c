#include "hessdalen_weather.h"

static m_node_t *temp_list;
static m_node_t *humi_list;
static m_node_t *pressure_list;
static m_node_t *wind_speed_list;
static m_node_t *wind_dir_list;

static node_t *final_values;

void start_watchdog(uint32_t ms)
{
	NutWatchDogStart(ms, 0);
}

void restart_watchdog(void)
{
	NutWatchDogRestart();
}

void read_sensors(tm *datetime)
{
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

void prepare_sht10_data(node_t *ret_temp, node_t *ret_humi)
{
	m_node_t *temp_current = NULL;
	m_node_t *humi_current = NULL;
	uint16_t temp_num = 0, humi_num = 0;
	double temp_sum = 0.0, humi_sum = 0.0, temp_max, temp_min, humi_max, humi_min, temp, humi;

	char *temp_max_dt, *humi_max_dt, *temp_min_dt, *humi_min_dt;
	node_t *new_temp = malloc(sizeof(node_t)), *new_humi = malloc(sizeof(node_t));
	m_node_t *last_temp = malloc(sizeof(m_node_t)), *last_humi = malloc(sizeof(m_node_t));

	m_get_last(temp_list, last_temp);
	m_get_last(humi_list, last_humi);

	new_temp->datetime = last_temp->datetime; //Henter ut siste tidverdi, som er fem-minutteren.
	new_temp->value = "temp";
	new_temp->now = last_temp->value;
	new_temp->max_dir = -1;
	new_temp->station_id = ETHERNUT_1; //Endres avhengig av ethernut.

	new_humi->datetime = last_humi->datetime;
	new_humi->value = "humidity";
	new_humi->now = last_humi->value;
	new_humi->max_dir = -1;
	new_humi->station_id = ETHERNUT_1;

	free(last_temp);
	free(last_humi);

	m_pop(&temp_list, temp_current);
	m_pop(&humi_list, humi_current);

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

	new_temp->avg = ((double) temp_sum) / ((double) temp_num);
	new_temp->max = temp_max;
	new_temp->min = temp_min;
	new_temp->time_max = temp_max_dt;
	new_temp->time_min = temp_min_dt;
	
	new_humi->avg = ((double) humi_sum) / ((double) humi_num);
	new_humi->max = humi_max;
	new_humi->min = humi_min;
	new_humi->time_max = humi_max_dt;
	new_humi->time_min = humi_min_dt;
	
	ret_temp = new_temp;
	ret_humi = new_humi;
}

void prepare_data(tm *datetime)
{

}

void send_data(void)
{

}

void wait_for_whole_min(tm *datetime) {
	do {
		get_current_time(datetime);
		NutSleep(100);
	} while (datetime->tm_sec == 0);
}

void wait_30_sec(tm *datetime) {
	do {
		get_current_time(datetime);
		NutSleep(100);
	} while((datetime->tm_sec % 30) == 0);
}

void configure_debug(uint32_t baud)
{
	NutRegisterDevice(&DEV_DEBUG, 0, 0);
	freopen(DEV_DEBUG_NAME, "w", stdout);
	_ioctl(_fileno(stdout), UART_SETSPEED, &baud);
}

int main(void)
{
	uint32_t baud = 115200;
	tm *datetime;

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
	
	wait_for_whole_min(datetime); // Venter på helt minutt før vi begynner å måle.

	for (;;) { //Hovedløkke.
		read_sensors(datetime);

		if((datetime->tm_min % 5) == 0) { //Regner ut gjennomsnitt og max/min hvert 5. min.
			prepare_data(datetime);

			if(datetime->tm_min == 0) { //Sender data hver hele time.
				send_data();
			}
		}
		wait_30_sec(&datetime); //Gjør ny måling hvert 30. sekund.
	}
	return 0;
}
