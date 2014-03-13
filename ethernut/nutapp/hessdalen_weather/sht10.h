#ifndef SHT10_H
#define SHT10_H

#include "hessdalen_weather.h"
#include <dev/gpio.h>
#include <math.h>

uint8_t sht10_measure(double *temp, double *humi, double *dew);

#endif
