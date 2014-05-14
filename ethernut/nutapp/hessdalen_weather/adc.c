#include "adc.h"

enum { WSPEED, WDIR }; //Skiller på vindhastighet og vindretning.

void adc_init(void)
{
	ADMUX = (1 << REFS0); //Setter Vref til AVCC = 5V.
	//Skrur på ADC og setter ADC-klokkehastighet til 115.2kHz. Må være mellom 50-200kHz for 10-bits konvertering.
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

void wind_data_read(double *speed, double *dir)
{
	double voltage;

	adc_read(&voltage, WSPEED); //Leser spenning.
	*speed = (voltage * 50) / 5; //Konverterer til hastighet mellom 0-50 m/s

	adc_read(&voltage, WDIR); //Leser spenning.
	*dir = (voltage * 360) / 5; //Konverterer til grader mellom 0-360.
}

void adc_set_channel(uint8_t mode)
{
	switch(mode) {
		case WSPEED:
			ADMUX = (1 << REFS0); //Endrer til kanal ADC0.
			break;
		case WDIR:
			ADMUX = (1 << REFS0) | (1 << MUX0); //Endrer til kanal ACD1.
			break;
		default:
			break;
	}
}

void adc_read(double *data, uint8_t mode)
{
	int16_t raw;
	uint8_t msb, lsb;

	adc_set_channel(mode);

	ADCSRA |= (1 << ADSC); //Starter konvertering.

	while (ADCSRA & (1 << ADSC)); //Venter på at konverteringen er ferdig.

	lsb = ADCL; //Leser verdien på ADC-en.
	msb = ADCH;

	raw = (msb << 8) | (lsb);
	
	*data = ((double)raw * 5) /  1023; //Gjør om til spenning.
}
