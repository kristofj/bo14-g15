#ifndef NETWORK_H
#define NETWORK_H

#include <pro/dhcp.h>
#include <pro/sntp.h>
#include <net/route.h>

typedef struct {
	const char *data;
	const char *address;
	uint16_t port;
} network_thread_args;

THREAD(Send_data_thread, arg);
THREAD(Ntp_thread, arg);
tm *get_current_time(void);
char *get_json_string_root(const char *date_time, uint8_t station_id);
char *get_json_string(const char *value, double avg, double now, double max, const char *time_max, double min, const char *time_min);
char *get_json_wstring(double avg, double now, double max, const char *time_max, double max_dir, double min, const char *time_min);
char *get_json(char *date_time, char *json_string1, char *json_string2, char *json_string3, char *json_wstring);
int send_data(const char *data, const char *address, uint16_t port);
int configure_network(void);

#endif
