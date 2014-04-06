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

//Starter watchdog-telleren, resetter CPU hvis ikke den bli tilbakestilt.
void start_watchdog(uint32_t ms);

//Tilbakestiller watchdog-telleren.
void restart_watchdog(void);

//Leser alle sensorer.
void read_sensors(void);

//Regner ut gjennomsnitt og finner ut max/min for de siste 5 min.
void prepare_data(void);

//Sender data til server.
void send_data(void);

//Venter på helt minutt.
void wait_for_whole_min(tm *datetime);

//Registerer output på serieutgang, brukes ved debug.
void configure_debug(uint32_t baud);

int main(void);

#endif
