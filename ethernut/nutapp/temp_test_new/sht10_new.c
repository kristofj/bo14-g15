#include "sht10_new.h"

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
#define LOW		0
#define HIGH		1

#define noACK		0
#define ACK		1
				//adr  command  r/w
#define STATUS_REG_W 	0x06	//000   0011    0
#define STATUS_REG_R 	0x07	//000   0011    1
#define MEASURE_TEMP 	0x03	//000   0001    1
#define MEASURE_HUMI 	0x05	//000   0010    1
#define RESET        	0x1e	//000   1111    0

double read_temperature_c(void);
double read_humidity(void);
double read_temperature_raw(void);
int shift_in(int _num_bits);
void shift_out(unsigned char cmd);
char send_cmd_sht(unsigned char cmd);
char wait_for_result(void);
long get_data(void);
void skip_crc(void);
void sht10_connectionreset(void);
void sht10_transstart(void);
double calc_dewpoint(double h, double t);
uint32_t read_data(void);
void set_data_output(void);
void set_data_input(void);
void initiate_ports(void);

double read_temperature_c(void)
{
	double _val;
	double _temp;

	const double D1 = -40.1;
	const double D2 = 0.01;

	_val = read_temperature_raw();
	
	_temp = D1 + (D2 * (double)_val);

	return _temp;
}

double read_humidity(void)
{
	long _val;
	double _lin_humi;
	double _true_humi;
	double _temp;
	
	const double C1 = -2.0468;
	const double C2 = 0.0367;
	const double C3 = -0.0000015955;
	const double T1 = 0.01;
	const double T2 = 0.00008;

	send_cmd_sht(MEASURE_HUMI);
	wait_for_result();
	_val = get_data();
	skip_crc();
	
	_lin_humi = C1 + C2 * _val + C3 * (double)_val * (double)_val;

	_temp = read_temperature_c();

	_true_humi = (_temp - 25.0) * (T1 + T2 * (double)_val) + _lin_humi;

	return _true_humi;
}

double read_temperature_raw(void)
{
	long _val;

	send_cmd_sht(MEASURE_TEMP);
	wait_for_result();
	_val = get_data();
	skip_crc();

	return _val;
}

int shift_in(int _num_bits)
{
	int ret = 0;
	int i;
	
	set_data_input();
	
	for(i=0x80; i>0; i/=2) {
		SCK_HIGH();
		NutMicroDelay(10);
		if(read_data())
			ret |= i;
		SCK_LOW();
	}
	printf("Byte read: %d\n",ret);
	return ret;
}

void shift_out(unsigned char cmd)
{
	unsigned char i;

	set_data_output();

	for(i=0; i<8; i++) {
		SCK_HIGH();
		if(cmd & 0x80) {
			DATA_HIGH();
		}
		else {
			DATA_LOW();
		}
		NutMicroDelay(10);
		SCK_LOW();

		cmd <<= 1;
	}
}

char send_cmd_sht(unsigned char cmd)
{
	int ack;

	sht10_transstart();

	shift_out(cmd);
	NutSleep(330);

	SCK_HIGH();
	set_data_input();
	ack = read_data();
	if(ack == HIGH){
		puts("Error in send_cmd_sht : -1");
		return -1;
	}
	//NutMicroDelay(10);
	SCK_LOW();
	//NutMicroDelay(10);
	ack = read_data();
	if(ack == LOW) {
		puts("Error in send_cmd_sht : -2");
		return -2;
	}
	return 0;
}

char wait_for_result(void)
{
	int i;
	int ack;

	set_data_input();

	for(i=0; i<100; i++) {
		NutMicroDelay(10);
		ack = read_data();

		if(ack == LOW)
			break;
	}

	if(ack == HIGH) {
		puts("Error in wait_for_result");
		return -1;
	}
	return 0;
}

long get_data(void)
{
	long val;
	long a;

	set_data_input();
	val = shift_in(8);

	val *= 256;

	set_data_output();
	DATA_HIGH();
	DATA_LOW();
	SCK_HIGH();
	SCK_LOW();

	set_data_input();
	/*
	a = shift_in(8);
	a *=256;
	val |= a;
	*/
	val |= shift_in(8);

	return val;
}

void skip_crc(void)
{
	set_data_output();

	DATA_HIGH();
	SCK_HIGH();
	SCK_LOW();
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

	//DATA_HIGH(); SCK_LOW();		//Initial state
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

double calc_dewpoint(double h, double t)
{
	double k, dew_point;

	k = (log10(h)-2)/0.4343+(17.62*t)/(243.12+t);
	dew_point = 243.12*k/(17.62-k);
	return dew_point;
}

uint32_t read_data(void)
{
	set_data_input();

	unsigned char a;
	a = GpioPinGet(NUTGPIO_PORTB, 1);
	return a;
}

void set_data_output(void)
{
	sbi(DDRB, 1);
}

void set_data_input(void)
{
	cbi(DDRB, 1);
	//cbi(PORTB, 1); //Skrur av pull-up resistor.
}

void initiate_ports(void)
{
	sbi(DDRB, 0);
	sbi(DDRB, 1);
}
