#ifndef HESSDALEN_WEATHER_H_
#define HESSDALEN_WEATHER_H_

#include <sys/timer.h>
#include <sys/socket.h>
#include <sys/confnet.h>
#include <sys/thread.h>
#include <sys/heap.h>

#include <arpa/inet.h>
#include <pro/dhcp.h>
#include <dev/board.h>
#include <netinet/tcp.h>
#include <avr/wdt.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include "network.h"
#include "adc.h"
#include "bmp180.h"
#include "sht10.h"

#define ETHERNUT_1	1;
//#define ETHERNUT_2	2;

#ifdef ETHERNUT_1
#define STATION_ID	ETHERNUT_1 //Endres avhengig av ethernut.
#else
#define STATION_ID	ETHERNUT_2
#endif

#define MEASURE_ARR_MAX	11
#define FINAL_ARR_MAX	12

//Holder på en måling for en målingstype gjort hvert 30.sek.
typedef struct measure {
	char datetime[DATETIME_STRING_LENGTH];
	double value;
} measure_t;

//Holder på ferdig utregnede verdier fra de sist 5 min.
typedef struct final_value {
	char datetime[DATETIME_STRING_LENGTH];
	uint8_t station_id;
	struct range *temp;
	struct range *humi;
	struct range *pressure;
	struct range *wind;
} final_value_t;

//Holder på utregnede verdier for en målingstype.
typedef struct range {
	char measure_class[10];
	double avg;
	double now;
	double max;
	char time_max[DATETIME_STRING_LENGTH];
	double min;
	char time_min[DATETIME_STRING_LENGTH];
	double max_dir;
} range_t;

//Starter watchdog-telleren, starter om CPU hvis ikke den blir tilbakestilt med restart_watchdog.
void start_watchdog(void);

//Tilbakestiller watchdog-telleren.
void restart_watchdog(void);

//Skrur av watchdog.
void disable_watchdog(void);

//Leser data fra alle sensorer.
void read_sensors(void);

//Samler data og gjør de klar for sending.
void prepare_data(void);

//Sender data til server.
void send_data(void);

//Venter på en 30.sekunder.
void wait_30_sec(void);

//Registerer output på serieutgang, brukes ved debug.
void configure_debug(uint32_t baud);

int main(void);

#endif

