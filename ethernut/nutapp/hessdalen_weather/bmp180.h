#ifndef BMP180_H_
#define BMP180_H_

/*
 * Grensesnitt mellom Ethernut 2.1 og BMP180.
 * Brukes ved å først kalle bmp180_init, deretter bmp180_read_data.
 * Pins brukt:
 *	PORT D, pin 0 for SLA. (Klokke)
 *	PORT D, pin 1 for SLD. (Data)
*/

#include "hessdalen_weather.h"

#define BMP180_OSS		3	//Bestemmer nøyaktigheten på måling av trykk.
#define BMP180_ADDR		0xee	//Adressen til BMP180.

#define BMP180_CTRL_ADDR	0xf4	//Adressen for å kontrollere BMP180.
#define BMP180_DATA_ADDR	0xf6	//Adressen der de ferdige dataene er lagret.

#define BMP180_MEASURE_TEMP	0x2e	//Kommando for å måle temp.
#define BMP180_MEASURE_PRESSURE	0xf4	//Kommando for å måle trykk med oss=3.

//Leser rådata og returnerer ferdig utregnede verdier.
//Alle formler er ifra databladet til bmp180.
void bmp180_read_data(int32_t *pressure);

//Henter kalibreringsparametere.
//Denne må kalles før bmp180_read_data.
uint8_t bmp180_init(void);

//Initialiserer TWI.
void TWI_init(void);

//Skanner TWI-bussen etter BMP180.
uint8_t TWI_scan(void);

//Sender start-signal.
void TWI_start(void);

//Sender stopp-signal.
void TWI_stop(void);

//Skriver en byte.
void TWI_write(uint8_t data);

//Leser en byte og sender ACK.
uint8_t TWI_read_ack(void);

//Leser en byte uten å sende ACK.
uint8_t TWI_read_nack(void);

//Leser status på TWI-bussen.
uint8_t TWI_get_status(void);

//Leser kalibreringsparametere fra bmp180.
void bmp180_read_cal_params(void);

//Leser 16 bit fra gitt registeradresse.
void bmp180_read16(uint8_t reg_addr, uint16_t *data);

//Leser 8 bit fra gitt registeradresse.
void bmp180_read8(uint8_t reg_addr, uint8_t *data);

//Skriver 8 bit til eeprom.
void bmp180_write8(uint8_t reg_addr, uint8_t data);

//Leser rådata for temperatur fra bmp180.
void bmp180_read_ut(uint16_t *data);

//Leser rådata for lufttrykk fra bmp180. 
void bmp180_read_up(uint32_t *data);

#endif
