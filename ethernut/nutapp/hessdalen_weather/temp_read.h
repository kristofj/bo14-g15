#ifndef TEMP_READ_H
#define TEMP_READ_H

#include "hessdalen_weather.h"
#include <dev/gpio.h>
#include <math.h>

typedef struct {
	double temp;
	double humi;
	double dew;
} sht10_data;

uint32_t sht10_write_byte(uint8_t value);
uint8_t sht10_read_byte(uint8_t ack);
void sht10_connectionreset(void);
void sht10_transstart(void);
uint32_t sht10_softreset(void);
uint32_t sht10_read_statusreg(uint8_t *p_value, uint8_t *p_checksum);
uint32_t sht10_write_statusreg(uint8_t *p_value);
uint32_t sht10_measure(uint8_t *p_value, uint8_t *p_checksum, uint8_t mode);
void calc_sht10(double *p_humidity, double *p_temperature);
double calc_dewpoint(double h, double t);
uint32_t sht10_get_data(sht10_data *data);
uint32_t read_data(void);
void set_data_output(void);
void set_data_input(void);
void initiate_ports(void);

#endif
