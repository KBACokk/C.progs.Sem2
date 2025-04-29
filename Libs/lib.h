struct node
{
    int data;
    struct node *next;
    struct node *second_next;
};
typedef struct node node_t;

node_t *newNode(int data);

void printList(node_t *head);

void printElement(node_t *element);

void addLast(node_t **head, int data);

void addSecond(node_t **head, int data, node_t **S);
