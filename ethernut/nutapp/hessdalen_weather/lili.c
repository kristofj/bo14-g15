#include "lili.h"

void m_push(m_node_t *head, m_node_t *next)
{
	m_node_t *current = head;

	while(current->next != NULL) {
		current = current->next;
	}
	current->next = next;
}

uint8_t pop(m_node_t **head, node_t *popped)
{
	m_node_t *next_node = NULL, *retval;

	if(*head == NULL)
		return -1;

	next_node = (*head)->next;
	popped = (*head);
	*head = next_node;

	return 0;
}

uint16_t m_list_length(m_node_t *head)
{
	m_node_t *current = head;
	uint16_t size = 0;

	while(current != NULL) {
		size++;
		current = current->head;
	}
	return size;
}

void push(node_t *head, node_t *next)
{
	node_t *current = head;
	
	while(current->next != NULL) {
		current = current->next;
	}

	current->next = next;
}

uint8_t pop(node_t **head, node_t *popped)
{
	node_t *next_node = NULL, *retval;

	if(*head == NULL)
		return -1;

	next_node = (*head)->next;
	popped = (*head);
	*head = next_node;
	
	return 0;
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

