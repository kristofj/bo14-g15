#include "hessdalen_weather.h"

static measure_t temp_list[MEASURE_ARR_MAX];
static measure_t humi_list[MEASURE_ARR_MAX];
static measure_t pressure_list[MEASURE_ARR_MAX];
static measure_t wind_speed_list[MEASURE_ARR_MAX];
static measure_t wind_dir_list[MEASURE_ARR_MAX];

static final_value_t final_values[FINAL_ARR_MAX];

static tm *datetime;
static uint8_t measured;

static int measure_index;
static int final_value_index;

uint8_t bmp180_initialized;

void start_watchdog(void)
{
	wdt_enable(7); //Setter wdt til 1.8s (egentlig rundt 1.5s).
}

void restart_watchdog(void)
{
	wdt_reset();
}

void disable_watchdog(void)
{
	wdt_disable();
}

void read_sensors(void)
{
	double temp, humi, dew, wspeed, wdirection;
	uint8_t error;
	int32_t pressure;
	char *dt = malloc(sizeof(char) * 20);
	measure_t *sht10_temp = &temp_list[measure_index],
		*sht10_humi = &humi_list[measure_index],
		*bmp180_pressure = &pressure_list[measure_index],
		*wind_speed = &wind_speed_list[measure_index],
		*wind_dir = &wind_dir_list[measure_index];

	restart_watchdog();

	sprintf(dt, "%d-%d-%d %d:%d:%d", (datetime->tm_year + 1900), (datetime->tm_mon + 1), datetime->tm_mday, datetime->tm_hour, datetime->tm_min, datetime->tm_sec);
	
	restart_watchdog();

	printf("Read sensors: current time: %s\n", dt);

	error = sht10_measure(&temp, &humi, &dew);

	restart_watchdog();

	if(error != 0) {
		temp = 0.0;
		humi = 0.0;
	}

	restart_watchdog();

	if(bmp180_initialized == 1)
		bmp180_read_data(&pressure);
	else
		pressure = 0;

	restart_watchdog();

	wind_data_read(&wspeed, &wdirection);

	restart_watchdog();
	
	printf("Read sensors: Temp: %lf, Humi: %lf, Pressure: %ld, WSpeed: %lf, WDir: %lf\n", temp, humi, pressure, wspeed, wdirection);

	strcpy(sht10_temp->datetime, dt);
	sht10_temp->value = temp;

	strcpy(sht10_humi->datetime, dt);
	sht10_humi->value = humi;

	strcpy(bmp180_pressure->datetime, dt);
	bmp180_pressure->value = (double) pressure;

	strcpy(wind_speed->datetime, dt);
	wind_speed->value = wspeed;

	strcpy(wind_dir->datetime, dt);
	wind_dir->value = wdirection;

	measure_index++;
	free(dt);

	restart_watchdog();
}

void prepare_wind_data(range_t *ret_wind)
{
	uint8_t i;
	measure_t *wspeed_current = wind_speed_list, *wdir_current = wind_dir_list;
	uint16_t wspeed_num = 0;
	double wspeed_sum = 0.0;
	double wspeed_max, wspeed_min, wdir_max, wspeed, wdir;

	char *wspeed_max_dt = malloc(sizeof(char) * 20),
		*wspeed_min_dt = malloc(sizeof(char) * 20);
	measure_t *last_wind = &wind_speed_list[measure_index - 1];
	
	strcpy(ret_wind->measure_class, "wind");
	ret_wind->now = last_wind->value;
	
	wspeed_max = wspeed_min = wspeed_current->value;
	strcpy(wspeed_max_dt, wspeed_current->datetime);
	strcpy(wspeed_min_dt, wspeed_current->datetime);
	wdir_max = wdir_current->value;

	for(i = 0; i < measure_index; i++, wspeed_current++, wdir_current++) {
		wspeed_num++;
		
		wspeed = wspeed_current->value;

		if(wspeed > wspeed_max) {
			wspeed_max = wspeed;
			strcpy(wspeed_max_dt, wspeed_current->datetime);
			wdir_max = wdir_current->value;
		}
		if(wspeed < wspeed_min) {
			wspeed_min = wspeed;
			strcpy(wspeed_min_dt, wspeed_current->datetime);
		}
		wspeed_sum += wspeed;
	}
	if(wspeed_num > 2) {
		wspeed_num -= 2;

		wspeed_sum -= wspeed_max;
		wspeed_sum -= wspeed_min;
	}
	
	if(wspeed_num > 0) {
		ret_wind->avg = ((double) wspeed_sum) / ((double) wspeed_num);
		ret_wind->max = wspeed_max;
		ret_wind->min = wspeed_min;
		strcpy(ret_wind->time_max, wspeed_max_dt);
		strcpy(ret_wind->time_min, wspeed_min_dt);
		ret_wind->max_dir = wdir_max;
	}

	free(wspeed_max_dt);
	free(wspeed_min_dt);
}

void prepare_bmp180_data(range_t *ret_pressure)
{
	uint8_t i;
	measure_t *pressure_current = pressure_list;
	uint16_t pressure_num = 0;
	int32_t pressure_sum = 0;
	int32_t pressure_max, pressure_min, pressure;

	char *pressure_max_dt = malloc(sizeof(char) * 20),
		*pressure_min_dt = malloc(sizeof(char) * 20);
	measure_t *last_pressure = &pressure_list[measure_index - 1];
	
	strcpy(ret_pressure->measure_class, "pressure");
	ret_pressure->now = last_pressure->value;
	
	pressure_max = pressure_min = pressure_current->value;
	strcpy(pressure_max_dt, pressure_current->datetime);
	strcpy(pressure_min_dt, pressure_current->datetime);

	for(i = 0; i < measure_index; i++, pressure_current++) {
		pressure_num++;
		
		pressure = pressure_current->value;

		if(pressure > pressure_max) {
			pressure_max = pressure;
			strcpy(pressure_max_dt, pressure_current->datetime);
		}
		if(pressure < pressure_min) {
			pressure_min = pressure;
			strcpy(pressure_min_dt, pressure_current->datetime);
		}

		pressure_sum += pressure;
	}
	if(pressure_num > 2) {
		pressure_num -= 2;

		pressure_sum -= pressure_max;
		pressure_sum -= pressure_min;
	}

	if(pressure_num > 0) {
		ret_pressure->avg = ((double) pressure_sum) / ((double) pressure_num);
		ret_pressure->max = pressure_max;
		ret_pressure->min = pressure_min;
		strcpy(ret_pressure->time_max, pressure_max_dt);
		strcpy(ret_pressure->time_min, pressure_min_dt);
	}

	free(pressure_max_dt);
	free(pressure_min_dt);
}

void prepare_sht10_data(range_t *ret_temp, range_t *ret_humi)
{
	uint8_t i;
	measure_t *temp_current = temp_list, *humi_current = humi_list;
	uint16_t temp_num = 0, humi_num = 0;
	double temp_sum = 0.0, humi_sum = 0.0, temp_max, temp_min, humi_max, humi_min, temp, humi;

	char *temp_max_dt = malloc(sizeof(char) * 20),
		*humi_max_dt = malloc(sizeof(char) * 20),
		*temp_min_dt = malloc(sizeof(char) * 20),
		*humi_min_dt = malloc(sizeof(char) * 20);

	measure_t *last_temp = NULL, *last_humi = NULL;
	
	last_temp = &temp_list[measure_index - 1];
	last_humi = &humi_list[measure_index - 1];
	
	strcpy(ret_temp->measure_class, "temp");
	ret_temp->now = last_temp->value;

	strcpy(ret_humi->measure_class, "humidity");
	ret_humi->now = last_humi->value;

	temp_max = temp_min = temp_current->value;
	strcpy(temp_max_dt, temp_current->datetime);
	strcpy(temp_min_dt, temp_current->datetime);

	humi_max = humi_min = humi_current->value;
	strcpy(humi_max_dt, humi_current->datetime);
	strcpy(humi_min_dt, humi_current->datetime);

	for(i = 0; i < measure_index; i++, temp_current++, humi_current++) {
		temp_num++;
		humi_num++;

		temp = temp_current->value;
		humi = humi_current->value;

		if(temp > temp_max) {
			temp_max = temp;
			strcpy(temp_max_dt, temp_current->datetime);
		}
		if(temp < temp_min) {
			temp_min = temp;
			strcpy(temp_min_dt, temp_current->datetime);
		}
		if(humi > humi_max) {
			humi_max = humi;
			strcpy(humi_max_dt, humi_current->datetime);
		}
		if(humi < humi_min) {
			humi_min = humi;
			strcpy(humi_min_dt, humi_current->datetime);
		}

		temp_sum += temp;
		humi_sum += humi;
	}
	
	if(temp_num > 2 && humi_num > 2) {
		temp_sum -= temp_max;
		temp_sum -= temp_min;
		humi_sum -= humi_max;
		humi_sum -= humi_min;

		temp_num -= 2;
		humi_num -= 2;
	}

	if(temp_num > 0) {
		ret_temp->avg = ((double) temp_sum) / ((double) temp_num);
		ret_temp->max = temp_max;
		ret_temp->min = temp_min;
		strcpy(ret_temp->time_max, temp_max_dt);
		strcpy(ret_temp->time_min, temp_min_dt);
	}

	if(humi_num > 0) {
		ret_humi->avg = ((double) humi_sum) / ((double) humi_num);
		ret_humi->max = humi_max;
		ret_humi->min = humi_min;
		strcpy(ret_humi->time_max, humi_max_dt);
		strcpy(ret_humi->time_min, humi_min_dt);
	}
	
	free(temp_max_dt);
	free(temp_min_dt);
	free(humi_max_dt);
	free(humi_min_dt);
}

void prepare_data(void)
{
	range_t *temp = malloc(sizeof(range_t)),
		*humi = malloc(sizeof(range_t)),
		*pressure = malloc(sizeof(range_t)),
		*wind = malloc(sizeof(range_t));
	final_value_t *final = &final_values[final_value_index];

	sprintf(final->datetime, "%d-%d-%d %d:%d:%d", (datetime->tm_year + 1900), (datetime->tm_mon + 1), datetime->tm_mday, datetime->tm_hour, datetime->tm_min, datetime->tm_sec);
	
	final->station_id = STATION_ID;

	restart_watchdog();

	prepare_sht10_data(temp, humi);

	restart_watchdog();

	prepare_bmp180_data(pressure);

	restart_watchdog();

	prepare_wind_data(wind);

	restart_watchdog();
	
	puts("---------------- PREPARED DATA -----------------");
	printf("DATETIME: %s \n", final->datetime);
	printf("STATION_ID: %d \n", final->station_id);
	printf("TEMP: class: %s, avg: %lf, now: %lf, max: %lf, time_max: %s, min: %lf, time_min: %s\n", temp->measure_class, temp->avg, temp->now, temp->max, temp->time_max, temp->min, temp->time_min);
	printf("HUMI: class: %s, avg: %lf, now: %lf, max: %lf, time_max: %s, min: %lf, time_min: %s\n", humi->measure_class, humi->avg, humi->now, humi->max, humi->time_max, humi->min, humi->time_min);
	printf("PRESSURE: class: %s, avg: %lf, now: %lf, max: %lf, time_max: %s, min: %lf, time_min: %s\n", pressure->measure_class, pressure->avg, pressure->now, pressure->max, pressure->time_max, pressure->min, pressure->time_min);
	printf("WIND: class: %s, avg: %lf, now: %lf, max: %lf, time_max: %s, min: %lf, time_min: %s, max_dir: %lf\n", wind->measure_class, wind->avg, wind->now, wind->max, wind->time_max, wind->min, wind->time_min, wind->max_dir);
	printf("AVAILABLE MEMORY: %d bytes\n", NutHeapAvailable());
	puts("---------------- PREPARED DATA -----------------");

	final->temp = temp;
	final->humi = humi;
	final->pressure = pressure;
	final->wind = wind;

	final_value_index++;
	measure_index = 0;

	restart_watchdog();
}

void send_data(void)
{
	uint8_t i;
	final_value_t *current = final_values;
	range_t *temp = NULL, *humi = NULL, *pressure = NULL, *wind = NULL;

	char *json_root = NULL,
		*json_temp = NULL,
		*json_humi = NULL,
		*json_pressure = NULL,
		*json_wind = NULL,
		*json = NULL,
		*json_array = malloc(13000),
		*temp_string = malloc(1024);

	memset(temp_string, 0, strlen(temp_string));
	memset(json_array, 0, strlen(json_array));

	restart_watchdog();

	for(i = 0; i < final_value_index; i++, current++) {
		json_root = malloc(100);
		json_temp = malloc(200);
		json_humi = malloc(200);
		json_pressure = malloc(200);
		json_wind = malloc(200);
		json = malloc(1024);

		memset(json_root, 0, strlen(json_root));
		memset(json_temp, 0, strlen(json_temp));
		memset(json_humi, 0, strlen(json_humi));
		memset(json_pressure, 0, strlen(json_pressure));
		memset(json_wind, 0, strlen(json_wind));
		memset(json, 0, strlen(json));

		restart_watchdog();

		temp = current->temp;
		humi = current->humi;
		pressure = current->pressure;
		wind = current->wind;
		
		puts("-------------- JSON STRINGS -------------");
		get_json_string_root(current->datetime, current->station_id, json_root);
		printf("Root string: %s\n", json_root);
		get_json_string(temp->measure_class, temp->avg, temp->now, temp->max, temp->time_max, temp->min, temp->time_min, json_temp);
		printf("Temp string: %s\n", json_temp);
		get_json_string(humi->measure_class, humi->avg, humi->now, humi->max, humi->time_max, humi->min, humi->time_min, json_humi);
		printf("Humi string: %s\n", json_humi);
		get_json_string(pressure->measure_class, pressure->avg, pressure->now, pressure->max, pressure->time_max, pressure->min, pressure->time_min, json_pressure);
		printf("Pressure string: %s\n", json_pressure);
		get_json_wstring(wind->avg, wind->now, wind->max, wind->time_max, wind->max_dir, wind->min, wind->time_min, json_wind);
		printf("Wind string: %s\n", json_wind);

		get_json(json_root, json_temp, json_humi, json_pressure, json_wind, json);
		printf("Json string: %s\n", json);
		puts("-------------- JSON STRINGS  -------------");
	
		restart_watchdog();

		if(i == 0) {
			get_json_array_root(json, temp_string);
			strcat(json_array, temp_string);

			if(i == (final_value_index -1))
				strcat(json_array, "]");

		} else if(i == (final_value_index - 1)) {
			get_json_array_end(json, temp_string);
			strcat(json_array, temp_string);
		} else if(i < (final_value_index - 1)) {
			get_json_array_ele(json, temp_string);
			strcat(json_array, temp_string);

			if(i == (final_value_index - 1))
				strcat(json_array, "]");
		}

		free(current->temp);
		free(current->humi);
		free(current->pressure);
		free(current->wind);

		free(json_root);
		free(json_temp);
		free(json_humi);
		free(json_pressure);
		free(json_wind);
		free(json);
	}
	free(temp_string);

	printf("JSON_ARRAY\n%s\n", json_array);

	restart_watchdog();
	send_json(json_array);
	restart_watchdog();

	free(json_array);

	final_value_index = 0;
}

void wait_for_whole_min(void) {
	for(;;) {
		restart_watchdog();
		datetime = get_current_time();
		if(datetime->tm_sec == 0)
			break;

		NutSleep(100);
		restart_watchdog();
	}
	puts("Whole min");
}

void wait_30_sec(void) {
	for(;;) {
		datetime = get_current_time();
		if(measured == 1) { //Venter slik at vi ikke gjør flere målinger hvert 30.sek.
			while(1) {
				datetime = get_current_time();
				if((datetime->tm_sec % 30) != 0)
					break;

				restart_watchdog();
			}
			measured = 0;
		}

		if((datetime->tm_sec % 30) == 0)
			break;
		NutSleep(100);
		restart_watchdog();
	} 
	puts("30 sec");
}

void init_arrays(void) {
	uint8_t i;
	measure_t *p = temp_list,
		*q = humi_list,
		*r = pressure_list,
		*s = wind_speed_list,
		*t = wind_dir_list;
	final_value_t *u = final_values;

	for(i = 0; i < MEASURE_ARR_MAX; i++, p++, q++, r++, s++, t++) {
		p = malloc(sizeof(measure_t));
		q = malloc(sizeof(measure_t));
		r = malloc(sizeof(measure_t));
		s = malloc(sizeof(measure_t));
		t = malloc(sizeof(measure_t));
	}

	for(i = 0; i < FINAL_ARR_MAX; i++, u++) {
		u = malloc(sizeof(final_value_t));
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
	disable_watchdog();
	uint32_t baud = 115200;
	uint8_t i;

	bmp180_initialized = 0;

	measure_index = 0;
	final_value_index = 0;
	datetime = malloc(sizeof(tm));

	init_arrays();

	configure_debug(baud); // Setter output til serieutgang.

	configure_network(); // Initialiserer ethernet.

// HOVEDPROGRAM

	puts("Initializing...");

	set_time_ntp(); // Setter klokken.

	start_watchdog();

	adc_init(); // Initialiserer ADC.

	restart_watchdog();

	i = bmp180_init();// Initialiserer BMP180.

	if(i == 0) //Klarte å initialisere
		bmp180_initialized = 1;
	else
		puts("BMP180 not initialized");

	restart_watchdog();

	puts("Project Hessdalen weather station");

	wait_for_whole_min(); // Venter på helt minutt før vi begynner å måle.

	for (;;) { //Hovedløkke.
		restart_watchdog();
		read_sensors();
		restart_watchdog();
		measured = 1;

		if(((datetime->tm_min % 5) == 0) && (datetime->tm_sec == 0)) { //Regner ut gjennomsnitt og max/min hvert 5. min.
			restart_watchdog();
			prepare_data();
			restart_watchdog();

			if((datetime->tm_min == 0) && (datetime->tm_sec == 0)) { //Sender data hver hele time.
				send_data();
				for(;;) {
					restart_watchdog();
					NutSleep(500);
				}
			}
		}
		wait_30_sec(); //Gjør ny måling hvert 30. sekund.
	}

	for(;;);

	return 0;
}

