#include "utils.h"
#include "queue.h"

Qnode *create_qnode(char *token)
{
    Qnode *node = (Qnode *)malloc(sizeof(Qnode));
    if(!node)
    {
        alloc_error();
        return NULL;
    }
    
    node->next = NULL;
    node->token = NULL;

    my_strncpy(node->token, token, sizeof(token));

    return node;
}

Queue *create_queue()
{
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if(!q)
    {
        alloc_error();
        return NULL;
    }

    return q;
}

int is_q_empty(Queue *q)
{
    return q->front == NULL? 1 : 0;
}

int enqueue(Queue *q, char *new_token)
{
    Qnode *new_node = create_qnode(new_token);
    if(!new_node)
    {
        alloc_error();
        return -1;
    }

    if(is_q_empty(q))
    {
        q->front = q->rear = new_node;
        //print_queue(q);
        return 1;
    }

    q->rear->next = new_node;
    q->rear = new_node;
    //print_queue(q);

    return 1;
}

Qnode *dequeue(Queue *q)
{
    if(is_q_empty(q))
    {
        return NULL;
    }

    Qnode *temp = q->front;
    q->front = q->front->next;
    
    if(q->front == NULL)
    {
        q->rear = NULL;
    }

    return temp;
}

void printQueue(Queue *q)
{
    if(is_q_empty(q))
    {
        printf("Queue is empty");
        return;
    }
    Qnode *temp = q->front;
    printf("Current Queue: ");
    
    while(temp != NULL)
    {
        printf("%s", temp->token);
        temp = temp->next;
    }

    printf("\n");
    return;
}
