#ifndef LILI_H_
#define LILI_H_

//Enkel implementasjon av lenket liste.
#include "hessdalen_weather.h"

//Node for ubehandlet verdi.
typedef struct m_node {
	char *datetime;
	double value;
} m_node_t;

//Node for ferdig utregnede verdier klar til sending.
typedef struct node {
	char *datetime;
	uint8_t station_id;
	struct values *temp;
	struct values *humi;
	struct values *pressure;
	struct values *wind;
} node_t;

//Holder verdiene for en utregner periode.
typedef struct values {
	char *value;
	double avg;
	double now;
	double max;
	char *time_max;
	double min;
	char *time_min;
	double max_dir;
} values_t;

void m_push(m_node_t *head, m_node_t *next);

void m_pop(m_node_t **head, m_node_t *popped);

uint16_t m_list_length(m_node_t *head);

void m_get_last(m_node_t *head, m_node_t *last);

//Legger til en node på slutten av listen.
void push(node_t *head, node_t *next);

//Fjerner første node i listen, returneres i popped. -1 hvis head er null.
void pop(node_t **head, node_t *popped);

//Returnerer lengden på listen.
uint16_t list_length(node_t *head);

void get_last(node_t *head, node_t *last);

#endif

