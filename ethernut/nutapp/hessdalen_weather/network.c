#include "network.h"

THREAD(Send_data_thread, arg)
{
	network_thread_args *args  = (network_thread_args *) arg;
	TCPSOCKET *sock = NutTcpCreateSocket();
	uint16_t bytes = strlen(args->data) + 1;
	uint16_t sent;
	
	puts("Sending data...");
	printf("Data to be sent: %d\n", bytes);
	
	NutTcpSetSockOpt(sock, TCP_MAXSEG, &bytes, sizeof(bytes)); //Endrer maksimal segmentstørrelse for denne socketen.
	
	while (NutTcpConnect(sock, inet_addr(args->address), args->port)) {
		puts("Could not connect to server, retrying in 10 seconds...");
		NutSleep(10000);
	}
	
	if ((sent = NutTcpSend(sock, args->data, bytes)) != bytes) {
		puts("Error sending data, exiting thread...");
		printf("Sent %d bytes\n", sent);
		NutTcpCloseSocket(sock);
		NutThreadExit();
	}
	
	printf("Sent %d bytes\n", sent);
	
	puts("Sending complete...");
	
	NutTcpCloseSocket(sock);
	NutThreadExit();
	for(;;);
}

THREAD(Ntp_thread, arg)
{
	time_t ntp_time = 0;
	uint32_t timeserver = 0;
	
	_timezone = -1 * 60 * 60; //Setter tidssonen til UTC+1
	timeserver = inet_addr("85.252.162.7"); //Benytter pool.ntp.org sin norske server. http://www.pool.ntp.org/en/

	for(;;) {
		if(NutSNTPGetTime(&timeserver, &ntp_time) == 0) {
			puts("Updating current time...");
			stime(&ntp_time); //Setter klokken til Ethernut.
			NutSleep(1800000); //Venter i 30 min.
		} else {
			puts("Failed retrieving time. Retrying in 10 sec...");
			NutSleep(10000);
		}
	}
}

void get_current_time(tm *datetime)
{
	time_t t;
	t = time(NULL);
	datetime = localtime(&t);
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

void get_json(char *date_time, char *json_string1, char *json_string2, char *json_string3, char *json_wstring, char *string)
{
	strncat(string, date_time, JSON_MAX_ROOT_LENGTH);
	strncat(string, json_string1, JSON_MAX_STRING_LENGTH);
	strncat(string, json_string2, JSON_MAX_STRING_LENGTH);
	strncat(string, json_string3, JSON_MAX_STRING_LENGTH);
	strncat(string, json_wstring, JSON_MAX_LENGTH);
}

int send_data(const char *data, const char *address, uint16_t port)
{
	network_thread_args *arguments = (network_thread_args *)malloc(sizeof(network_thread_args));
	arguments->data = data;
	arguments->address = address;
	arguments->port = port;
	
	NutThreadCreate("send_data_thread", Send_data_thread, arguments, 256); //Størrelse på stacken må evt. justeres fra 256 hvis tråden blir endret
	
	return 0;
}

int configure_network(void)
{
	uint8_t mac[6] = MAC_ETHERNUT1;
	//uint8_t mac[6] = MAC_ETHERNUT2;
	
	//Registrerer ethernet-kontroller
	if (NutRegisterDevice(&DEV_ETHER, 0, 0)) {
		puts("Registering " DEV_ETHER_NAME " failed.");
		return -1;
	}
	//Konfigurerer DHCP.
	if (NutDhcpIfConfig(DEV_ETHER_NAME, mac, 0)) {
		puts("Configuring " DEV_ETHER_NAME " failed.");
		return -1;
	}
	printf("I'm at %s.\n", inet_ntoa(confnet.cdn_ip_addr)); //Printer IP-adressen til standard output
	return 0;
}
