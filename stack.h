#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include "utils.h"

typedef struct Snode{
    char *token;
    struct Snode *next;
}Snode;

typedef struct Stack
{
    Snode *head;
}Stack;

Snode *create_snode(char *token);
Stack *create_stack();
int is_s_empty(Stack *stack);
void push(Stack *stack, char *new_token);
Snode *pop(Stack *stack);
char *peek(Stack *stack);
void free_stack(Stack *stack);
void free_snode(Snode *node);

#endif
