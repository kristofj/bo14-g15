#ifndef LILI_H_
#define LILI_H_

//Enkel implementasjon av lenket liste.

#include "hessdalen_weather.h"

//Node for ubehandlet verdi.
typedef struct m_node {
	char *datetime;
	double v1;
	double v2;

	struct m_node *next;
} measure_node_t

//Node for ferdig utregnede verdier klar til sending.
typedef struct node {
	char *datetime;
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

void m_push(m_node_t *head, m_node_t *next);

uint8_t m_pop(m_node_t *head, m_node_t *popped);

uint16_t m_list_length(m_node_t *head);

//Legger til en node på slutten av listen.
void push(node_t *head, node_t *next);

//Fjerner første node i listen, returneres i popped. -1 hvis head er null.
uint8_t pop(node_t *head, node_t *popped);

//Returnerer lengden på listen.
uint16_t list_length(node_t *head);

#endif

