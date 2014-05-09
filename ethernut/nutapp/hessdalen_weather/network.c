#include "network.h"

TCPSOCKET *sock;

THREAD(DataThread, arg)
{
	network_thread_args_t *args  = (network_thread_args_t *) arg;

	sock = NutTcpCreateSocket();
	uint16_t bytes = strlen(args->data) + 1;
	uint16_t sent, sock_opt = 1460, timeout = 10000;
	uint8_t retries = 0;

	puts("Sending data...");

	printf("Data to be sent: %s \n Bytes to be sent: %d\n", args->data, bytes);

	NutTcpSetSockOpt(sock, TCP_MAXSEG, &sock_opt, sizeof(sock_opt)); //Endrer maksimal segmentstørrelse for denne socketen.
	NutTcpSetSockOpt(sock, SO_SNDBUF, &sock_opt, sizeof(sock_opt));
	NutTcpSetSockOpt(sock, SO_SNDTIMEO, &timeout, sizeof(timeout));

	while (NutTcpConnect(sock, inet_addr(FREJA_IP), FREJA_PORT)) {
		printf("Could not connect to server, retry %d\n", retries);

		if(retries == 5) {
			NutThreadSetPriority(0);
			for(;;); //Restarter ethernut.
		} else {
			retries++;
		}
	}

	retries = 0;
	
	while ((sent = NutTcpSend(sock, args->data, bytes)) != bytes) {
		printf("Sent %d bytes\n", sent);
		
		bytes -= sent;
		memcpy(args->data, &args->data[sent], bytes);
		printf("Remaining: %d bytes \n", bytes);
	}
	
	puts("Sending complete...");
	
	free(args->data);
	free(args);
	
	NutTcpCloseSocket(sock);

	NutThreadSetPriority(0);
	for(;;); //Restarter ethernut.
}

void set_time_ntp(void)
{
	time_t ntp_time = 0;
	uint32_t timeserver = 0;
	uint8_t i = 0;

	_timezone = -1 * 60 * 60; //Setter tidssonen til UTC+1
	timeserver = inet_addr("85.252.162.7"); //Benytter pool.ntp.org sin norske server. http://www.pool.ntp.org/en/

	for(;;) {
		if(NutSNTPGetTime(&timeserver, &ntp_time) == 0) {
			puts("Time set");
			stime(&ntp_time); //Setter klokken til Ethernut.
			return;
		} else {
			i++;
			printf("Failed retrieving time, retry %d\n", i);
			NutSleep(10000);
			
			if(i == 5) { //Restarter ethernut.
				start_watchdog();
				for(;;);
			}
		}
	}
}

tm *get_current_time(void)
{
	time_t t;
	t = time(NULL);
	return localtime(&t);
}

void get_json_string_root(const char *date_time, uint8_t station_id, char *string)
{
	const char json_string[] = "{\"mainDatetime\":\"%s\",\"stationId\":%d,";

	sprintf(string, json_string, date_time, station_id);
}

void get_json_string(const char *value, double avg, double now, double max, const char *time_max, double min, const char *time_min, char *string)
{
	const char json_string[] = "\"%s\":{\"avg\":%lf,\"now\":%lf,\"max\":%lf,\"timeMax\":\"%s\",\"min\":%lf,\"timeMin\":\"%s\"},";

	sprintf(string, json_string, value, avg, now, max, time_max, min, time_min);
}

void get_json_wstring(double avg, double now, double max, const char *time_max, double max_dir, double min, const char *time_min, char *string)
{
	const char json_string_wind[] = "\"wind\":{\"avg\":%lf,\"now\":%lf,\"max\":%lf,\"timeMax\":\"%s\",\"maxDir\":%lf,\"min\":%lf,\"timeMin\":\"%s\"}}";

	sprintf(string, json_string_wind, avg, now, max, time_max, max_dir, min, time_min);
}

void get_json(char *json_root, char *json_string1, char *json_string2, char *json_string3, char *json_wstring, char *string)
{
	strcat(string, json_root);
	strcat(string, json_string1);
	strcat(string, json_string2);
	strcat(string, json_string3);
	strcat(string, json_wstring);
}

void get_json_array_root(char *json_root, char *string) 
{
	const char json_array_root[] = "[%s";

	sprintf(string, json_array_root, json_root);
}

void get_json_array_ele(char *json_ele, char *string)
{
	const char json_array_ele[] = ",%s";

	sprintf(string, json_array_ele, json_ele);
}

void get_json_array_end(char *json_end, char *string)
{
	const char json_array_end[] = ",%s]";

	sprintf(string, json_array_end, json_end);
}

void send_json(char *data)
{
	network_thread_args_t *args = malloc(sizeof(network_thread_args_t));

	args->data = strdup(data);

	NutThreadCreate("DataThread", DataThread, args, 512);
}

int configure_network(void)
{
#ifdef ETHERNUT_1
	uint8_t mac[6] = MAC_ETHERNUT1;
#else
	uint8_t mac[6] = MAC_ETHERNUT2;
#endif
	
	//Registrerer ethernet-kontroller
	if (NutRegisterDevice(&DEV_ETHER, 0, 0)) {
		puts("Registering " DEV_ETHER_NAME " failed.");
		return 1;
	}
	
	//Konfigurerer DHCP.
	if (NutDhcpIfConfig(DEV_ETHER_NAME, mac, 0)) {
		puts("Configuring " DEV_ETHER_NAME " failed.");
		return 1;
	}

	printf("I'm at %s.\n", inet_ntoa(confnet.cdn_ip_addr)); //Printer IP-adressen til standard output
	return 0;
}

