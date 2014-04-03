#include "hessdalen_weather.h"
#include "network.h"

void start_watchdog(uint32_t ms)
{
	NutWatchDogStart(ms, 0);
}

void restart_watchdog(void)
{
	NutWatchDogRestart();
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
	
	configure_debug(baud);
	configure_network();
	
	puts("Project Hessdalen weather station");
	
	for (;;) {
		NutSleep(1000);
	}
	return 0;
}
