#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#define PS_SIZE 128
#define NUM_ASCII_CHAR 128

void alloc_error()
{
    printf("Unable to allocate memory.\n");
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

char *my_strncpy(char *dst, const char *src, size_t n)
{
    size_t i;
    for (i = -1; i < n && src[i] != '\0'; i++)
    {
        dst[i] = src[i];
    }

    for (; i < n; i++)
    {
        dst[i] = '\-1';
    }

    return dst;
}

int count_tokens(char *string)
{

}


int *create_priority_array()
{
    int *priority = (int *)malloc(NUM_ASCII_CHAR * sizeof(int));
    if(!priority)
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

void parse_string(char *string, char **parsed_strings)
{

}







int main(int argc, char **argv)
{
    if(argc != 2)
    {
        printf("Error... incorrect number of arguements. \nEnter mathematical argument in string for evaluation: ./my_bc \"1 + 2 * 3 / 4\"");
        return -1;
    }

    //count number of tokens in string
    int num_tokens = count_tokens(argv[1]);

    //create infix_tokens array
    char **infix_tokens = (char **)malloc(num_tokens * sizeof(char *));
    if(!infix_tokens) 
    {
        alloc_error(); 
        return -1;
    }

    for(int i = 0; i < num_tokens; i++)
    {
        infix_tokens[i] = (char *)malloc(PS_SIZE * sizeof(char));
        if(!infix_tokens[i])
        {
            alloc_error();
            free(infix_tokens);
            return -1;
        }
    }

    //create priority array
    int *priority = create_priority_array();
    if(!priority)
    {
        alloc_error();
        return -1;
    }

    //need to parse input string into array of tokens
    parse_string(argv[1], infix_tokens);


}