#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include "lcd_custom.h"
#include <util/delay.h>
#include <stdio.h>

static void lcd_pulse(void) {
    PORTH |= (1 << PH6);  
    _delay_us(1);
    PORTH &= ~(1 << PH6); 
    _delay_us(50);
}

static void lcd_write_nibble(uint8_t data) {
    if (data & 0x01) PORTH |= (1 << PH5); else PORTH &= ~(1 << PH5); // D4
    if (data & 0x02) PORTH |= (1 << PH4); else PORTH &= ~(1 << PH4); // D5
    if (data & 0x04) PORTH |= (1 << PH3); else PORTH &= ~(1 << PH3); // D6
    if (data & 0x08) PORTE |= (1 << PE3); else PORTE &= ~(1 << PE3); // D7
    lcd_pulse();
}

void lcd_command(uint8_t cmd) {
    PORTB &= ~(1 << PB4);
    lcd_write_nibble(cmd >> 4);
    lcd_write_nibble(cmd & 0x0F);
    _delay_ms(2);
}

void lcd_char(char data) {
    PORTB |= (1 << PB4);
    lcd_write_nibble(data >> 4);
    lcd_write_nibble(data & 0x0F);
    _delay_us(50);
}

void lcd_init(void) {
    // Configure Pins as Outputs
    DDRB |= (1 << PB4);                                    
    DDRH |= (1 << PH6) | (1 << PH5) | (1 << PH4) | (1 << PH3); // E, D4-D6
    DDRE |= (1 << PE3);                                    

    _delay_ms(50);
    PORTB &= ~(1 << PB4); 
    
    // Reset sequence
    lcd_write_nibble(0x03); _delay_ms(5);
    lcd_write_nibble(0x03); _delay_us(150);
    lcd_write_nibble(0x03); _delay_us(150);
    lcd_write_nibble(0x02); 
    
    lcd_command(0x28); 
    lcd_command(0x0C); 
    lcd_command(0x06); 
    lcd_clear();
}

void lcd_clear(void) {
    lcd_command(0x01);
    _delay_ms(2);
}

void lcd_print(const char* str) {
    while (*str) lcd_char(*str++);
}

void lcd_print_int(int val) {
    char buffer[10];
    sprintf(buffer, "%d", val);
    lcd_print(buffer);
}

void lcd_setCursor(uint8_t col, uint8_t row) {
    uint8_t offsets[] = {0x00, 0x40};
    lcd_command(0x80 | (col + offsets[row]));
}