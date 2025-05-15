#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "utils.h"
#include "stack.h"
#include "queue.h"

#define PS_SIZE 128
#define NUM_ASCII_CHAR 128

int count_tokens(char *string)
{
    int token_count = 0;
    int i = 0;

    while (string[i] != '\0')
    {
        if (string[i] == '+' || string[i] == '-' || string[i] == '%' || string[i] == '*' || string[i] == '/' || string[i] == '(' || string[i] == ')')
        {
            token_count++;
            i++;
        }

        else if (string[i] >= '0' && string[i] <= '9')
        {
            i++;

            while (string[i] >= '0' && string[i] <= '9' && i < PS_SIZE - 2)
            {
                i++;
            }

            token_count++;
        }
        else
        {
            i++;
        }
    }

    return token_count;
}

int *create_priority_array()
{
    int *priority = (int *)malloc(NUM_ASCII_CHAR * sizeof(int));
    if (!priority)
    {
        alloc_error();
        return NULL;
    }

    my_memset(priority, 0, NUM_ASCII_CHAR);

    // priority['('] = 3;
    // priority[')'] = 3;
    priority['/'] = 2;
    priority['*'] = 2;
    priority['%'] = 2;
    priority['+'] = 1;
    priority['-'] = 1;

    return priority;
}

void parse_string(char *string, char **parsed_tokens, int num_tokens)
{
    int pa_idx = 0;
    int s_idx = 0;
    int i = 0;

    while (string[i] != '\0' && s_idx < PS_SIZE - 2 && pa_idx < num_tokens)
    {
        // maybe break up into parentheses and main operators
        if (string[i] == '+' || string[i] == '-' || string[i] == '%' || string[i] == '*' || string[i] == '/' || string[i] == '(' || string[i] == ')')
        {
            parsed_tokens[pa_idx][0] = string[i];
            parsed_tokens[pa_idx][1] = '\0';
            pa_idx++;
            i++;
            s_idx = 0;
        }
        else if (string[i] >= '0' && string[i] <= '9')
        {
            parsed_tokens[pa_idx][s_idx] = string[i];
            s_idx++;
            i++;

            while (string[i] >= '0' && string[i] <= '9')
            {
                parsed_tokens[pa_idx][s_idx] = string[i];
                s_idx++;
                i++;
            }

            parsed_tokens[pa_idx][s_idx] = '\0';
            pa_idx++;
            s_idx = 0;
        }
        else
        {
            i++;
        }
    }

    return;
}

Queue *process_to_rpn(char **tokens, int num_tokens)
{
    if (!tokens)
    {
        printf("Error with tokens list");
        return NULL;
    }

    Stack *operators_stack = create_stack();
    if (!operators_stack)
    {
        alloc_error();
        return NULL;
    }
    printf("stack status: %d\n", is_s_empty(operators_stack));

    Queue *rpn_queue = create_queue();
    if (!rpn_queue)
    {
        // free stack
        alloc_error();
        return NULL;
    }
    printf("queue status: %d\n", is_q_empty(rpn_queue));

    int *priority = create_priority_array();
    if (!priority)
    {
        // free stack and queue
        alloc_error();
        return -1;
    }

    // test using tokens[i][0] is a number
    // if a number enqueue it

    for (int i = 0; i < num_tokens; i++)
    {
        char c = tokens[i][0];

        if (c >= '0' && c <= '9')
        {
            enqueue(rpn_queue, tokens[i]);
            print_queue(rpn_queue);
        }

        //***need to account for 6+-5 not sure if here or in evaluate_rpn ******************************//
        else if (c == '+' || c == '-' || c == '%' || c == '*' || c == '/' || c == '(' || c == ')')
        {
            // int par_counter = 0; // keeps track of parentheses
            // the counter has to be positive or zero during stack
            // counter != -1
            // after everything is done counter needs to be zero
            // if(c == ')' && tokens[i + 1][0] == '(' && i < num_tokens - 1)
            // if (par_counter < 0)
            // {
            //     printf("parsing error");
            //     // free queue, free stack, free priority array
            // }

            printf("top of stack: %s\n", peek(operators_stack));

            // if stack empty
            if (is_s_empty(operators_stack))
            {
                if (c != ')')
                {
                    push(operators_stack, tokens[i]);
                }
                else
                {
                    parse_error();
                    //*** need to do frees */
                    return NULL;
                }

                // if (c == '(')
                // {
                //    // par_counter++;
                // }

                printf("top of stack: %s\n", peek(operators_stack));
            }

            // operators are on the stack
            else
            {
                char *top = peek(operators_stack);
                char top_op = top[0];

                // cannot have "()" with no contents inside or ")("
                if ((c == ')' && top_op == '(') || (c == '(' && top_op == ')'))
                {
                    parse_error();
                    return NULL;
                }
                else if (c == '(')
                {
                    push(operators_stack, tokens[i]);
                    //  par_counter++;
                }

                // pop stack to queue until '(' reached
                // may not need to use par_counter could just have
                else if (c == ')') //&& par_counter > 0
                {
                    Snode *top = pop(operators_stack);
                    char *popped_top = top->token;

                    // run until '(' found, if not found, parse error
                    while (popped_top[0] != '(')
                    {
                        enqueue(rpn_queue, popped_top);
                        if (is_s_empty(operators_stack)) // || popped_top[0] == ')'
                        {
                            parse_error();
                            // free stack, free queue
                            return NULL;
                        }
                        //*************create free_snode fn *************************/
                        // free popped node
                        top->next = NULL;
                        free(top->token);
                        free(top);
                        top = NULL;

                        top = pop(operators_stack);
                        popped_top = top->token;
                    }

                    //*************create free_snode fn *************************/
                    // free top to get rid of bottom '('
                    top->next = NULL;
                    free(top->token);
                    free(top);
                    top = NULL;

                    // and need to remember freeing the tokens list in main

                    // par_counter--;
                }

                // current operator of greater precedence
                else if (priority[c] >= priority[top_op])
                {
                    push(operators_stack, tokens[i]);
                }
                // current operator of lower precedence
                else if (priority[c] < priority[top_op])
                {
                    // if peeked head of higher precedence and is not ')'  and ')' ???
                    // if (c != ')') //*** may not need
                    //{
                    // take top of stack and place it into the queue
                    Snode *t = pop(operators_stack);
                    enqueue(rpn_queue, t->token);

                    //*************create free_snode fn *************************/
                    // free top to get rid of bottom '('
                    t->next = NULL;
                    free(t->token);
                    free(t);
                    t = NULL;

                    // add tokens[i] to stack
                    push(operators_stack, tokens[i]);
                    printf("top of stack: %s\n", peek(operators_stack));
                    //}
                }
            }
            print_queue(rpn_queue);
        }
    }

    //***at end if still ( at top of stack parsing error!!!... also go until end of stack if any remaining*/
    while (!is_s_empty(operators_stack))
    {
        Snode *top = pop(operators_stack);

        if(top->token[0] == '(')
        {
            parse_error();
            //free Snode *top
            //free stack, queue, priority array 
            return NULL;
        }

        enqueue(rpn_queue, top->token);

        //*************create free_snode fn *************************/
        // free top to get rid of bottom '('
        top->next = NULL;
        free(top->token);
        free(top);
        top = NULL;
    }

    /*
    edge cases:
    Example 00
    $>./my_bc "312/0"
    divide by zero
    $>
    Example 01
    $>./my_bc "321()"
    parse error
    $>
    Example 02
    $>./my_bc "-(-((-4)+-6))"
    -10
    $>

    */

    //***temporary return */

    // free stuff
    free(priority);

    return NULL;
}

// void evaluate_rpn(rpn_queue)
//{
/*
edge case examples:
if 1 and - are left, should have that evaluate just the right value

printf("divide by zero\n");
printf("parse error\n");

    */

//   }

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Error... incorrect number of arguements. \nEnter mathematical argument in string for evaluation: ./my_bc \"1 + 2 * 3 / 4\"");
        return -1;
    }

    // count number of tokens in string
    int num_tokens = count_tokens(argv[1]);

    printf("Num tokens: %d\n", num_tokens);

    // create infix_tokens array
    char **infix_tokens = (char **)malloc(num_tokens * sizeof(char *));
    if (!infix_tokens)
    {
        alloc_error();
        return -1;
    }

    for (int i = 0; i < num_tokens; i++)
    {
        infix_tokens[i] = (char *)malloc(PS_SIZE * sizeof(char));
        if (!infix_tokens[i])
        {
            alloc_error();
            free(infix_tokens);
            return -1;
        }
        my_memset(infix_tokens[i], '\0', PS_SIZE);
    }

    // need to parse input string into array of tokens
    parse_string(argv[1], infix_tokens, num_tokens);

    // test printing
    for (int i = 0; i < num_tokens; i++)
    {
        printf("infix_tokens[%d]: %s\n", i, infix_tokens[i]);
    }

    Queue *rpn_queue = process_to_rpn(infix_tokens, num_tokens);
    if (rpn_queue == NULL)
    {
        return -1;
    }

    // evaluate_rpn(rpn_queue);

    // free and cleanup stuff
    // free infix_tokens here
    return 0;
}