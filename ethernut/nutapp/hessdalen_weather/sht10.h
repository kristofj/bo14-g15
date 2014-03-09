#ifndef SHT10_H
#define SHT10_H

#include "hessdalen_weather.h"
#include <dev/gpio.h>
#include <math.h>

typedef struct {
	double temp;
	double humi;
	double dew;
} sht10_data;

uint32_t sht10_get_data(sht10_data *data);

#endif
