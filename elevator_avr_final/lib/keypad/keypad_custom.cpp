#include "keypad_custom.h"

static const char keypadMap[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

void keypad_init(void) {
    
    DDRA &= ~((1 << PA2) | (1 << PA1) | (1 << PA0)); // Rows INPUT
    PORTA |= (1 << PA2) | (1 << PA1) | (1 << PA0);   // Pullups ON
    DDRA |= (1 << PA4) | (1 << PA6);                 // Cols OUTPUT
    DDRC |= (1 << PC7) | (1 << PC5);
    PORTA |= (1 << PA4) | (1 << PA6);                // Cols HIGH
    PORTC |= (1 << PC7) | (1 << PC5);

    
    DDRG &= ~((1 << PG0) | (1 << PG2));              // Rows INPUT
    DDRC &= ~(1 << PC0);
    PORTG |= (1 << PG0) | (1 << PG2);                // Pullups ON
    PORTC |= (1 << PC0);
    DDRL |= (1 << PL6) | (1 << PL4) | (1 << PL2) | (1 << PL0); // Cols OUTPUT
    PORTL |= (1 << PL6) | (1 << PL4) | (1 << PL2) | (1 << PL0); // Cols HIGH
}


static uint8_t read_inRow(uint8_t r) {
    if (r == 0) return (PINA & (1 << PA2)) == 0;
    if (r == 1) return (PINA & (1 << PA1)) == 0;
    if (r == 2) return (PINA & (1 << PA0)) == 0;
    return 0;
}
static void write_inCol(uint8_t c, uint8_t val) {
    if (c == 0) { if (val) PORTA |= (1<<PA4); else PORTA &= ~(1<<PA4); }
    if (c == 1) { if (val) PORTA |= (1<<PA6); else PORTA &= ~(1<<PA6); }
    if (c == 2) { if (val) PORTC |= (1<<PC7); else PORTC &= ~(1<<PC7); }
    if (c == 3) { if (val) PORTC |= (1<<PC5); else PORTC &= ~(1<<PC5); }
}

static uint8_t read_outRow(uint8_t r) {
    if (r == 0) return (PING & (1 << PG0)) == 0;
    if (r == 1) return (PING & (1 << PG2)) == 0;
    if (r == 2) return (PINC & (1 << PC0)) == 0;
    return 0;
}
static void write_outCol(uint8_t c, uint8_t val) {
    if (c == 0) { if (val) PORTL |= (1<<PL6); else PORTL &= ~(1<<PL6); }
    if (c == 1) { if (val) PORTL |= (1<<PL4); else PORTL &= ~(1<<PL4); }
    if (c == 2) { if (val) PORTL |= (1<<PL2); else PORTL &= ~(1<<PL2); }
    if (c == 3) { if (val) PORTL |= (1<<PL0); else PORTL &= ~(1<<PL0); }
}

char scanInsideKeypad(void) {
    for (int col = 0; col < 4; col++) {
        write_inCol(col, 0);
        for (int row = 0; row < 3; row++) {
            if (read_inRow(row)) {
                write_inCol(col, 1);
                return keypadMap[col][row];
            }
        }
        write_inCol(col, 1);
    }
    return ' ';
}

char scanOutsideKeypad(void) {
    for (int col = 0; col < 4; col++) {
        write_outCol(col, 0);
        for (int row = 0; row < 3; row++) {
            if (read_outRow(row)) {
                write_outCol(col, 1);
                return keypadMap[col][row];
            }
        }
        write_outCol(col, 1);
    }
    return ' ';
}