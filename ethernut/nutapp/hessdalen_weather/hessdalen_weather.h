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
#include <dev/watchdog.h>

//Standard C-library headers
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>

#include "network.h"
#include "adc.h"
#include "bmp180.h"
#include "sht10.h"
#include "lili.h"

#define ETHERNUT_1	1;
#define ETHERNUT_2	2;

#define STATION_ID	ETHERNUT_1 //Endres avhengig av ethernut.

//Starter watchdog-telleren, resetter CPU hvis ikke den blir tilbakestilt med restart_watchdog.
void start_watchdog(uint32_t ms);

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

