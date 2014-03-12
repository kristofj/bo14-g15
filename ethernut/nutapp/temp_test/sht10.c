#include "sht10.h"

enum {TEMP, HUMI};

#define NOP() 		asm volatile("nop")
#define SCK_PORT	NUTGPIO_PORTB
#define SCK_PIN		0
#define DATA_PORT	NUTGPIO_PORTB
#define DATA_PIN	1
#define SCK_LOW()	cbi(PORTB, 0);
#define SCK_HIGH()	sbi(PORTB, 0);
#define DATA_LOW()	cbi(PORTB, 1);
#define DATA_HIGH()	sbi(PORTB, 1);

#define noACK		0
#define ACK		1
				//adr  command  r/w
#define STATUS_REG_W 	0x06	//000   0011    0
#define STATUS_REG_R 	0x07	//000   0011    1
#define MEASURE_TEMP 	0x03	//000   0001    1
#define MEASURE_HUMI 	0x05	//000   0010    1
#define RESET        	0x1e	//000   1111    0

char sht10_write_byte(unsigned char value);
char sht10_read_byte(unsigned char ack);
void sht10_connectionreset(void);
void sht10_transstart(void);
uint32_t sht10_softreset(void);
uint32_t sht10_read_statusreg(uint8_t *p_value, uint8_t *p_checksum);
uint32_t sht10_write_statusreg(uint8_t *p_value);
char sht10_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode);
void calc_sht10(double *p_humidity, double *p_temperature);
double calc_dewpoint(double h, double t);
unsigned char sht10_get_data(sht10_data *data);
unsigned char read_data(void);
void set_data_output(void);
void set_data_input(void);
void initiate_ports(void);

char sht10_write_byte(unsigned char value) {
	set_data_output();
	
	unsigned char error = 0, i;
	
	for(i=0x80; i>0; i/=2) {
		if(i & value) {
			DATA_HIGH();
		}
		else {
			DATA_LOW();
		}
		NutMicroDelay(1);
		SCK_HIGH();
		NutMicroDelay(10);
		SCK_LOW();
		NutMicroDelay(1);
	}

	DATA_HIGH();
	set_data_input();

	SCK_HIGH();
	NutSleep(330);
	error = read_data();
	SCK_LOW();

	if(error == 1)
		puts("Error in sht10_write_byte");

	return error; //error=1 hvis det ikke kommer ack fra sht10.
}

char sht10_read_byte(unsigned char ack)
{
	unsigned char i, val = 0;
	set_data_output();
	DATA_HIGH();
	set_data_input();
	
	for(i=0; i<8; i++) {
		SCK_HIGH();
		NutMicroDelay(10);
		val = val*2 + read_data();
		SCK_LOW();
	}
	set_data_output();
	if(ack == 1)
		DATA_LOW();
	NutMicroDelay(1);
	SCK_HIGH();
	NutMicroDelay(10);
	SCK_LOW();
	NutMicroDelay(1);
	DATA_HIGH();
	set_data_input();
	return val;
}

void sht10_connectionreset(void)
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart
//       _____________________________________________________         ________
// DATA:                                                      |_______|
//          _    _    _    _    _    _    _    _    _        ___     ___
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______
{
	unsigned char i; 
	set_data_output();
	
	DATA_HIGH(); SCK_LOW();		//Initial state
	for(i=0; i<9; i++) { 		//9 SCK cycles
		SCK_HIGH();                  
		SCK_LOW();
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

char sht10_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)
{
	unsigned char error = 0;
	unsigned int i;
	
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
	*p_value = sht10_read_byte(ACK); //Leser første byte.
	*p_value *= 256;
	*p_value |= sht10_read_byte(ACK); //Leser andre byte.
	*p_checksum = sht10_read_byte(noACK); //Leser checksum.

	printf("p_value in sht10_measure: %d\n", *p_value);
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

unsigned char sht10_get_data(sht10_data *data)
{
	unsigned char error = 0, checksum;
	//sht10_data *_data = (sht10_data *) data;
	initiate_ports();

	//sht10_connectionreset();
	
	error += sht10_measure((unsigned char *)&data->temp, &checksum, TEMP);
	error += sht10_measure((unsigned char *)&data->humi, &checksum, HUMI);

	if(error != 0) {
		return error; //Det skjedde noe feil, returnerer feilen.
	}

	calc_sht10((double *)&data->humi, (double *)&data->temp);
	data->dew = calc_dewpoint(data->humi, data->temp);
	return error;
}

unsigned char read_data(void)
{
	set_data_input();

	return GpioPinGet(DATA_PORT, DATA_PIN);
}

void set_data_output(void)
{
	sbi(DDRB, 1);
}

void set_data_input(void)
{
	cbi(DDRB, 1);
	cbi(PORTB, 1);
}

void initiate_ports(void)
{
	sbi(DDRB, 0);
	sbi(DDRB, 1);
}
