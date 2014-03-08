#include <dev/gpio.h>

#define NOP() asm volatile("nop")
#define SCK_PORT	NUTGPIO_PORTB
#define SCK_PIN		0
#define DATA_PORT	NUTGPIO_PORTB
#define DATA_PIN	1
#define SCK_LOW()	GpioPinSetLow(OUT_PORT, OUT_PIN)
#define SCK_HIGH()	GpioPinSetHigh(OUT_PORT, OUT_PIN)
#define DATA_LOW()	GpioPinSetLow(IN_PORT, IN_PIN)
#define DATA_HIGH()	GpioPinSetHigh(IN_PORT, IN_PIN)
#define DATA_READ()	GpioPinGet(DATA_PORT, DATA_PIN);

                            //adr  command  r/w
#define STATUS_REG_W 0x06   //000   0011    0
#define STATUS_REG_R 0x07   //000   0011    1
#define MEASURE_TEMP 0x03   //000   0001    1
#define MEASURE_HUMI 0x05   //000   0010    1
#define RESET        0x1e   //000   1111    0

uint32_t sht10_write_byte(uint8_t value) {
	set_data_output();
	
	uint8_t i;
	uint32_t error = 0;
	
	for(i = 0x80; i > 0; i /= 2) {
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
	
	set_data_input();
	NOP();
	SCK_HIGH();
	error = DATA_READ();
	SCK_LOW();
	return error;
}

int8_t sht10_read_byte(uint8_t ack)
{
	uint8_t i, val = 0;
	set_data_input();
	
	for(i = 0x80; i > 0; i /= 2) {
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
	NOP();NOP();NOP();
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
	for(i=0;i<9;i++) { 		//9 SCK cycles
		SCK_LOW();                  
		SCK_HIGH();
	}
	sht10_transstart();		//transmission start
}

//----------------------------------------------------------------------------------
void sht10_transstart(void)
//----------------------------------------------------------------------------------
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
	NOP();NOP();NOP();
	SCK_HIGH();
	NOP();
	DATA_HIGH();
	NOP();
	SCK_LOW();
}

u_int32_t read_data(void)
{
	return GpioPinGet(DATA_PORT, DATA_PIN);
}

void set_data_output(void)
{
	GpioPinConfigSet(DATA_PORT, DATA_PIN, GPIO_CFG_OUTPUT);
}

void set_data_input(void)
{
	GpioPinConfigSet(DATA_PORT, DATA_PIN, GPIO_CFG_PULLUP); //Med pullup settes DATA_PIN default til 1.
}

void initiate_ports(void)
{
	GpioPinConfigSet(SCK_PORT, SCK_PIN, GPIO_CFG_OUTPUT);	//Konfigurerer PORTB pin 0 for output.
	GpioPinConfigSet(DATA_PORT, DATA_PIN, GPIO_CFG_OUTPUT); //Konfigurerer PORTB pin 1 for output.
}
