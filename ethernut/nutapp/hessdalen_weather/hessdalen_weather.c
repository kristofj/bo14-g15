#include <sys/timer.h>
#include <sys/socket.h>
#include <sys/confnet.h>
#include <sys/thread.h>

#include <arpa/inet.h>
#include <pro/dhcp.h>
#include <dev/board.h>

//Standard C-library headers
#include <stdint.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Definisjoner på strenglengder for JSON-strenger.
#define JSON_MAX_DATETIME_LENGTH	39
#define JSON_MAX_STRING_LENGTH		146
#define JSON_MAX_WSTRING_LENGTH		163
#define JSON_MAX_LENGTH			631

//MAC address for this device
#define MAC { 0x00, 0x06, 0x33, 0x21, 0x6D, 0xC2 }

typedef struct {
	const char *data;
	const char *address;
	uint16_t port;
} network_thread_args;

THREAD(Send_data_thread, arg)
{
	puts("Sending data...");
	
	network_thread_args *args  = (network_thread_args *) arg;
	TCPSOCKET *sock = NutTcpCreateSocket();
	
	if(NutTcpConnect(sock, inet_addr(args->address), args->port)) {
		//Could not connect to server.
		NutTcpCloseSocket(sock);
		NutThreadExit();
	}
	
	if(NutTcpSend(sock, args->data, sizeof(args->data)) != sizeof(args->data)) {
		//Error sending data.
		NutTcpCloseSocket(sock);
		NutThreadExit();
	}
	
	NutTcpCloseSocket(sock);
	NutThreadExit();
	for(;;);
}

char *get_json_string_datetime(const char *date_time)
{
	char *string = (char *)malloc(JSON_MAX_DATETIME_LENGTH);
	const char json_string[] = "{\"mainDatetime\":\"%s\",";

	sprintf(string, json_string, date_time);

	return string;
}

char *get_json_string(const char *value, double avg, double now, double max, const char *time_max, double min, const char *time_min)
{
	char *string = (char *)malloc(JSON_MAX_STRING_LENGTH);
	const char json_string[] = "\"%s\":{\"avg\":%lf,\"now\":%lf,\"max\":%lf,\"timeMax\":\"%s\",\"min\":%lf,\"timeMin\":\"%s\"},";

	sprintf(string, json_string, value, avg, now, max, time_max, min, time_min);
	
	return string;
}

char *get_json_wstring(double avg, double now, double max, const char *time_max, double max_dir, double min, const char *time_min)
{
	char *string = (char *)malloc(JSON_MAX_WSTRING_LENGTH);
	const char json_string_wind[] = "\"wind\":{\"avg\":%lf,\"now\":%lf,\"max\":%lf,\"timeMax\":\"%s\",\"maxDir\":%lf,\"min\":%lf,\"timeMin\":\"%s\"}}";

	sprintf(string, json_string_wind, avg, now, max, time_max, max_dir, min, time_min);

	return string;
}

char *get_json(char *date_time, char *json_string1, char *json_string2, char *json_string3, char *json_wstring)
{
	char *json_data = (char *)malloc(JSON_MAX_LENGTH);

	strncat(json_data, date_time, JSON_MAX_DATETIME_LENGTH);
	strncat(json_data, json_string1, JSON_MAX_STRING_LENGTH);
	strncat(json_data, json_string2, JSON_MAX_STRING_LENGTH);
	strncat(json_data, json_string3, JSON_MAX_STRING_LENGTH);
	strncat(json_data, json_wstring, JSON_MAX_LENGTH);

	return json_data;
}

//Format for sending av data:
//verdi;min;max;avg;timeMin,timeMax,dateTime:verdi;min;max;avg;timeMin,timeMax,dateTime osv.
//Tidsformat: "YYYY-MM-DD HH:MM:SS"
int send_data(const char *data, const char *address, uint16_t port)
{
	network_thread_args *arguments = malloc(sizeof(network_thread_args));
	arguments->data = data;
	arguments->address = address;
	arguments->port = port;
	
	NutThreadCreate("send_data_thread", Send_data_thread, arguments, 512); //Størrelse på stacken må evt. justeres fra 512
	
	return 0;
}

int configure_network(uint8_t *mac_address)
{
	//Registrerer ethernet-kontroller
	if (NutRegisterDevice(&DEV_ETHER, 0, 0)) {
		puts("Registering " DEV_ETHER_NAME " failed.");
		return 1;
	}
	//Konfigurerer DHCP.
	if (NutDhcpIfConfig(DEV_ETHER_NAME, mac_address, 0)) {
		puts("Configuring " DEV_ETHER_NAME " failed.");
		return 2;
	}
	printf("I'm at %s.\n", inet_ntoa(confnet.cdn_ip_addr));
	return 0;
}

void configure_debug(u_long baud)
{
	NutRegisterDevice(&DEV_DEBUG, 0, 0);
	freopen(DEV_DEBUG_NAME, "w", stdout);
	_ioctl(_fileno(stdout), UART_SETSPEED, &baud);
}

int main(void)
{
	u_long baud = 115200;
	uint8_t mac[6] = MAC;
	
	//configure_debug(baud);
	configure_network(mac);
	
	puts("Project Hessdalen weather station");
	
	for (;;) {
		NutSleep(1000);
	}
 
	return 0;
}