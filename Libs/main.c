#include "lib.h"
#include <stdio.h>
#include <stdlib.h>


int main()
{
    node_t *head = NULL;
    int n;

    printf("Введите числа для первого списка (0 для завершения):\n");
    while (1)
    {
        scanf("%d", &n);
        if (n == 0)
            break;
        addLast(&head, n);
    }

    node_t *S = NULL;
    printf("\nПервый список:\n");
    printList(head);

    printf("\nВведите числа для второго списка (0 для завершения):\n");
    while (1)
    {
        scanf("%d", &n);
        if (n == 0)
            break;
        addSecond(&head, n, &S);
    }

    printf("\nВторой список:\n");
    printList(S);

    node_t *current = head;
    char con;

    printf("\nУправление: \nW - Печать текущего элемента списка\nD - Печать следующего элемента списка\nS - Печать нижнего элемента\nA - Печать начального элемента текущего списка\n");

    while (1)
    {
        scanf(" %c", &con);
        switch (con)
        {
            case 'W':
                printElement(current);
                break;
            case 'D':
                if (current && current->next)
                    current = current->next;
                printElement(current);
                break;
            case 'S':
                if (current && current->second_next)
                    current = current->second_next;
                printElement(current);
                break;
            case 'A':
                current = head;
                printElement(current);
                break;
            default:
                printf("Неизвестная команда!\n");
                break;
        }
    }
    return 0;
}
