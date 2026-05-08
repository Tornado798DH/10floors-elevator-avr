#ifndef LCD_CUSTOM_H
#define LCD_CUSTOM_H

#include <avr/io.h>
#include <stdint.h>

void lcd_init(void);
void lcd_command(uint8_t cmd);
void lcd_char(char data);
void lcd_print(const char* str);
void lcd_print_int(int val);
void lcd_setCursor(uint8_t col, uint8_t row);
void lcd_clear(void);

#endif 