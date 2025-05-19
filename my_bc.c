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

int find_next_sig_tok(char **tokens, int num_tokens, int idx)
{
    while (idx < num_tokens - 2)
    {
        if (tokens[idx + 1][0] == ' ')
        {
            idx++;
        }
        else
        {
            break;
        }
    }
    return idx + 1;
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

    // if last token is '-' such as in "1 + 2 * 3 -"
    if (tokens[num_tokens - 1][0] == '-')
    {
        parse_error();
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
        free_stack(operators_stack);
        alloc_error();
        return NULL;
    }
    printf("queue status: %d\n", is_q_empty(rpn_queue));

    int *priority = create_priority_array();
    if (!priority)
    {
        free_stack(operators_stack);
        free_queue(rpn_queue);
        alloc_error();
        return NULL;
    }

    for (int i = 0; i < num_tokens; i++)
    {
        char c = tokens[i][0];

        if (c >= '0' && c <= '9')
        {
            enqueue(rpn_queue, tokens[i]);
            print_queue(rpn_queue);
        }

        // cannot have 2(3+4)
        if (c >= '0' && c <= '9' && i < num_tokens - 1)
        {
            int next_sig_idx = find_next_sig_tok(tokens, num_tokens, i);
            if (next_sig_idx == -1)
            {
                parse_error();
                return NULL;
            }
            else if (tokens[next_sig_idx][0] == '(')
            {
                parse_error();
                return NULL;
            }
        }

        else if (c == '+' || c == '-' || c == '%' || c == '*' || c == '/' || c == '(' || c == ')')
        {
            printf("top of stack: %s\n", peek(operators_stack));

            if (is_s_empty(operators_stack))
            {
                if (c != ')')
                {
                    push(operators_stack, tokens[i]);
                }
                else
                {
                    parse_error();
                    free_stack(operators_stack);
                    free_queue(rpn_queue);
                    free(priority);
                    return NULL;
                }
                printf("top of stack: %s\n", peek(operators_stack));
            }

            else
            {
                char *top = peek(operators_stack);
                char top_op = top[0];

                // cannot have "()" with no contents inside or ")("
                if ((top_op == '(' && c == ')') || (top_op == ')' && c == '('))
                {
                    parse_error();
                    free_stack(operators_stack);
                    free_queue(rpn_queue);
                    free(priority);
                    return NULL;
                }
                else if (c == '(')
                {
                    push(operators_stack, tokens[i]);
                }

                // pop stack to queue until '(' reached
                else if (c == ')')
                {
                    Snode *top = pop(operators_stack);
                    char *popped_top = top->token;

                    // run until '(' found, if not found, parse error
                    while (popped_top[0] != '(')
                    {
                        enqueue(rpn_queue, popped_top);
                        if (is_s_empty(operators_stack))
                        {
                            parse_error();
                            free_stack(operators_stack);
                            free_queue(rpn_queue);
                            free(priority);
                            return NULL;
                        }
                        // free popped node
                        free_snode(top);

                        top = pop(operators_stack);
                        popped_top = top->token;
                    }

                    // free top to get rid of bottom '('
                    free_snode(top);
                }

                // current operator of greater precedence
                else if (priority[(int)c] >= priority[(int)top_op])
                {
                    push(operators_stack, tokens[i]);
                }
                // current operator of lower precedence
                else if (priority[(int)c] < priority[(int)top_op])
                {
                    Snode *t = pop(operators_stack);
                    enqueue(rpn_queue, t->token);
                    free_snode(t);

                    push(operators_stack, tokens[i]);
                    printf("top of stack: %s\n", peek(operators_stack));
                }
            }
            print_queue(rpn_queue);
        }
    }

    // finish the rest of the stack
    while (!is_s_empty(operators_stack))
    {
        Snode *top = pop(operators_stack);

        if (top->token[0] == '(')
        {
            parse_error();
            free_stack(operators_stack);
            free_queue(rpn_queue);
            free(priority);
            return NULL;
        }

        enqueue(rpn_queue, top->token);

        free_snode(top);
    }

    // free stuff
    free_stack(operators_stack);
    free(priority);

    return rpn_queue;
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

/*
edge case examples:
if 1 and - are left, should have that evaluate just the right value

printf("divide by zero\n");
printf("parse error\n");

*/

int parse_int(Qnode *node)
{
}

void evaluate_rpn(Queue *rpn_queue)
{
    // create num_stack
    Stack *num_stack = create_stack();
    if (!num_stack)
    {
        alloc_error();
        return -1;
    }
    while (is_q_empty(rpn_queue) == 0)
    {
        // first in the queue a number?
        char c = rpn_queue->front->token[0];
        while (c >= '0' && c <= '9')
        {
            // add number to stack
            Qnode *num_node = dequeue(rpn_queue);
            push(num_stack, num_node->token);
            free_qnode(num_node);
        }

        // once operator evaluate
        if (c == '+' || c == '-' || c == '%' || c == '*' || c == '/')
        {
            Qnode *op = dequeue(rpn_queue);
            Snode *temp2 = pop(num_stack);
            Snode *temp1 = pop(num_stack);

            //**********************NEED TO IMPLEMENT parse_int*********************************/
            int num2 = parse_int(temp2);
            int num1 = parse_int(temp1);

            switch (op->token[0])
            {
            case '+':
                int result = num1 + num2;
                //************************NEED TO IMPLEMENT int_to_string************/
                char *s_result = int_to_string(result);
                push(num_stack, s_result);
                break;

            case '-':
                int result = num1 - num2;
                char *s_result = int_to_string(result);
                push(num_stack, s_result);
                break;
            case '*':
                int result = num1 * num2;
                char *s_result = int_to_string(result);
                push(num_stack, s_result);
                break;

            case '/':
                // remember divide by zero
                if (num2 != 0)
                {
                    int result = num1 / num2;
                    char *s_result = int_to_string(result);
                    push(num_stack, s_result);
                }
                else
                {
                    printf("divide by zero\n");
                    free_qnode(op);
                    free_snode(temp2);
                    free_snode(temp1);
                    free_stack(num_stack);
                    return;
                }
                break;
            case '%':
                int result = num1 % num2;
                char *s_result = int_to_string(result);
                push(num_stack, s_result);
                break;

            default:
                printf("ERROR"); //*********************************???
            }
                   
                    free_qnode(op);
                    free_snode(temp2);
                    free_snode(temp1);
        }
    }

    // check if there is only a single snode on stack, pop it
    // parse it and return the int answer
    if (num_stack->head != NULL && num_stack->head->next == NULL)
    {
        Snode *answer_node = pop(num_stack);
        //**********************NEED TO IMPLEMENT parse_int*********************************/
        int answer = parse_int(answer_node);
        free_snode(answer_node);
        free_stack(num_stack);

        return answer;
    }
    else
    {
        parse_error();
        free_stack(num_stack);
        return;
    }

    free_stack(num_stack);
    return;
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

    print_queue(rpn_queue);

    //(evaluate_rpn(rpn_queue);

    // free and cleanup stuff
    free_queue(rpn_queue);
    // free infix_tokens here
    return 0;
}