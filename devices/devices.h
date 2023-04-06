#pragma once
#include <types.h>


void keyboard_install();
void keyboard_wait();
uint16_t keyboard_wait_scancode();
unsigned char keyboard_scancode_to_assci_us(uint16_t scancode);

void init_video(void);
void settextcolor(unsigned char forecolor, unsigned char backcolor);
void puts(unsigned char *text);
void putsn(unsigned char *text, size_t len);
void putch(unsigned char c);
void cls();
void move_csr(void);
void scroll(void);
void printf (const char *format, ...);
void putch_at(unsigned char c, int x, int y);


void timer_wait(int ticks);
void timer_install();


uint8_t cmos_read(uint8_t index);
void cmose_write(uint8_t index, uint8_t data);


