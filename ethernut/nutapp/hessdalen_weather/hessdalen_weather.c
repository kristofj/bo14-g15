#include "hessdalen_weather.h"

#define ETHERNUT_1	1
#define ETHERNUT_2	2

static m_node *sht10_val;
static m_node *bmp180_val;
static m_node *wind_val;

static node_t *fin_values;

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
	double temp, humi, dew, bmp180_temp, wspeed, wdirection;
	int32_t pressure;
	m_node_t *sth10, *bmp180, *wind;
	char datetime[20];

	sprintf(datetime, "%d-%d-%d %d:%d:%d", (datetime->tm_year + 1900), (datetime->tm_mon + 1), datetime->tm_mday, datetime->tm_hour, datetime->tm_min, datetime->tm_sec);

	sht10_measure(&temp, &humi, &dew);
	bmp180_read_data(&pressure, &bmp180_temp);
	wspeed_read(&wspeed);
	wdirection_read(&wdirection);

	sht10 = malloc(sizeof(m_node_t));
	sht10->datetime = datetime;
	sht10->v1 = temp;
	sht10->v2 = humi;

	bmp180 = malloc(sizeof(m_node_t));
	bmp180->datetime = datetime;
	bmp180->v1 = (double) pressure;
	bmp180->v2 = bmp180_temp;

	wind = malloc(sizeof(m_node_t));
	wind->datetime = datetime;
	wind->v1 = wspeed;
	wind->v2 = wdirection;

	m_push(sht10_val, sht10);
	m_push(bmp180_val, bmp180);
	m_push(wind_val, wind);
}

void prepare_sht10_data(node_t *ret)
{
	m_node_t *sht10_current;
	uint16_t temp_num = 0, humi_sum = 0;
	double temp_sum = 0.0, humi_sum = 0.0, 
		temp_max, temp_min, humi_max, humi_min, temp, humi;
	char *dt;
	node_t *new = malloc(sizeof(node_t));

	current = m_pop(sht10_val);
	dt = current->datetime;

	while(current != NULL) {
		temp_num++;
		humi_num++;
		
		temp = current->v1;
		humi = current->v2;

		if(temp > max)
			temp_max = temp;
		if(temp < min)
			temp_min = temp;
		if(humi > max)
			humi_max = humi;
		if(humi < min)
			humi_min = humi;
		
		temp_sum += temp;
		humi_sum += humi;
	}

}

void prepare_data(void)
{
	uint8_t i;
	m_node_t *sht10_current, *bmp180_current, *wind_current;
	node_t *new = NULL;
	char temp[] = "temp";
	double temp_measure;


	if(!(m_list_length(sht10_val) > 0))
		return;

	sht10_current = m_pop(sht10_val);
	bmp180_current = m_pop(bmp180_val);
	wind_current = m_pop(wind_val);

	while(sht10_current != NULL) {
		


	}

}

void send_data(void)
{

}

void wait_for_whole_min(tm *datetime) {
	do {
		get_current_time(&datetime);
	} while (datetime->tm_sec == 0);
}

void wait_ten_sec(tm *datetime) {
	do {
		get_current_time(&datetime);
	} while((datetime->tmsec % 10) == 0);
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

	sht10_val = malloc(sizeof(m_node_t));
	bmp180_val = malloc(sizeof(m_node_t));
	wind_val = malloc(sizeof(m_node_t));

	fin_val = malloc(sizeof(node_t));

	configure_debug(baud);
	configure_network();
	
	set_time_ntp(&datetime);

	puts("Project Hessdalen weather station");
	
	wait_for_whole_min(&datetime);

	for (;;) { //Hovedløkke.
		read_sensors();

		if((datetime->tm_min % 5) == 0) { //Regner ut gjennomsnitt og max/min hvert 5. min.
			prepare_data();

			if(datetime->tm_min == 0) { //Sender data hver hele time.
				send_data();
			}
		}
		wait_ten_sec(&datetime); //Gjør ny måling hvert tiende sekund.
	}
	return 0;
}
