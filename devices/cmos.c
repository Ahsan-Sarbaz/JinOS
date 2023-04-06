#include "devices.h"
#include "../cpu/cpu.h"

uint8_t cmos_read(uint8_t index)
{
    outportb(0x70, index);
    return inportb(0x71);
}

void cmose_write(uint8_t index, uint8_t data)
{
    outportb(0x70, index);
    outportb(0x71, data);
}