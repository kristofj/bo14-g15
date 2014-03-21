#ifndef ADC_H_
#define ADC_H_

#include "hessdalen_weather.h"

//Måler vindhastighet i m/s.
void wspeed_read(double *speed);

//Måler vindretning i grader.
void wdirection_read(double *direction);

//Initialiserer ADC-en.
void adc_init(void);

//Måler av spenningen på ADC-en.
void adc_read(double *data, uint8_t mode);

#endif
