#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdio.h>
#include "stack.h"

#define PS_SIZE 128

void *my_memset(void *str, int c, size_t n);
void free_string_array(char **tokens, int num_tokens);
char *my_strncpy(char *dst, const char *src, size_t n);
int my_strcmp(const char *s1, const char *s2);
void alloc_error();
void parse_error();
int parse_int(Snode *node);
char *int_to_string(int num);
int parse_str_int(char *string);

#endif
