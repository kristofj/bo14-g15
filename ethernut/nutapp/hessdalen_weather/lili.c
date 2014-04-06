#include "lili.h"

void m_push(m_node_t *head, m_node_t *next)
{
	m_node_t *current = head;

	while(current->next != NULL) {
		current = current->next;
	}
	current->next = next;
}

void m_pop(m_node_t **head, m_node_t *popped)
{
	m_node_t *next_node = NULL;

	if(*head == NULL) {
		return;
	}

	next_node = (*head)->next;
	popped = (*head);
	*head = next_node;
}

uint16_t m_list_length(m_node_t *head)
{
	m_node_t *current = head;
	uint16_t size = 0;

	while(current != NULL) {
		size++;
		current = current->next;
	}
	return size;
}

void m_get_last(m_node_t *head, m_node_t *last)
{
	if(head->next == NULL) {
		*last = *head;
		return;
	}

	m_node_t *current = head;

	while(current->next != NULL)
		current = current->next;

	*last = *current;
}

void push(node_t *head, node_t *next)
{
	node_t *current = head;
	
	while(current->next != NULL) {
		current = current->next;
	}

	current->next = next;
}

void pop(node_t **head, node_t *popped)
{
	node_t *next_node = NULL;

	if(*head == NULL)
		return;

	next_node = (*head)->next;
	popped = (*head);
	*head = next_node;
}

uint16_t list_length(node_t *head)
{
	node_t *current = head;
	uint16_t size = 0;
	
	while(current != NULL) {
		size++;
		current = current->next;
	}
	return size;
}

void get_last(node_t *head, node_t *last)
{
	if(head->next == NULL) {
		*last = *head;
		return;
	}

	node_t *current = head;

	while(current->next != NULL)
		current = current->next;

	*last = *current;
}

