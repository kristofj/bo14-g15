#ifndef HESSDALEN_WEATHER_H_
#define HESSDALEN_WEATHER_H_

#include <sys/timer.h>
#include <sys/socket.h>
#include <sys/confnet.h>
#include <sys/thread.h>

#include <arpa/inet.h>
#include <pro/dhcp.h>
#include <dev/board.h>
#include <netinet/tcp.h>

//Standard C-library headers
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include <avr/wdt.h>

#include "network.h"
#include "adc.h"
#include "bmp180.h"
#include "sht10.h"

#define ETHERNUT_1	1;
#define ETHERNUT_2	2;

#define STATION_ID	ETHERNUT_1 //Endres avhengig av ethernut.

#define MEASURE_ARR_MAX	11
#define FINAL_ARR_MAX	12

typedef struct measure {
	char *datetime;
	double value;
} measure_t;

typedef struct final_value {
	char *datetime;
	uint8_t station_id;
	struct range *temp;
	struct range *humi;
	struct range *pressure;
	struct range *wind;
} final_value_t;

typedef struct range {
	char *measure_class;
	double avg;
	double now;
	double max;
	char *time_max;
	double min;
	char *time_min;
	double max_dir;
} range_t;

//Starter watchdog-telleren, resetter CPU hvis ikke den blir tilbakestilt med restart_watchdog.
void start_watchdog(void);

//Tilbakestiller watchdog-telleren.
void restart_watchdog(void);

//Leser alle sensorer.
void read_sensors(void);

//Samler data og gjør de klar for sending.
void prepare_data(void);

//Sender data til server.
void send_data(void);

//Venter på helt minutt.
void wait_for_whole_min(void);

//Venter i 30 sekunder.
void wait_30_sec(void);

//Registerer output på serieutgang, brukes ved debug.
void configure_debug(uint32_t baud);

int main(void);

#endif

