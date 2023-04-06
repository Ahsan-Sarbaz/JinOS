#pragma once
#include <types.h>

size_t strlen(const char *str);
int strcmp(unsigned char* s1, unsigned  char* s2);
char* strpbrk(const char *s1, const char *s2);
char *strtok(char *str, const char *delim);
bool str_empty_or_whitespace(char* str);
char* str_trimfront(char* str);
int atoi(const char *str);
void itoa(char *buf, int base, int d);
bool isalpha(uint8_t c);
bool isalnum(uint8_t c);
bool isspace(uint8_t c);
bool isdigit(uint8_t c);
uint8_t tolower(uint8_t c);
uint8_t toupper(uint8_t c);

long int strtol(const char *nptr, char **endptr, int base);
