#include "lib.h"
#include <stdio.h>
#include <stdlib.h>

node_t *newNode(int data)
{
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->data = data;
    node->next = NULL;
    node->second_next = NULL;
    return node;
}

void printList(node_t *head)
{
    node_t *cur = head;
    while (cur != NULL)
    {
        printf("%d ", cur->data);
        cur = cur->next;
    }
    printf("\n");
}

void printElement(node_t *element)
{
    if (element)
        printf("%d\n", element->data);
    else
        printf("Элемент не найден!\n");
}

void addLast(node_t **head, int data)
{
    node_t *new = newNode(data);
    if (*head == NULL)
    {
        *head = new;
        return;
    }

    node_t *cur = *head;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    cur->next = new;
}

void addSecond(node_t **head, int data, node_t **S)
{
    node_t *new = newNode(data);
    if (*S == NULL)
    {
        *S = new;
        if (*head != NULL)
        {
            (*head)->second_next = *S;
        }
        return;
    }

    node_t *cur_second = *S;
    while (cur_second->next != NULL)
    {
        cur_second = cur_second->next;
    }
    cur_second->next = new;

    node_t *cur_first = *head;
    while (cur_first !=NULL && cur_first->second_next)
    {
        cur_first = cur_first->next;
    }
    cur_first->second_next = new;
}
