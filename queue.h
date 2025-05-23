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
int add_qnode(Queue *q, Qnode * node, int position);
int delete_qnode(Queue *q, Qnode * node, int position);
Qnode *dequeue(Queue *q);
void print_queue(Queue *q);
void free_queue(Queue *q);
void free_qnode(Qnode *node);

#endif
