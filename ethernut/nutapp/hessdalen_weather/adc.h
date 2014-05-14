#ifndef ADC_H_
#define ADC_H_

#include "hessdalen_weather.h"

//Initialiserer ADC-en. Må kalles før wind_data_read
void adc_init(void);

//Regner ut vindhastighet og vindretning.
void wind_data_read(double *speed, double *dir);

//Velger inngang på ADC-en.
void adc_set_channel(uint8_t channel);

//Måler spenningen på ADC-en.
void adc_read(double *data, uint8_t mode);

#endif
