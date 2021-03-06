#include "sht10.h"

enum {TEMP, HUMI};

void initiate_pins(void)
{
	//Setter pins til output.
	GpioPinConfigSet(PORT_B, SCK_PIN, GPIO_CFG_OUTPUT);
	GpioPinConfigSet(PORT_B, DATA_PIN, GPIO_CFG_OUTPUT);
}

uint8_t sht10_measure(double *temp, double *humi)
{
	uint8_t error;
	uint16_t humival_raw, tempval_raw;
	double humival_d, tempval_d, dew_point_d;
	
	initiate_pins();
	restart_watchdog();
	reset_connection();
	restart_watchdog();

	if((error = read_sensor_raw(&tempval_raw, TEMP)) != 0)
		return error;

	restart_watchdog();

	if((error = read_sensor_raw(&humival_raw, HUMI)) != 0)
		return error;

	restart_watchdog();

	humival_d = (double) humival_raw;
	tempval_d = (double) tempval_raw;

	extract_values(&humival_d, &tempval_d); //Råverdi til temp og fuktighet.
	restart_watchdog();

	*temp = tempval_d;
	*humi = humival_d;

	return 0;
}

uint8_t read_sensor_raw(uint16_t *p_value, uint8_t mode)
{
	uint8_t i = 0;
	*p_value = 0;
	
	start_transmission(); //Starter kommunikasjon.
	
	switch(mode) {
		case TEMP:
			mode = MODE_TEMP;
			break;
		case HUMI:
			mode = MODE_HUMI;
			break;
		default:
			break;
	}
	
	if(write_byte(mode)) { //Skriver kommando.
		return 1;
	}
	
	while(i < 240) { //Venter på at sht10 har målt ferdig data i max 720 ms.
		PULSE_SHORT;
		if(read_data_pin() == 0) {
			i = 0;
			break;
		}
		i++;
	}
	
	if(i) { //Timeout.
		return 1;
	}
	i = read_byte(1); //Leser MSB.
	*p_value = (i << 8) | read_byte(0); //Leser LSB.
	
	return 0;
}

uint8_t read_byte(uint8_t ack)
{
	uint8_t i = 0x80; //Bit-maske.
	uint8_t val = 0;
	
	set_data_input(); //Setter data-pin til input.
	
	while(i) {
		SCK_HIGH;
		PULSE_SHORT;
		if(read_data_pin()) {
			val = (val | i); //Leser en bit
		}
		SCK_LOW;
		PULSE_SHORT;
		i >>= 1;
	}
	set_data_output();
	
	if(ack) { //Sender ACK.
		DATA_LOW;
	} else {
		DATA_HIGH;
	}
	SCK_HIGH;
	PULSE_LONG;
	SCK_LOW;
	set_data_input();
	
	return val;
}

uint8_t write_byte(uint8_t data)
{
	uint8_t i = 0x80; //Bit-maske.
	uint8_t error = 0;

	set_data_output();

	while(i) {
		if(i & data) { //Sender 1.
			DATA_HIGH;
		} else { //Sender 0.
			DATA_LOW;
		}

		SCK_HIGH;
		PULSE_LONG;
		SCK_LOW;
		PULSE_SHORT;
		i >>= 1;
	}
	set_data_input();
	SCK_HIGH;
	PULSE_LONG;
	
	if (read_data_pin()) { //Fikk ikke ack fra sensoren.
		error = 1;
	}
	PULSE_SHORT;
	SCK_LOW;
	
	return error;
}

void start_transmission(void)
{
	SCK_LOW;
	set_data_output();
	DATA_HIGH;
	PULSE_SHORT;
	
	SCK_HIGH;
	PULSE_SHORT;
	
	DATA_LOW;
	PULSE_SHORT;
	
	SCK_LOW;
	PULSE_SHORT;
	
	SCK_HIGH;
	PULSE_SHORT;
	
	DATA_HIGH;
	PULSE_SHORT;
	
	SCK_LOW;
	PULSE_SHORT;

	set_data_input();
}

void reset_connection(void)
{
	uint8_t i;

	SCK_LOW;
	set_data_output();
	DATA_HIGH;

	for(i=0; i<9; i++) {
		SCK_HIGH;
		PULSE_LONG;
		SCK_LOW;
		PULSE_LONG;
	}
	start_transmission();
}

uint8_t soft_reset(void)
{
	reset_connection();
	
	return write_byte(REG_RESET);
}

void extract_values(double *p_humidity, double *p_temperature)
{
	//Paramtere fra datablad.
	const double C1 = -2.0468;
	const double C2 = +0.0367;
	const double C3 = -0.0000015955;
	const double T1 = +0.01;
	const double T2 = +0.00008;

	double rh = *p_humidity;
	double t = *p_temperature;
	double rh_lin;
	double rh_true;
	double t_C;

	t_C = t * 0.01 - 40.1; //Temperaturen i celsius
	rh_lin = C3*rh*rh + C2*rh + C1; //Relativ fuktighet.
	rh_true = (t_C - 25)*(T1+T2*rh)+rh_lin; //Relavitv fuktighet kompansert med temperatur.

	if(rh_true > 100) //Forsikrer at fuktigheten er innenfor fysisk mulige parametere.
		rh_true = 100;
	if(rh_true < 0.1)
		rh_true = 0.1;

	*p_temperature = t_C;
	*p_humidity = rh_true;
}

uint16_t read_data_pin(void)
{
	return(GpioPinGet(PORT_B, DATA_PIN));
}

void set_data_output(void)
{
	GpioPinConfigSet(PORT_B, DATA_PIN, GPIO_CFG_OUTPUT);
}

void set_data_input(void)
{
	//Setter data til input og aktiverer pullup-motstand.
	GpioPinConfigSet(PORT_B, DATA_PIN, GPIO_CFG_PULLUP);
}

