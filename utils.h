#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void *my_memset(void *str, int c, size_t n);
char *my_strncpy(char *dst, const char *src, size_t n);
int my_strcmp(const char *s1, const char *s2);
void alloc_error();

#endif
