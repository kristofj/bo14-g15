#ifndef BMP180_H
#define BMP180_H

#include "hessdalen_weather.h"

uint8_t bmp180_read_data(int32_t *pressure, double *temperature);
uint8_t bmp180_init(void);

#endif
