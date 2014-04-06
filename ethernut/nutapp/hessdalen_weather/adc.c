#include "adc.h"

enum {WSPEED, WDIR};

void wind_data_read(double *speed, double *dir)
{
	double voltage;

	adc_read(&voltage, WSPEED);
	*speed = (voltage * 50) / 2.5;

	adc_read(&voltage, WDIR);
	*dir = (voltage * 360) / 2.5;
}

void adc_init(void)
{
	ADCInit();
	ADCSetRef(AVCC);
}

void adc_read(double *data, uint8_t mode)
{
	uint16_t raw;

	switch(mode) {
		case WSPEED: //Vindhastighet er på ADC0.
			ADCSetChannel(ADC0);
			break;
		case WDIR: //Vindretning er på ADC1.
			ADCSetChannel(ADC1);
			break;
		default:
			break;
	}

	ADCStartConversion();

	while(ADCRead(&raw))
		NutThreadYield;

	*data = ((double)raw * 2.5) / 511.5; //Gjør om til spenning.
}

