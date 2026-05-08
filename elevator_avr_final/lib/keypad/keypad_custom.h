#ifndef KEYPAD_CUSTOM_H
#define KEYPAD_CUSTOM_H

#include <avr/io.h>
#include <stdint.h>

void keypad_init(void);
char scanInsideKeypad(void);
char scanOutsideKeypad(void);

#endif 