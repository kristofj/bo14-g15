#ifndef ADC_H_
#define ADC_H_

#include "hessdalen_weather.h"

//Leser vindhastighet og vindretning.
void wind_data_read(double *speed, double *dir);

//Initialiserer ADC-en. Må kalles før wind_data_read
void adc_init(void);

//Velger inngang på ADC-en.
void adc_set_channel(uint8_t channel);

void adc_read_single(double *data);

//Måler av spenningen på ADC-en.
void adc_read_diff(double *data, uint8_t mode);

#endif
