#include "adc.h"

enum { WSPEED, WDIR };

void lm35_temp_read(double *temp)
{
	double voltage;

	adc_read_single(&voltage);
	*temp = (voltage - 0.5) * 100; // Er ikke garantert riktig.
}

void wind_data_read(double *speed, double *dir)
{
	double voltage;

	adc_read_diff(&voltage, WSPEED);
	*speed = (voltage * 50) / 5;

	adc_read_diff(&voltage, WDIR);
	*dir = (voltage * 360) / 5;
}

void adc_init(void)
{
	ADMUX = (1 << REFS0); //Setter Vref til AVCC = 5V.
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //Skrur på ADC og setter ADC-klokkehastighet til 115.2kHz. Må være mellom 50-200kHz for 10-bits konvertering.
	MCUCSR |= (1 << JTD); // Skrur av JTAG for å bruke ADC-bits 4-7.
	MCUCSR |= (1 << JTD); // Må repeteres innenfor to cycles for å endre verdien, ifølge datablad.
}

void adc_set_channel(uint8_t mode)
{
	switch(channel) {
		case WSPEED:
			ADMUX = (1 << REFS0) | (1 << MUX4); //Positiv inngang på ADC0, negativ på ADC1.
			break;
		case WDIR:
			ADMUX = (1 << REFS0) | (1 << MUX4) | (1 << MUX3) | (1 << MUX1) | (1 << MUX0); //Positiv inngang på ADC3, negativ på ADC2.
			break;
		default:
			break;
	}
}

void adc_read_single(double *data)
{
	ADMUX = (1 << REFS0) | (1 << MUX2); //Setter inngang til ADC4.
	
}

void adc_read_diff(double *data, uint8_t mode)
{
	int16_t raw;
	uint8_t msb, lsb;

	adc_set_channel(mode);

	ADCSRA |= (1 << ADSC); //Starter konvertering.

	while (ADCSRA & (1 << ADSC)); //Venter på at konverteringen er ferdig.

	lsb = ADCL; //Leser verdien på ADC-en.
	msb = ADCH;

	raw = (msb << 8) | (lsb);
	
	printf("ADC Value: %d\n". raw);

	*data = ((double)raw * 5) / 1023; //Gjør om til spenning.
}

