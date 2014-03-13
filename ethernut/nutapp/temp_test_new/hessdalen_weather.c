#include "hessdalen_weather.h"
#include "sht10_new.h"

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
	double temp = 0, humi = 0;

	
	configure_debug(baud);
	//configure_network();
	
	puts("Project Hessdalen New Temp Read Test");
	
	NutSleep(100);

	initiate_ports();
	sht10_connectionreset();

	temp = read_temperature_c();
	humi = read_humidity();

	printf("Temp: %lf\nHumi: %lf\n", temp, humi);
	
	for (;;) {
		NutSleep(1000);
	}
	return 0;
}
