#include "hessdalen_weather.h"
#include "network.h"
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
	uint32_t error = 0;
	sht10_data data;

	configure_debug(baud);
	//configure_network();
	
	puts("Project Hessdalen temp read test");

	error = sht10_get_data(&data);

	if(error == 0)
		printf("Temp: %lf\nHumi: %lf\nDew: %lf\n");
	else
		puts("Error reading sht10");
	
	for (;;) {
		NutSleep(1000);
	}
	return 0;
}
