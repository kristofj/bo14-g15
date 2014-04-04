#ifndef NETWORK_H_
#define NETWORK_H_

#include <pro/dhcp.h>
#include <pro/sntp.h>
#include <net/route.h>
#include "hessdalen_weather.h"

//Definisjoner på strenglengder for JSON-strenger.
#define JSON_MAX_ROOT_LENGTH		60
#define JSON_MAX_STRING_LENGTH		150
#define JSON_MAX_WSTRING_LENGTH		170
#define JSON_MAX_LENGTH			700

//MAC addresse for denne enheten
#define MAC_ETHERNUT1 { 0x00, 0x06, 0x33, 0x21, 0x6D, 0xC2 }
#define MAC_ETHERNUT2 { 0x00, 0x06, 0x33, 0x21, 0x6D, 0xE2 }

#define FREJA_IP	"158.39.165.8"
#define FREJA_PORT	3123

typedef struct {
	const char *data;
	const char *address;
	uint16_t port;
} network_thread_args;

//Tråd for å sende data til en server med TCP
THREAD(Send_data_thread, arg);

//Setter klokken.
void set_time_ntp(void);

//Henter nåværende tid fra timeren. Bør ikke kalles før Ntp_thread har kjørt.
void get_current_time(tm *datetime);

/*
Lager JSON-strengen:


{
	"mainDatetime": "1991-08-23 22:55:45",
	"stationId": 1,


Med makslengde JSON_MAX_ROOT_LENGTH.
Returnerer strengen i *string.
*/
void get_json_string_root(const char *date_time, uint8_t station_id, char *string);

//Lager JSON-strengen:
/*

	"temp":
	{
		"avg": 1.2,
		"now": 1.2,
		"max": 1.2,
		"timeMax": "1991-08-23 22:55:45",
		"min": 1.2,
		"timeMin": "1991-08-23 22:55:45"
	},

Med makslengde JSON_MAX_STRING_LENGTH
Returnerer strengen i *string.
*/
void get_json_string(const char *value, double avg, double now, double max, const char *time_max, double min, const char *time_min, char *string);

/*Lager JSON-strengen:

	"wind":
	{
		"avg": 1.2,
		"now": 1.2,
		"max": 1.2,
		"timeMax": "1991-08-23 22:55:45",
		"maxDir" : 1.2,
		"min": 1.2,
		"timeMin": "1991-08-23 22:55:45"
	}
}

Med makslengde JSON_MAX_WSTRING_LENGTH
Returnerer strengen i *string.
*/
void get_json_wstring(double avg, double now, double max, const char *time_max, double max_dir, double min, const char *time_min, char *string);

//Samler sammen delene av JSON-strengen. Returnerer den i *string.
void get_json(char *date_time, char *json_string1, char *json_string2, char *json_string3, char *json_wstring, char *string);

//Sender av gårde gitt data.
int send_data(const char *data, const char *address, uint16_t port);

//Konfigurerer netverkskontrolleren til ethernut, setter IP med DHCP.
int configure_network(void);

#endif
