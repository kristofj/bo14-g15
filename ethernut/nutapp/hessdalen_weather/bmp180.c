#include "bmp180.h"

//Holder på kalibreringsparamterene som leses ifra bmp180.
struct bmp180_cal_params {
	int16_t AC1;
	int16_t AC2;
	int16_t AC3;
	uint16_t AC4;
	uint16_t AC5;
	uint16_t AC6;
	int16_t B1;
	int16_t B2;
	int16_t MB;
	int16_t MC;
	int16_t MD;
};

struct bmp180_cal_params params;

void bmp180_read_data(int32_t *pressure)
{
	uint16_t ut;
	uint32_t up;
	int32_t X1, X2, X3, B3, B5, B6;
	uint32_t B4, B7;

	//Leser rådata
	bmp180_read_ut(&ut);
	bmp180_read_up(&up);

	//Regner ut temperatur:
	X1 = (((int32_t) ut - (int32_t)params.AC6) * (int32_t)params.AC5) >> 15;
	X2 = ((int32_t) params.MC << 11) / (X1 + params.MD);
	B5 = X1 + X2;

	//Regner ut trykk:
	B6 = B5 - 4000;

	X1 = (B6 * B6) >> 12;
	X1 *= params.B2;
	X1 >>= 11;

	X2 = params.AC2 * B6;
	X2 >>= 11;

	X3 = X1 + X2;
	B3 = (((((int32_t) params.AC1)*4 + X3) << BMP180_OSS)+2) >> 2;

	X1 = (params.AC3 * B6) >> 13;
	X2 = (params.B1 * ((B6*B6) >> 12)) >> 16;
	X3 = ((X1 + X2) + 2) >> 2;
	B4 = (params.AC4 * (uint32_t) (X3 + 32768)) >> 15;

	B7 = ((uint32_t)(up - B3) * (50000 >> BMP180_OSS));
	
	if(B7 < 0x80000000) {
		*pressure = (B7 << 1) / B4;
	}
	else {
		*pressure = (B7 / B4) << 1;
	}

	X1 = *pressure >> 8;
	X1 *= X1;
	X1 = (X1 * 3038) >> 16;
	X2 = (*pressure * (-7357)) >> 16;
	*pressure += (X1 + X2 + 3791) >> 4; //Lufttrykk i Pa.
	*pressure /= 100; //Lufttrykk i hPa.
}

uint8_t bmp180_init(void)
{
	uint8_t chip_id;
	uint8_t bmp180_adress;

	TWI_init();
	
	bmp180_adress = TWI_scan();

	if(bmp180_adress != 0xee) {
		puts("Could not find bmp180.");
		return -1;
	}

	bmp180_read8(0xd0, &chip_id); //Chip-id skal være 0x55.

	if(chip_id == 0x55) {
		puts("Chip id confirmed");
	} else {
		printf("Wrong chip id: %x\n", chip_id);
		return -1;
	}

	bmp180_read_cal_params();

	return 0;
}

void TWI_init(void)
{
	//SCL frekvens = 14745.6/(16+2*TWBR*4^TWSR) = 97kHz. Atmega128 datablad s.203.
	TWSR = (0<<TWPS1) | (0<<TWPS0); //Setter prescalar til 1.
	TWBR = (1<<TWBR4) | (1<<TWBR0); //Setter bit rate til 17.
	TWCR = (1<<TWEN); //Aktiverer TWI. 
}

uint8_t TWI_scan(void)
{
	uint8_t i, status, addr = -1;

	for(i=0x10; i<0xf0; i++) { //Sjekker alle mulige adresser på TWI-bussen.
		TWI_start();

		TWI_write(i);
		status = TWI_get_status();

		TWI_stop();

		if(status == 0x18) { //ACK har blitt mottatt. Fant en enhet.
			addr = i;
			printf("Slave found at %x\n", i);
			break;
		}
	}
	return addr;
}

void TWI_start(void)
{
	uint8_t i = 0;

	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN); //Sender START.
	//Venter på at TWINT er satt. START har blitt sendt. Venter maks 5*10 ms før vi gir opp.
	while((TWCR & (1<<TWINT)) == 0) {
		if(i == 5) { //Timeout.
			return;
		} else {
			NutSleep(10);
			restart_watchdog();
			i++;
		}
	}
}

void TWI_stop(void)
{
	TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN); //Sender STOP.
}

void TWI_write(uint8_t data)
{
	uint8_t i = 0;

	TWDR = data; //Laster byten inn i data-registeret. Klar for sending.
	TWCR = (1<<TWINT) | (1<<TWEN); //Starter sending av data i data-registeret.
	while((TWCR & (1<<TWINT)) == 0) { //Venter på at data er sendt. Venter maks 5*10 ms.
		if(i == 5) { //Timeout.
			return;
		} else {
			NutSleep(10);
			restart_watchdog();
			i++;
		}
	}
}

uint8_t TWI_read_ack(void)
{
	uint8_t data;

	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA); //Sender ACK.
	while((TWCR & (1<<TWINT)) == 0); //Venter på at TWINT er satt. Data har blitt mottatt.
	data = TWDR; //Leser mottat data.
	
	return data;
}

uint8_t TWI_read_nack(void)
{
	uint8_t data;

	TWCR = (1<<TWINT) | (1<<TWEN); //Sender ikke ACK.
	while((TWCR & (1<<TWINT)) == 0); //Venter på at TWINT er satt. Data har blitt mottatt.
	data = TWDR; //Leser mottatt data.
	
	return data;
}

uint8_t TWI_get_status(void)
{
	uint8_t status;
	status = TWSR & 0xf8; //Trenger bare å lese øverste 5 bit.
	return status;
}

void bmp180_read_cal_params(void)
{
	uint8_t i;
	uint16_t data[11];
	uint16_t *p = data;

	for(i=0xaa; i<0xbf; i+=2, p++) { //Leser EEPROM fra 0xAA - 0xBF.
		bmp180_read16(i, p);
	}

	params.AC1 = (int16_t) data[0];
	params.AC2 = (int16_t) data[1];
	params.AC3 = (int16_t) data[2];
	params.AC4 = (uint16_t) data[3];
	params.AC5 = (uint16_t) data[4];
	params.AC6 = (uint16_t) data[5];
	params.B1 = (int16_t) data[6];
	params.B2 = (int16_t) data[7];
	params.MB = (int16_t) data[8];
	params.MC = (int16_t) data[9];
	params.MD = (int16_t) data[10];
}

void bmp180_read16(uint8_t reg_addr, uint16_t *data)
{
	uint8_t MSB, LSB;

	TWI_start();

	TWI_write(BMP180_ADDR); //Master Transmitter Mode.
	TWI_write(reg_addr);

	TWI_start(); //Restart

	TWI_write(BMP180_ADDR+1); //Master Receiver Mode.
	MSB = TWI_read_ack(); //Leser MSB.
	LSB = TWI_read_nack(); //Leser LSB. Trenger ikke ack for siste byte.

	TWI_stop();

	*data = (MSB<< 8) | (LSB); //Setter sammen MSB og LSB.
}

void bmp180_read8(uint8_t reg_addr, uint8_t *data)
{
	TWI_start(); //Start på kommunikasjon.
	
	TWI_write(BMP180_ADDR); //Master Transmitter mode.
	TWI_write(reg_addr);

	TWI_start(); //Restart.

	TWI_write(BMP180_ADDR + 1); //Master Reciever mode.
	*data = TWI_read_nack(); //Trenger ikke ack for siste byte.
	
	TWI_stop();
}

void bmp180_write8(uint8_t reg_addr, uint8_t data)
{
	TWI_start();

	TWI_write(BMP180_ADDR); //Master Transmitter Mode.
	TWI_write(reg_addr);
	TWI_write(data);

	TWI_stop();
}

void bmp180_read_ut(uint16_t *data)
{
	uint16_t a;

	//Ber BMP180 starte måling av temperatur.
	bmp180_write8(BMP180_CTRL_ADDR, BMP180_MEASURE_TEMP);

	NutMicroDelay(5); //Venter på at bmp180 er ferdig med å måle temp. Maks 4.5ms

	bmp180_read16(BMP180_DATA_ADDR, &a); //Leser målt verdi.

	*data = a;
}

void bmp180_read_up(uint32_t *data)
{
	uint16_t MSB_LSB; //Deklarerer variabler.
	uint8_t XLSB;

	//Ber BMP180 starte måling av lufttrykk.
	bmp180_write8(BMP180_CTRL_ADDR, BMP180_MEASURE_PRESSURE);

	//Venter på at bmp180 er ferdig. Maks 25.5 ms ved oss=3.
	NutMicroDelay(26);

	//Leser MSB + LSB fra 0xF6-0xF7.
	bmp180_read16(BMP180_DATA_ADDR, &MSB_LSB);

	//Leser XLSB fra 0xF8.
	bmp180_read8(BMP180_DATA_ADDR + 2, &XLSB);

	*data = ((a << 8) | (b)) >> (8 - BMP180_OSS); //Returnerer rådata. Formel fra datablad.
}
