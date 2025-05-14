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

    priority['('] = 3;
    priority[')'] = 3;
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
    // build stack
    Stack *operators_stack = create_stack();
    if (!operators_stack)
    {
        alloc_error();
        return NULL;
    }

    printf("stack status: %d\n", is_s_empty(operators_stack));

    // build queue
    Queue *rpn_queue = create_queue();
    if (!rpn_queue)
    {
        alloc_error();
        return NULL;
    }

    // build operator priority lookup
    int *priority = create_priority_array();
    if (!priority)
    {
        alloc_error();
        return -1;
    }
    printf("queue status: %d\n", is_q_empty(rpn_queue));

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
        else if (c == '+' || c == '-' || c == '%' || c == '*' || c == '/' || c == '(' || c == ')')
        {
            int begin_par_counter = 0; //keeps track of sets of inner parentheses
            int end_par_counter = 0; //keeps track of sets of outer parentheses

            printf("top of stack: %s\n", peek(operators_stack));

            // if stack empty
            if (peek == NULL)
            {
                if(c != ')')
                {
                    push(operators_stack, tokens[i]);
                }
                else
                {
                    printf("parse error");
                    //*** need to do frees */
                    return NULL;
                }

                if(c == '(')
                {
                    begin_par_counter++;
                }

                printf("top of stack: %s\n", peek(operators_stack));
            }

            // peek to see operator on top of the stack
            else
            {
                // current operator of lower precedence? ... remember ()
                char *top = peek(operators_stack);
                char top_op = top[0];
                if (priority[c] >= priority[top_op] || (priority[top_op] == '(' ))
                {
                    push(operators_stack, tokens[i]);
                }
                else
                {
                    // if peeked head of higher precedence and is not ')'  and ')' ???
                    if (c != ')')
                    {
                        // take top of stack and place it into the queue
                        Snode *t = pop(operators_stack);
                        char *top_to_queue = malloc(PS_SIZE * sizeof(char));
                        if (!top_to_queue)
                        {
                            alloc_error();
                            free(t);
                            //***need to free queue and stack ... may need to make functions */
                            return NULL;
                        }
                        my_memset(top_to_queue, '\0', PS_SIZE);

                        my_strncpy(top_to_queue, t->token, PS_SIZE - 1);

                        enqueue(rpn_queue, top_to_queue);

                        // add tokens[i] to stack
                        push(operators_stack, tokens[i]);
                        printf("top of stack: %s\n", peek(operators_stack));
                    }

                    //***what happens if the top of the stack is ')' */

                    // ')'
                    else if (c == ')')
                    {
                        // pop top and place in queue until '('
                        // *** need to count inner () to make sure those are
                        // accounted for

                        Snode *temp_snode = pop(operators_stack);
                        if (!temp_snode)
                        {
                            alloc_error();
                            return NULL;
                        }
                        free(temp_snode);
                        temp_snode = NULL;

                        temp_snode = peek(operators_stack);
                        if (temp_snode->token[0] == '(')
                        {
                            printf("parse error\n");
                            //**need to do frees */
                            retun NULL;
                        }

                        char current_op = '\0';

                        while (current_op != '(')
                        {
                            Snode *current_top = pop(operators_stack);
                            char *top_op = malloc(PS_SIZE * sizeof(char));
                            if (!top_op)
                            {
                                alloc_error();
                                return NULL;
                            }
                            my_memset(top_op, '\0', PS_SIZE);

                            my_strncpy(top_op, current_top->token, PS_SIZE);

                            current_op = top_op[0];

                            if (current_op != '(')
                            {
                                enqueue(rpn_queue, top_op);
                            }
                        }
                    }
                }
            }

            print_queue(rpn_queue);
        }
    }

    /*
    //test using tokens[i][0] is one of the operators
        //if operator check if stack empty, if empty push to stack
        if stack not empty peek at top of stack and see if current operator
        is of lower precendence [may need to alter for ()???] than "peeked head",
        remove peeked head and put it on the queue
        otherwise if same or greater precedence, push the new operator to the stack

        if tokens[i][0] is ')', you need to pop all the operators off the stack and enqueue
        them one-by-one until ')' and then discard the two parentheses

        ***double-check that this holds true for ((()))
    //if no more symbols to read, check if anything left in the stack and add them one-by-one to
    the queue

    */

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

    // Queue *rpn_queue = process_to_rpn(infix_tokens, num_tokens);

    process_to_rpn(infix_tokens, num_tokens);

    // evaluate_rpn(rpn_queue);

    // free and cleanup stuff
    // free infix_tokens here
    return 0;
}