#ifndef LILI_H_
#define LILI_H_

//Enkel implementasjon av lenket liste.

#include "hessdalen_weather.h"

typedef struct node {
	char *date_time;
	uint8_t station_id;
	char *value;
	double avg;
	double now;
	double max;
	char *time_max;
	double min;
	char *time_min;
	double max_dir;
	
	struct node *next;
} node_t;

//Legger til en node på slutten av listen.
void push(node_t *head, node_t *next);

//Fjerner første node i listen, returneres i popped. -1 hvis head er null.
uint8_t pop(node_t *head, node_t *popped);

//Returnerer lengden på listen.
uint16_t list_length(node_t *head);

#endif

