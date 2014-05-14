#ifndef NETWORK_H_
#define NETWORK_H_

#include <pro/dhcp.h>
#include <pro/sntp.h>
#include <net/route.h>
#include <sys/thread.h>
#include "hessdalen_weather.h"

//Definisjoner på strenglengder for JSON-strenger.
#define JSON_ROOT_LENGTH		100
#define JSON_STRING_LENGTH		200
#define JSON_OBJECT_LENGTH		1024

#define JSON_ARRAY_LENGTH		13000

#define DATETIME_STRING_LENGTH		20

//MAC addresse for begge mikrokontrollere
#define MAC_ETHERNUT1 { 0x00, 0x06, 0x33, 0x21, 0x6D, 0xC2 }
#define MAC_ETHERNUT2 { 0x00, 0x06, 0x33, 0x21, 0x6D, 0xE2 }

#define FREJA_IP	"158.39.165.8" //IP for server.
#define FREJA_PORT	3123 //Port som vi skal sende på.

//Holder på data som skal sendes til server.
typedef struct network_thread_args{
	char *data;
} network_thread_args_t;

//Setter klokken ved å kontakte en NTP-server.
void set_time_ntp(void);

//Henter nåværende tid fra timeren.
tm *get_current_time(void);

/*
Lager JSON-strengen:


{
	"mainDatetime": "1991-08-23 22:55:45",
	"stationId": 1,

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

Returnerer strengen i *string.
*/
void get_json_wstring(double avg, double now, double max, const char *time_max, double max_dir, double min, const char *time_min, char *string);

//Samler sammen delene av JSON-strengen. Returnerer den i *string.
void get_json(char *json_root, char *json_string1, char *json_string2, char *json_string3, char *json_wstring, char *string);

//Lager første del av et JSON-array.
void get_json_array_root(char *json_root, char *string);

//Lager et element i et JSON-array.
void get_json_array_ele(char * json_ele, char *string);

//Lager siste del av et JSON-array.
void get_json_array_end(char *json_end, char *string);

//Sender av gårde gitt data.
void send_json(char *data);

//Konfigurerer netverkskontrolleren til ethernut, setter IP med DHCP.
uint8_t configure_network(void);

#endif
