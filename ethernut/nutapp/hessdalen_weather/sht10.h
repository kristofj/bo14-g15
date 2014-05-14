#ifndef SHT10_H_
#define SHT10_H_

/*
 * Grensesnitt mellom ethernut og SHT10.
 * Innholder funksjoner som tar seg av kommunikasjon med SHT10.
 * Råverdiene konverteres til grader celcius og relativt fuktighet.
 * Pins brukt:
 *	PORTB, pin 0 for SCK.
 *	PORTB, pin 1 for DATA.
*/

#include "hessdalen_weather.h"
#include <dev/gpio.h>
#include <math.h>

//Kommandoer for å styre sht10	  adr	cmd	r/w
#define MODE_TEMP	0x03	//000	0001	1
#define MODE_HUMI	0x05	//000	0010	1
#define REG_RESET	0x1e	//000	1111	0

//Forsinkelser brukt for å vente på SHT10.
#define PULSE_LONG	NutMicroDelay(9)
#define PULSE_SHORT	NutMicroDelay(3)

//Definisjoner på brukt port og pins.
#define PORT_B		NUTGPIO_PORTB
#define SCK_PIN		0
#define DATA_PIN	1

//Styrer SCK og DATA.
#define SCK_LOW		GpioPinSetLow(PORT_B, SCK_PIN)
#define SCK_HIGH	GpioPinSetHigh(PORT_B, SCK_PIN)
#define DATA_LOW	GpioPinSetLow(PORT_B, DATA_PIN)
#define DATA_HIGH	GpioPinSetHigh(PORT_B, DATA_PIN)

//Setter sck-pin og data-pin til output.
void initiate_pins(void);

//Leser all nødvendig data fra sht10 og konverterer de til normale verdier.
uint8_t sht10_measure(double *temp, double *humi);

//Leser rådata ifra sht10.
uint8_t read_sensor_raw(uint16_t *p_value, uint8_t mode);

//Sender en byte med data til sht10. Og leser ackownledge fra sensoren.
uint8_t read_byte(uint8_t ack);

//Leser en byte og sender ack til sht10 hvis ack er satt.
uint8_t write_byte(uint8_t data);

//Starten på en overføring.
void start_transmission(void);

//Starter om koblingen mellom ethernut og sensoren, kan kalles hvis man har mistet kontakt med sensoren.
void reset_connection(void);

//Kjører en soft-reset av sensoren. Setter registeret til sht10 til standard verdier.
uint8_t soft_reset(void);

//Konverterer rådata fra sensoren til relativ fuktighet og temperatur i celsius.
void extract_values(double *p_humidity, double *p_temperature);

//Henter ut status på data-pin.
uint16_t read_data_pin(void);

//Setter data-pin til å være utgang.
void set_data_output(void);

//Setter data-pin til å være inngang, og aktiverer den interne pull-up motstanden.
void set_data_input(void);

#endif

