#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

void alloc_error()
{
    printf("Unable to allocate memory.\n");
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
