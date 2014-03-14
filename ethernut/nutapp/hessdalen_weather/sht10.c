#include "sht10.h"

/*
Grensesnitt mellom ethernut og SHT10.
Innholder funksjoner som tar seg av kommunikasjon med SHT10.
Råverdiene konverteres til grader celcius og relativt fuktighet.
Disse brukes så til å regne ut duggpunkt.
*/			
				//adr	cmd	r/w
#define MODE_TEMP	0x03	//000	0001	1
#define MODE_HUMI	0x05	//000	0010	1
#define REG_RESET	0x1e	//000	1111	0

#define PULSE_LONG	NutMicroDelay(9)
#define PULSE_SHORT	NutMicroDelay(3)

#define PORT_B		NUTGPIO_PORTB
#define SCK_PIN		0
#define DATA_PIN	1

#define SCK_LOW		GpioPinSetLow(PORT_B, SCK_PIN)
#define SCK_HIGH	GpioPinSetHigh(PORT_B, SCK_PIN)
#define DATA_LOW	GpioPinSetLow(PORT_B, DATA_PIN)
#define DATA_HIGH	GpioPinSetHigh(PORT_B, DATA_PIN)

enum{TEMP, HUMI};

void initiate_pins(void);
uint8_t read_sensor_raw(uint16_t *p_value, uint8_t mode);
uint8_t sht10_measure(double *temp, double *humi, double *dew);
uint8_t write_byte(uint8_t data);
uint8_t read_byte(uint8_t ack);
void start_transmission(void);
void reset_connection(void);
uint8_t soft_reset(void);
void extract_values(double *p_humidity, double *p_temperature);
double get_dew_point(double h, double t);
uint16_t read_data_pin(void);
void set_data_output(void);
void set_data_input(void);

void initiate_pins(void)
//Setter sck-pin og data-pin til output.
{
	GpioPinConfigSet(PORT_B, SCK_PIN, GPIO_CFG_OUTPUT);
	GpioPinConfigSet(PORT_B, DATA_PIN, GPIO_CFG_OUTPUT);
}

uint8_t sht10_measure(double *temp, double *humi, double *dew)
//Leser all nødvendig data fra sht10 og konverterer de til normale verdier.
{
	uint8_t error;
	uint16_t humival_raw, tempval_raw;
	double humival_d, tempval_d, dew_point_d;
	
	initiate_pins();
	reset_connection();

	if((error = read_sensor_raw(&tempval_raw, TEMP)) != 0)
		return error;

	if((error = read_sensor_raw(&humival_raw, HUMI)) != 0)
		return error;

	humival_d = (double) humival_raw;
	tempval_d = (double) tempval_raw;

	extract_values(&humival_d, &tempval_d);
	dew_point_d = get_dew_point(humival_d, tempval_d);

	*temp = tempval_d;
	*humi = humival_d;
	*dew = dew_point_d;

	return 0;
}

uint8_t read_sensor_raw(uint16_t *p_value, uint8_t mode)
//Leser rådata ifra sht10.
{
	uint8_t i = 0;
	*p_value = 0;
	
	start_transmission();
	
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
	
	if(write_byte(mode)) {
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
	
	if(i) {
		return 3;
	}
	i = read_byte(1); //Leser første byte. MSB først.
	*p_value = (i << 8) | read_byte(0); //Leser andre byte. LSB.
	
	return 0;
}

uint8_t read_byte(uint8_t ack)
//Leser en byte og sender ack til sht10 hvis ack er satt.
{
	uint8_t i = 0x80;
	uint8_t val = 0;
	
	set_data_input();
	
	while(i) {
		SCK_HIGH;
		PULSE_SHORT;
		if(read_data_pin()) {
			val = (val | i);
		}
		SCK_LOW;
		PULSE_SHORT;
		i >>= 1;
	}
	set_data_output();
	
	if(ack) {
		DATA_LOW;
	}
	else {
		DATA_HIGH;
	}
	SCK_HIGH;
	PULSE_LONG;
	SCK_LOW;
	set_data_input();
	
	return val;
}

uint8_t write_byte(uint8_t data)
//Sender en byte med data til sht10. Og leser ackownledge fra sensoren.
{
	uint8_t i = 0x80; //Bit-maske.
	uint8_t error = 0;

	set_data_output();

	while(i) {
		if(i & data) {
			DATA_HIGH;
		}
		else {
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
//Starten på en overføring.
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
//Starter om koblingen mellom ethernut og sensoren, kan kalles hvis man har mistet kontakt med sensoren.
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
//Kjører en soft-reset av sensoren. Setter registeret til sht10 til standard verdier.
{
	reset_connection();
	
	return write_byte(REG_RESET);
}

void extract_values(double *p_humidity, double *p_temperature)
//Konverterer rådata fra sensoren til relativ fuktighet og temperatur i celsius.
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

	t_C = t * 0.01 - 40.1; //Temperaturen i celsius
	rh_lin = C3*rh*rh + C2*rh + C1; //Relativ fuktighet.
	rh_true = (t_C - 25)*(T1+T2*rh)+rh_lin; //Relavitv fuktighet kompansert med temperatur.
	if(rh_true > 100)
		rh_true = 100;
	if(rh_true < 0.1)
		rh_true = 0.1;

	*p_temperature = t_C;
	*p_humidity = rh_true;
}

double get_dew_point(double h, double t)
//Regner ut duggpunkt baser på den relative fuktigheten og temperatur.
{
	double log_ex, dew_point;
	
	log_ex = (log10(h)-2)/0.4343 + (17.62*t)/(243.12*t);
	dew_point = 243.12*log_ex/(17.62 - log_ex);
	
	return dew_point;
}

uint16_t read_data_pin(void)
//Henter ut status på data-pin.
{
	return(GpioPinGet(PORT_B, DATA_PIN));
}

void set_data_output(void)
//Setter data-pin til å være utgang.
{
	GpioPinConfigSet(PORT_B, DATA_PIN, GPIO_CFG_OUTPUT);
}

void set_data_input(void)
//Setter data-pin til å være inngang, og aktiverer den interne pull-up motstanden.
{
	GpioPinConfigSet(PORT_B, DATA_PIN, GPIO_CFG_PULLUP);
}
