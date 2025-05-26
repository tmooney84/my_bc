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
        if (is_alpha(string[i]))
        {
            parse_error();
        }

        if (is_op(string[i]))
        {
            token_count++;
            i++;
        }

        else if (is_num(string[i]))
        {
            i++;

            while (is_num(string[i]) && i < PS_SIZE - 2)
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

char **build_string_array(int num_strings, int string_size)
{
    char **array = malloc(num_strings * sizeof(char *));
    if (!array)
    {
        alloc_error();
        return NULL;
    }
    for (int i = 0; i < num_strings; i++)
    {
        array[i] = malloc(string_size * sizeof(char));
        if (!array[i])
        {
            alloc_error();
            return NULL;
        }
    }
    return array;
}

char **expand_neg_par(char **tokens, int *num_tokens, int idx)
{
    //-(4) >>> 1- *(4)  and (-(4)) >>> (1- *(4))
    if (idx == 0 || tokens[idx - 1][0] == '(')
    {
        *num_tokens += 1;
        tokens[idx][0] = '1';
        tokens[idx][1] = '-';

        char **temp_list = build_string_array(*num_tokens, PS_SIZE);
        if (!temp_list)
        {
            return NULL;
        }

        char *token1 = malloc(PS_SIZE * sizeof(char));
        if (!token1)
        {
            alloc_error();
            return NULL;
        }
        my_memset(token1, '\0', PS_SIZE);
        token1[0] = '*';

        int k = 0;
        int j = 0;
        for (; j <= idx && j < PS_SIZE - 1 && k < PS_SIZE; j++, k++)
        {
            my_strncpy(temp_list[j], tokens[k], PS_SIZE - 1);
        }

        free(temp_list[j]);
        temp_list[j] = token1;
        j += 1;

        for (; j < *num_tokens && k < *num_tokens - 1; j++, k++)
        {
            my_strncpy(temp_list[j], tokens[k], PS_SIZE - 1);
        }

        free_string_array(tokens, *num_tokens - 1);
        tokens = temp_list;
        temp_list = NULL;
    }
    else
    {
        *num_tokens += 2;
        // account for '-(' >>> '1 -(2 * 3)' == '1 + -1 *(2 * 3)'
        tokens[idx][0] = '*';

        char **temp_list = build_string_array(*num_tokens, PS_SIZE);
        if (!temp_list)
        {
            return NULL;
        }

        // then the two tokens before are entered '+' and '1-'
        char *token1 = malloc(PS_SIZE * sizeof(char));
        if (!token1)
        {
            alloc_error();
            return NULL;
        }
        my_memset(token1, '\0', PS_SIZE);
        token1[0] = '+';

        char *token2 = malloc(PS_SIZE * sizeof(char));
        if (!token1)
        {
            alloc_error();
            return NULL;
        }
        my_memset(token2, '\0', PS_SIZE);
        token2[0] = '1';
        token2[1] = '-';

        int k = 0;
        int j = 0;
        for (; j < idx && j < PS_SIZE - 2 && k < PS_SIZE; j++, k++)
        {
            my_strncpy(temp_list[j], tokens[k], PS_SIZE - 1);
        }

        free(temp_list[j]);
        free(temp_list[j + 1]);
        temp_list[j] = token1;
        temp_list[j + 1] = token2;
        j += 2;

        for (; j < *num_tokens && k < *num_tokens - 2; j++, k++)
        {
            my_strncpy(temp_list[j], tokens[k], PS_SIZE - 1);
        }

        free_string_array(tokens, *num_tokens - 2);
        tokens = temp_list;
        temp_list = NULL;
    }

    return tokens;
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
        if (is_op(string[i]))
        {
            parsed_tokens[pa_idx][0] = string[i];
            parsed_tokens[pa_idx][1] = '\0';
            pa_idx++;
            i++;
            s_idx = 0;
        }
        else if (is_num(string[i]))
        {
            parsed_tokens[pa_idx][s_idx] = string[i];
            s_idx++;
            i++;

            while (is_num(string[i]))
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

int finish_stack_eval(Stack *operators_stack, Queue *rpn_queue)
{
    while (!is_s_empty(operators_stack))
    {
        Snode *top = pop(operators_stack);

        if (top->token[0] == '(')
        {
            parse_error();
            free_stack(operators_stack);
            free_queue(rpn_queue);
            return -1;
        }

        enqueue(rpn_queue, top->token);

        free_snode(top);
    }

    return 0;
}

int eval_empty_stack(Stack *operators_stack, Queue *rpn_queue, char **tokens, int num_tokens, char c, int *i)
{
    char b4c = '\0';

    if (*i > 0)
    {
        b4c = tokens[*i - 1][0];
    }

    int next_sig_idx = find_next_sig_tok(tokens, num_tokens, *i);
    if (next_sig_idx == -1)
    {
        parse_error();
        return -1;
    }
    int n_next_sig_idx = find_next_sig_tok(tokens, num_tokens, next_sig_idx);
    if (n_next_sig_idx == -1)
    {
        parse_error();
        return -1;
    }
    if (is_op(b4c) && (c == '-' && is_num(tokens[next_sig_idx][0])))
    {
        int k = 0;
        //***********Create end_num() function
        while (tokens[next_sig_idx][k] != '\0' && k < PS_SIZE - 1)
        {
            k++;
        }
        tokens[next_sig_idx][k] = '-';
        enqueue(rpn_queue, tokens[next_sig_idx]);
        *i = next_sig_idx;
    }

    else if (c != ')')
    {
        push(operators_stack, tokens[*i]);
    }
    else
    {
        parse_error();
        free_stack(operators_stack);
        free_queue(rpn_queue);
        return -1;
    }

    // accounts for '1+-9'
    if (*i < num_tokens - 2 && tokens[next_sig_idx][0] == '-' && is_num(tokens[n_next_sig_idx][0]))
    {
        int j = 0;
        while (tokens[*i + 2][j] != '\0' && j < PS_SIZE - 1)
        {
            j++;
        }
        tokens[*i + 2][j] = '-';
        enqueue(rpn_queue, tokens[*i + 2]);
        *i += 2;
    }
    return 0;
}

int eval_full_stack(Stack *operators_stack, Queue *rpn_queue, int *priority, char **tokens, char c, int i)
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
        return -1;
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
                return -1;
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
    else if (priority[(int)c] > priority[(int)top_op])
    {

        push(operators_stack, tokens[i]);
    }
    // current operator of lower precedence
    else if (priority[(int)c] <= priority[(int)top_op])
    {
        Snode *t = pop(operators_stack);
        enqueue(rpn_queue, t->token);
        free_snode(t);

        push(operators_stack, tokens[i]);
    }
    return 0;
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
    }

    Stack *operators_stack = create_stack();
    if (!operators_stack)
    {
        alloc_error();
        return NULL;
    }

    Queue *rpn_queue = create_queue();
    if (!rpn_queue)
    {
        free_stack(operators_stack);
        alloc_error();
        return NULL;
    }

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

        if (is_num(c))
        {
            enqueue(rpn_queue, tokens[i]);
        }

        // cannot have 2(3+4)
        if (is_num(c) && i < num_tokens - 1)
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

        // account for '-(' >>> '1 -(2 * 3)' == '1 + -1 *(2 * 3)'
        if (i < num_tokens - 3)
        {
            int next_sig_idx = find_next_sig_tok(tokens, num_tokens, i);
            if (next_sig_idx == -1)
            {
                parse_error();
                return NULL;
            }

            // account for '-(' >>> '1 -(2 * 3)' == '1 + -1 *(2 * 3)'
            if (c == '-' && tokens[next_sig_idx][0] == '(')
            {
                tokens = expand_neg_par(tokens, &num_tokens, i);
                c = tokens[i][0];
            }
        }

        if (is_op(c))
        {
            if (is_s_empty(operators_stack))
            {
                if (eval_empty_stack(operators_stack, rpn_queue, tokens, num_tokens, c, &i) < 0)
                {
                    free(priority);
                    return NULL;
                }
            }
            else
            {

                if (eval_full_stack(operators_stack, rpn_queue, priority, tokens, c, i) < 0)
                {
                    return NULL;
                }
            }
        }
    }

    if (finish_stack_eval(operators_stack, rpn_queue) < 0)
    {
        free(priority);
        return NULL;
    }

    free_stack(operators_stack);
    free(priority);

    return rpn_queue;
}

int eval_ops(Queue *rpn_queue, Stack *num_stack)
{
    Qnode *op = dequeue(rpn_queue);
    if (is_s_empty(num_stack))
    {
        parse_error();
    }
    Snode *temp2 = pop(num_stack);
    if (is_s_empty(num_stack))
    {
        parse_error();
    }
    Snode *temp1 = pop(num_stack);

    int num2 = parse_int(temp2);
    int num1 = parse_int(temp1);
    int result = -999;

    switch (op->token[0])
    {
    case '+':
        result = num1 + num2;
        char *s_result = int_to_string(result);
        push(num_stack, s_result);
        free(s_result);
        break;

    case '-':
        result = num1 - num2;
        s_result = int_to_string(result);
        push(num_stack, s_result);
        free(s_result);
        break;
    case '*':
        result = num1 * num2;
        s_result = int_to_string(result);
        push(num_stack, s_result);
        free(s_result);
        break;

    case '/':
        // remember divide by zero
        if (num2 != 0)
        {
            result = num1 / num2;
            s_result = int_to_string(result);
            push(num_stack, s_result);
            free(s_result);
        }
        else
        {
            fprintf(stderr, "divide by zero\n");

            free_qnode(op);
            free_snode(temp2);
            free_snode(temp1);
            free_stack(num_stack);
            exit(1);
        }
        break;
    case '%':
        result = num1 % num2;
        s_result = int_to_string(result);
        push(num_stack, s_result);
        free(s_result);
        break;

    default:
        printf("ERROR");
    }

    free_qnode(op);
    free_snode(temp2);
    free_snode(temp1);

    return 0;
}

int evaluate_rpn(Queue *rpn_queue)
{
    // create num_stack
    Stack *num_stack = create_stack();
    if (!num_stack)
    {
        alloc_error();
        return 1;
    }
    while (is_q_empty(rpn_queue) == 0)
    {
        // first in the queue a number?
        char c = rpn_queue->front->token[0];
        while (is_num(c))
        {
            // add number to stack
            Qnode *num_node = dequeue(rpn_queue);
            push(num_stack, num_node->token);
            free_qnode(num_node);
            c = rpn_queue->front->token[0];
        }

        // once operator evaluate
        if (c == '+' || c == '-' || c == '%' || c == '*' || c == '/')
        {
            eval_ops(rpn_queue, num_stack);
        }
    }

    // check if there is only a single snode on stack, pop it
    // parse it and return the int answer
    if (num_stack->head != NULL && num_stack->head->next == NULL)
    {
        Snode *answer_node = pop(num_stack);
        int answer = parse_int(answer_node);
        printf("%d\n", answer);

        free_snode(answer_node);
        free_stack(num_stack);

        return 0;
    }
    else
    {
        parse_error();
        free_stack(num_stack);
        return 1;
    }

    free_stack(num_stack);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Error... incorrect number of arguements. \nEnter mathematical argument in string for evaluation: ./my_bc \"1 + 2 * 3 / 4\"");
        return -1;
    }

    char *input = argv[1];

    int num_tokens = count_tokens(input);

    if (num_tokens == 1)
    {
        int i = 0;

        while (input[i] != '\0')
        {
            if (is_alpha(input[i]))
            {
                parse_error();
            }

            i++;
        }
        int num = parse_str_int(input);
        printf("%d\n", num);
        return 0;
    }

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

    parse_string(input, infix_tokens, num_tokens);

    Queue *rpn_queue = process_to_rpn(infix_tokens, num_tokens);
    if (rpn_queue == NULL)
    {
        printf("Error building rpn_queue");
        return -1;
    }

    if (evaluate_rpn(rpn_queue) == 1)
    {
        return 1;
    }

    // free and cleanup stuff
    free_queue(rpn_queue);
    free_string_array(infix_tokens, num_tokens);
    return 0;
}