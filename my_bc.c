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
    /*
//build stack

//build queue

//test using tokens[i][0] is a number
//if a number enqueue it

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
}


void evaluate_rpn(rpn_queue)
{
/*    
edge case examples:
if 1 and - are left, should have that evaluate just the right value

printf("divide by zero\n");
printf("parse error\n");

    */

    }

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Error... incorrect number of arguements. \nEnter mathematical argument in string for evaluation: ./my_bc \"1 + 2 * 3 / 4\"");
        return -1;
    }

    // count number of tokens in string
    int num_tokens = count_tokens(argv[1]);

    printf("Num tokens: %d", num_tokens);


    
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

    // create priority array
    int *priority = create_priority_array();
    if (!priority)
    {
        alloc_error();
        return -1;
    }

    // need to parse input string into array of tokens
    parse_string(argv[1], infix_tokens, num_tokens);
    
    //test printing
    for(int i = 0; i < num_tokens; i++)
    {
        printf("infix_tokens[%d]: %s\n", i, infix_tokens[i]);
    }

    Queue * rpn_queue = process_to_rpn(infix_tokens, num_tokens);

    evaluate_rpn(rpn_queue);



    // free and cleanup stuff
    return 0;
}