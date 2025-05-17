#include <stdlib.h>
#include "stack.h"
#include "utils.h"

#define PS_SIZE 128

Snode *create_snode(char *token)
{
    Snode *new_node = (Snode *)malloc(sizeof(Snode));
    if (!new_node)
    {
        alloc_error();
        return NULL;
    }
    new_node->next = NULL;

    new_node->token = (char *)malloc(PS_SIZE * sizeof(char));
    if (!new_node->token)
    {
        alloc_error();
        return NULL;
    }
    my_memset(new_node->token, '\0', PS_SIZE);

    ///**** */
    my_strncpy(new_node->token, token, PS_SIZE - 1);

    return new_node;
}

Stack *create_stack()
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (!stack)
    {
        alloc_error();
        return NULL;
    }

    stack->head = NULL;

    return stack;
}

int is_s_empty(Stack *stack)
{
    return stack->head == NULL ? 1 : 0;
}

void push(Stack *stack, char *token)
{
    char *new_token = (char *)malloc(PS_SIZE * sizeof(char));
    if (!new_token)
    {
        alloc_error();
        return;
    }
    my_memset(new_token, '\0', PS_SIZE);
    my_strncpy(new_token, token, PS_SIZE);

    Snode *new_node = create_snode(new_token);
    if (!new_node)
    {
        return;
    }
    new_node->next = stack->head;
    stack->head = new_node;
}

Snode *pop(Stack *stack)
{
    if (is_s_empty(stack))
    {
        printf("Stack Overflow\n");
        return NULL;
    }

    Snode *temp = stack->head;
    stack->head = stack->head->next;

    return temp;
}

char *peek(Stack *stack)
{
    if (!is_s_empty(stack))
    {
        return stack->head->token;
    }
    else
    {
        printf("Stack is empty\n");
        return NULL;
    }
}


void free_stack(Stack *stack){
    while(is_s_empty(stack) == 0)
    {
       Snode *temp = pop(stack);
        free_snode(temp);
    }

    free(stack);
}

void free_snode(Snode *node)
{
       node->next = NULL;
       free(node->token);
       free(node);
       node = NULL;
}