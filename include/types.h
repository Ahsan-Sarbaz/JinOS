#pragma once

typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef unsigned int size_t;

typedef int8_t bool;
#define true 1
#define false 0

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))
#define NULL 0

