#ifndef ADC_H_
#define ADC_H_

#include "hessdalen_weather.h"
#include <dev/adc.h>

//Leser vindhastighet og vindretning.
void  wind_data_read(double *speed, double *dir);

//Initialiserer ADC-en. Må kalles før wind_data_read
void adc_init(void);

//Måler av spenningen på ADC-en.
void adc_read(double *data, uint8_t mode);

#endif
