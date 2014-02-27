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

//Format for sending av data:
//verdi;min;max;avg;timeMin,timeMax,dateTime:verdi;min;max;avg;timeMin,timeMax,dateTime osv.
//Tidsformat: "YYYY-MM-DD HH:MM:SS"
int send_data(const char *data, const char *address, uint16_t port)
{
	network_thread_args *arguments = malloc(sizeof(network_thread_args));
	arguments->data = data;
	arguments->address = address;
	arguments->port = port;
	
	NutThreadCreate("send_data_thread", Send_data_thread, arguments, 512);
	
	return 0;
}

int configure_network(uint8_t *mac_address)
{
	//Register ethernet controller
	if (NutRegisterDevice(&DEV_ETHER, 0, 0)) {
		puts("Registering " DEV_ETHER_NAME " failed.");
		return 1;
	}
	//Configure DHCP.
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
	//uint8_t i;
	
	configure_debug(baud);
	configure_network(mac);
	
	puts("Hessdalen weather program");
	
	send_data("Hei på deg, gamle kar!", "84.212.49.255", 6789);
	
	puts("Done, looping");
	for (;;) {
		NutSleep(1000);
	}
 
	return 0;
}
