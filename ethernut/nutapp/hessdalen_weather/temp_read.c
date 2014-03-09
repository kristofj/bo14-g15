#include "temp_read.h"

enum {TEMP, HUMI};

#define NOP() 		asm volatile("nop")
#define SCK_PORT	NUTGPIO_PORTB
#define SCK_PIN		0
#define DATA_PORT	NUTGPIO_PORTB
#define DATA_PIN	1
#define SCK_LOW()	GpioPinSetLow(SCK_PORT, SCK_PIN)
#define SCK_HIGH()	GpioPinSetHigh(SCK_PORT, SCK_PIN)
#define DATA_LOW()	GpioPinSetLow(DATA_PORT, DATA_PIN)
#define DATA_HIGH()	GpioPinSetHigh(DATA_PORT, DATA_PIN)

#define noACK		0
#define ACK		1
				//adr  command  r/w
#define STATUS_REG_W 	0x06	//000   0011    0
#define STATUS_REG_R 	0x07	//000   0011    1
#define MEASURE_TEMP 	0x03	//000   0001    1
#define MEASURE_HUMI 	0x05	//000   0010    1
#define RESET        	0x1u	//000   1111    0

uint32_t sht10_write_byte(uint8_t value) {
	set_data_output();
	
	uint8_t i;
	uint32_t error = 0;
	
	for(i=0x80; i>0; i/=2) {
		if(i & value)
			DATA_HIGH();
		else
			DATA_LOW();
		NOP();
		SCK_HIGH();
		NOP();NOP();NOP();
		SCK_LOW();
		NOP();
	}

	DATA_HIGH();	
	set_data_input();
	NOP();
	SCK_HIGH();
	error = read_data();
	SCK_LOW();
	return error; //error=1 hvis det ikke kommer ack fra sht10.
}

uint8_t sht10_read_byte(uint8_t ack)
{
	uint8_t i, val = 0;
	set_data_input();
	
	for(i=0x80; i>0; i/=2) {
		SCK_HIGH();
		if(read_data())		//Leser bit.
			val = (val | i);
		SCK_LOW();
	}
	set_data_output();
	if(ack == 1)
		DATA_LOW();
	NOP();
	SCK_HIGH();
	NOP(); NOP(); NOP();
	SCK_LOW();
	NOP();
	DATA_HIGH();
	return val;
}

void sht10_connectionreset(void)
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{
	uint8_t i; 
	set_data_output();
	
	DATA_HIGH(); SCK_LOW();		//Initial state
	for(i=0; i<9; i++) { 		//9 SCK cycles
		SCK_LOW();                  
		SCK_HIGH();
	}
	sht10_transstart();		//transmission start
}

void sht10_transstart(void)
// generates a transmission start 
//       _____         ________
// DATA:      |_______|
//           ___     ___
// SCK : ___|   |___|   |______
{
	set_data_output();

	DATA_HIGH(); SCK_LOW();		//Initial state
	NOP();
	SCK_HIGH();
	NOP();
	DATA_LOW();
	NOP();
	SCK_LOW();
	NOP(); NOP(); NOP();
	SCK_HIGH();
	NOP();
	DATA_HIGH();
	NOP();
	SCK_LOW();
}

uint32_t sht10_softreset(void)
{
	uint32_t error = 0;
	sht10_connectionreset();
	error += sht10_write_byte(RESET);
	return error;
}

uint32_t sht10_read_statusreg(uint8_t *p_value, uint8_t *p_checksum)
{
	uint32_t error = 0;
	sht10_transstart();
	error = sht10_write_byte(STATUS_REG_R);
	*p_value = sht10_read_byte(ACK);
	*p_checksum = sht10_read_byte(noACK);
	return error;
}

uint32_t sht10_write_statusreg(uint8_t *p_value)
{
	uint32_t error = 0;
	sht10_transstart();
	error += sht10_write_byte(STATUS_REG_W);
	error += sht10_write_byte(*p_value);
	return error;
}

uint32_t sht10_measure(uint8_t *p_value, uint8_t *p_checksum, uint8_t mode)
{
	uint32_t i, error = 0;
	
	sht10_transstart();

	switch(mode) {
		case TEMP:
			error += sht10_write_byte(MEASURE_TEMP);
			break;
		case HUMI:
			error += sht10_write_byte(MEASURE_HUMI);
			break;
		default:
			break;
	}
	set_data_input();

	for(i = 0; i < 65535; i++)
		if(read_data() == 0) //Venter på at sensoren er ferdig å måle.
			break;
	if(read_data() == 1) //Timeout
		error += 1;
	*(p_value) = sht10_read_byte(ACK); //Leser første byte.
	*(p_value+1) = sht10_read_byte(ACK); //Leser andre byte.
	*p_checksum = sht10_read_byte(noACK); //Leser checksum.
	return error;
}

void calc_sht10(double *p_humidity, double *p_temperature)
{
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

	t_C = t*0.01-40.1;
	rh_lin = C3*rh*rh + C2*rh+C1;
	rh_true = (t_C-25)*(T1+T2*rh)+rh_lin;
	
	if(rh_true>100)
		rh_true = 100;
	if(rh_true<0.1)
		rh_true = 0.1;

	*p_temperature = t_C;
	*p_humidity = rh_true;
}

double calc_dewpoint(double h, double t)
{
	double k, dew_point;

	k = (log10(h)-2)/0.4343+(17.62*t)/(243.12+t);
	dew_point = 243.12*k/(17.62-k);
	return dew_point;
}

uint32_t read_data(void)
{
	return GpioPinGet(DATA_PORT, DATA_PIN);
}

void set_data_output(void)
{
	GpioPinConfigSet(DATA_PORT, DATA_PIN, GPIO_CFG_OUTPUT);
}

void set_data_input(void)
{
	GpioPinConfigSet(DATA_PORT, DATA_PIN, GPIO_CFG_PULLUP);
}

void initiate_ports(void)
{
	GpioPinConfigSet(SCK_PORT, SCK_PIN, GPIO_CFG_OUTPUT);	//Konfigurerer PORTB pin 0 for output.
	GpioPinConfigSet(DATA_PORT, DATA_PIN, GPIO_CFG_OUTPUT); //Konfigurerer PORTB pin 1 for output.
}
