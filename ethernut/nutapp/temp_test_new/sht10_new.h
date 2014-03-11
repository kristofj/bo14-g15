#include "hessdalen_weather.h"
#include <dev/gpio.h>
#include <compiler.h>

void sht10_connectionreset(void);
void initiate_ports(void);
double read_temperature_c();
double read_humidity();

