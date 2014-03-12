#include "hessdalen_weather.h"
#include "sht10.h"

//Registerer output på serieutgang for debug.
void configure_debug(uint32_t baud)
{
	NutRegisterDevice(&DEV_DEBUG, 0, 0);
	freopen(DEV_DEBUG_NAME, "w", stdout);
	_ioctl(_fileno(stdout), UART_SETSPEED, &baud);
}

int main(void)
{
	uint32_t baud = 115200;
	sht10_data data;
	uint32_t error = 0;

	configure_debug(baud);

	puts("I'm not working");

	//configure_network();
	/*
	puts("Project Hessdalen weather station");
	
	error = sht10_get_data(&data);
	
	if(error == 0) {
		printf("Temp: %lf\nHumi: %lf\nDew: %lf\n", data.temp, data.humi, data.dew);
	}
	else {
		puts("Error in main");
	}
	*/

	for (;;) {
		NutSleep(1000);
	}
	return 0;
}
