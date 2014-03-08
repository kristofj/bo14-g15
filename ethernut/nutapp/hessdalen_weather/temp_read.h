#ifndef TEMP_READ_H
#define TEMP_READ_H

#include "hessdalen_weather.h"
#include <dev/gpio.h> 

uint32_t sht10_write_byte(uint8_t value);
int8_t sht10_read_byte(uint8_t ack);
void sht10_connectionreset(void);
void sht10_transstart(void);
uint32_t read_data(void);
void set_data_output(void);
void set_data_input(void);
void initiate_ports(void);

#endif
