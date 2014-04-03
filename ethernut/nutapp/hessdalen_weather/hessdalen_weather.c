#include "hessdalen_weather.h"

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
	double temp, humi, dew, bmp180_temp, wspeed, wdirection;
	int32_t pressure;

	sht10_measure(&temp, &humi, &dew);
	bmp180_read_data(&pressure, &bmp180_temp);
	wspeed_read(&wspeed);
	wdirection_read(&wdirection);
}

void prepare_data(void)
{
	
}

void send_data(void)
{

}

void wait_for_whole_min(tm *datetime) {
	do {
		get_current_time(&datetime);
	} while (datetime->tm_sec == 0);
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

	configure_debug(baud);
	configure_network();
	
	set_time_ntp();

	puts("Project Hessdalen weather station");
	
	for (;;) { //Hovedløkke.
		wait_for_whole_min(&datetime);

		read_sensors();

		wait_for_whole_min(&datetime);
		
		if((datetime->tm_min % 5) == 0) { //Regner ut gjennomsnitt og max/min hvert 5. min.
			prepare_data();

			if(datetime->tm_min == 0) { //Sender data hver hele time.
				send_data();
			}
		}
	}
	return 0;
}
