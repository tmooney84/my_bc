#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include "utils.h"

typedef struct Qnode{
    char *token;
    struct Qnode *next;
}Qnode;

typedef struct Queue{
    Qnode *front;
    Qnode *rear;
} Queue;

Qnode *create_qnode(char *token);
Queue *create_queue();
int is_q_empty(Queue *q);
int enqueue(Queue *q, char *new_token);
Qnode *dequeue(Queue *q);
void printQueue(Queue *q);

#endif
