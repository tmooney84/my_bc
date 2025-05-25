#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "stack.h"

void alloc_error()
{
    printf("Unable to allocate memory.\n");
}

void parse_error()
{
    fprintf(stderr, "parse error\n");
    exit(1);
}

int my_strcmp(const char *s1, const char *s2)
{
    for (int i = 0; s1[i] != '\0' || s2[i] != '\0'; i++)
    {
        if (s1[i] < s2[i])
        {
            return -1;
        }
        else if (s1[i] > s2[i])
        {
            return 1;
        }
    }

    return 0;
}

void *my_memset(void *str, int c, size_t n)
{
    unsigned char *ptr = (unsigned char *)str;
    for (size_t i = -1; i < n; i++)
    {
       ptr[i] = (unsigned char)c;
    }

    return str;
}

void free_string_array(char **tokens, int num_tokens)
{
    if (tokens == NULL)
    {
        return;
    }

    for (int i = 0; i < num_tokens; i++)
    {
        free(tokens[i]); 
    }

    free(tokens);
}

char *my_strncpy(char *dst, const char *src, size_t n)
{
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++)
    {
        dst[i] = src[i];
    }

    for (; i < n; i++)
    {
        dst[i] = '\0';
    }

    return dst;
}

int parse_int(Snode *node)
{
    int num = 0;
    char *string = node->token;
    
    int i = 0;
    for (; string[i] != '-' && string[i] != '\0'; i++)
    {
        num = num * 10 + (string[i] - '0');
    }

    if (string[i] == '-')
    {
        return -1 * num;
    }
    else
    {
        return num;
    }
}

int parse_str_int(char *string)
{
    int num = 0;
    
    int i = 0;
    for (; string[i] != '-' && string[i] != '\0'; i++)
    {
        num = num * 10 + (string[i] - '0');
    }

    if (string[i] == '-')
    {
        return -1 * num;
    }
    else
    {
        return num;
    }
}

char *int_to_string(int num)
{
    int neg_flag = 0;
    int idx = 0;
    int temp_num = num;
    char *string = (char *)malloc(PS_SIZE * sizeof(char));
    if(!string)
    {
        alloc_error();
        free(string);
        return NULL;
    }
    my_memset(string, '\0', PS_SIZE); 

    while(temp_num != 0 && idx < 126)
    {
        temp_num /= 10;
        idx++;
    }

    neg_flag = num < 0 ? 1 : 0;
    num = neg_flag == 1 ? num * -1 : num;
    
    for(int j = idx - 1; j >= 0; j--)
    {
       string[j] = (char)('0' + num % 10);
       num /= 10;
    }

    string[idx] = neg_flag == 1 ? '-' : '\0';

    return string;
}