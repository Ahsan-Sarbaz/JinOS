#pragma once
#include <types.h>

void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, char val, size_t count);
uint16_t *memsetw(uint16_t *dest, uint16_t val, size_t count);

