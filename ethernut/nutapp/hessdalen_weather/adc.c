#include "adc.h"

enum {WSPEED, WDIR};

void wspeed_read(double *speed)
{
	double voltage;

	adc_read(&voltage, WSPEED);

	*speed = (voltage * 50) / 5; //Konverterer til hastighet i m/s.
}

void wdirection_read(double *direction)
{
	double voltage;

	adc_read(&voltage, WDIR);

	*direction = (voltage * 360) / 5; //Konverterer til retning i grader.
}

void adc_init(void)
{
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Aktiverer ADC. Setter inputfrekvens til ~115kHz. Må være mellom 50-200kHz ifølge datablad.
	ADMUX |= (1 << REFS0); //Setter refereansespenning til AVCC.
}

void adc_read(double *data, uint8_t mode)
{
	uint16_t raw;

	switch(mode) {
		case WSPEED: //Vindhastighet er på ADC0.
			ADMUX |= (0 << MUX0);
			break;
		case WDIR: //Vindretning er på ADC1.
			ADMUX |= (1 << MUX0);
			break;
		default:
			break;
	}

	ADCSRA |= (1 << ADSC); //Starter konverting.
	while(ADCSRA & (1 << ADSC)); //Venter på at konverting er ferdig.

	raw = ADCH | ADCL; //Leser rådata fra dataregister.
	*data = ((double)raw * 5) / 1023; //Gjør om til spenning.
}

