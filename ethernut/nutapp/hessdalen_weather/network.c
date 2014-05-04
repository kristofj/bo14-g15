#include "network.h"

TCPSOCKET *sock;

THREAD(Send_data_thread, arg)
{
	network_thread_args_t *args  = (network_thread_args_t *) arg;
	sock = NutTcpCreateSocket();
	uint16_t bytes = strlen(args->data) + 1;
	uint16_t sent, got;
	char buffer[32];
	
	puts("Sending data...");

	printf("Data to be sent: %s \n Bytes to be sent: %d\n", args->data, bytes);

	NutTcpSetSockOpt(sock, TCP_MAXSEG, &bytes, sizeof(bytes) + 32); //Endrer maksimal segmentstørrelse for denne socketen.
	
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

	if((got = NutTcpReceive(sock, buffer, sizeof(buffer)))) {
		puts("Error receiving data");
		printf("Got %d bytes\n", got);
	}
	
	if(strcmp(buffer, "Done") == 0) {
		puts("Got done from server.");
	}
	
	puts("Sending complete...");
	
	NutTcpCloseSocket(sock);
	
	free(args);
	NutThreadExit();
	for(;;);
}

void set_time_ntp(void)
{
	time_t ntp_time = 0;
	uint32_t timeserver = 0;
	
	_timezone = -1 * 60 * 60; //Setter tidssonen til UTC+1
	timeserver = inet_addr("85.252.162.7"); //Benytter pool.ntp.org sin norske server. http://www.pool.ntp.org/en/

	for(;;) {
		if(NutSNTPGetTime(&timeserver, &ntp_time) == 0) {
			puts("Time set");
			stime(&ntp_time); //Setter klokken til Ethernut.
			return;
		} else {
			puts("Failed retrieving time. Retrying in 10 sec...");
			NutSleep(10000);
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
	char temp[JSON_MAX_ROOT_LENGTH];
	const char json_string[] = "{\"mainDatetime\":\"%s\",\"stationId\":%d,";

	sprintf(temp, json_string, date_time, station_id);
	strcpy(string, temp);
}

void get_json_string(const char *value, double avg, double now, double max, const char *time_max, double min, const char *time_min, char *string)
{
	char temp[JSON_MAX_STRING_LENGTH];
	const char json_string[] = "\"%s\":{\"avg\":%lf,\"now\":%lf,\"max\":%lf,\"timeMax\":\"%s\",\"min\":%lf,\"timeMin\":\"%s\"},";

	sprintf(temp, json_string, value, avg, now, max, time_max, min, time_min);
	strcpy(string, temp);
}

void get_json_wstring(double avg, double now, double max, const char *time_max, double max_dir, double min, const char *time_min, char *string)
{
	char temp[JSON_MAX_WSTRING_LENGTH];
	const char json_string_wind[] = "\"wind\":{\"avg\":%lf,\"now\":%lf,\"max\":%lf,\"timeMax\":\"%s\",\"maxDir\":%lf,\"min\":%lf,\"timeMin\":\"%s\"}}";

	sprintf(temp, json_string_wind, avg, now, max, time_max, max_dir, min, time_min);
	strcpy(string, temp);
}

void get_json(char *json_root, char *json_string1, char *json_string2, char *json_string3, char *json_wstring, char *string)
{
	char temp[JSON_MAX_LENGTH];

	strcat(temp, json_root);
	strcat(temp, json_string1);
	strcat(temp, json_string2);
	strcat(temp, json_string3);
	strcat(temp, json_wstring);
	
	string = temp;
}

int send_json(char *data)
{
	sock = NutTcpCreateSocket();
	uint16_t sent, got;
	char testy[20] = "hello";
	char buffer[32];
	char json_string[JSON_MAX_LENGTH];
	uint16_t bytes;

	strcpy(json_string, data);

	bytes = strlen(json_string) + 1;

	puts("Sending data...");

	printf("Data to be sent: %s \n Bytes to be sent: %d\n", json_string, bytes);

	NutTcpSetSockOpt(sock, TCP_MAXSEG, &bytes, sizeof(bytes)); //Endrer maksimal segmentstørrelse for denne socketen.
	
	while (NutTcpConnect(sock, inet_addr(FREJA_IP), FREJA_PORT)) {
		puts("Could not connect to server, retrying...");
		NutSleep(100);
	}
	
	if ((sent = NutTcpSend(sock, json_string, bytes)) != bytes) {
		puts("Error sending data, exiting thread...");
		printf("Sent %d bytes\n", sent);
		NutTcpCloseSocket(sock);
		return 1;
	}
	
	printf("Sent %d bytes\n", sent);
	/*
	if((got = NutTcpReceive(sock, buffer, sizeof(buffer)))) {
		puts("Error receiving data");
		printf("Got %d bytes\n", got);
		NutTcpCloseSocket(sock);
		return 1;
	}
	
	if(strcmp(buffer, "Done") == 0) {
		puts("Got done from server.");
	}
	else {
		puts("Didn't get done from server.");
	}

	puts("Sending complete...");
	*/
	NutTcpCloseSocket(sock);

	return 0;
}

int configure_network(void)
{
	//uint8_t mac[6] = MAC_ETHERNUT1;
	uint8_t mac[6] = MAC_ETHERNUT2;
	
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

