#include "hessdalen_weather.h"
#include <dev/gpio.h>
#include <compiler.h>
#include <math.h>

void sht10_connectionreset(void);
void initiate_ports(void);
double read_temperature_c(void);
double read_humidity(void);

