#define F_CPU 16000000UL 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

volatile uint32_t timer0_millis = 0;

ISR(TIMER0_COMPA_vect) {
    timer0_millis++;
}

void init_millis() {
    TCCR0A = (1 << WGM01);              
    TCCR0B = (1 << CS01) | (1 << CS00); 
    OCR0A = 249;                        
    TIMSK0 |= (1 << OCIE0A);            
    sei();                              
}

uint32_t millis() {
    uint32_t m;
    cli(); 
    m = timer0_millis;
    sei(); 
    return m;
}

void lcd_pulse() {
    PORTH |= (1 << PH6);  
    _delay_us(1);
    PORTH &= ~(1 << PH6); 
    _delay_us(50);
}

void lcd_write_nibble(uint8_t data) {
    
    if (data & 0x01) PORTH |= (1 << PH5); else PORTH &= ~(1 << PH5); 
    if (data & 0x02) PORTH |= (1 << PH4); else PORTH &= ~(1 << PH4); 
    if (data & 0x04) PORTH |= (1 << PH3); else PORTH &= ~(1 << PH3); 
    if (data & 0x08) PORTE |= (1 << PE3); else PORTE &= ~(1 << PE3); 
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

void lcd_init() {
    _delay_ms(50);
    PORTB &= ~(1 << PB4); 
    
    
    lcd_write_nibble(0x03); _delay_ms(5);
    lcd_write_nibble(0x03); _delay_us(150);
    lcd_write_nibble(0x03); _delay_us(150);
    lcd_write_nibble(0x02); 
    
    lcd_command(0x28); 
    lcd_command(0x0C); 
    lcd_command(0x06); 
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

char keypadMap[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

uint8_t read_inRow(uint8_t r) {
    if (r == 0) return (PINA & (1 << PA2)) == 0;
    if (r == 1) return (PINA & (1 << PA1)) == 0;
    if (r == 2) return (PINA & (1 << PA0)) == 0;
    return 0;
}

void write_inCol(uint8_t c, uint8_t val) {
    if (c == 0) { if (val) PORTA |= (1<<PA4); else PORTA &= ~(1<<PA4); }
    if (c == 1) { if (val) PORTA |= (1<<PA6); else PORTA &= ~(1<<PA6); }
    if (c == 2) { if (val) PORTC |= (1<<PC7); else PORTC &= ~(1<<PC7); }
    if (c == 3) { if (val) PORTC |= (1<<PC5); else PORTC &= ~(1<<PC5); }
}

uint8_t read_outRow(uint8_t r) {
    if (r == 0) return (PING & (1 << PG0)) == 0;
    if (r == 1) return (PING & (1 << PG2)) == 0;
    if (r == 2) return (PINC & (1 << PC0)) == 0;
    return 0;
}

void write_outCol(uint8_t c, uint8_t val) {
    if (c == 0) { if (val) PORTL |= (1<<PL6); else PORTL &= ~(1<<PL6); }
    if (c == 1) { if (val) PORTL |= (1<<PL4); else PORTL &= ~(1<<PL4); }
    if (c == 2) { if (val) PORTL |= (1<<PL2); else PORTL &= ~(1<<PL2); }
    if (c == 3) { if (val) PORTL |= (1<<PL0); else PORTL &= ~(1<<PL0); }
}

char scanInsideKeypad() {
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
    return '\0';
}

char scanOutsideKeypad() {
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
    return '\0';
}


void init_hardware() {
  
    DDRB |= (1 << PB7) | (1 << PB6) | (1 << PB5);
    DDRG |= (1 << PG5);
    DDRE |= (1 << PE5);

    DDRB |= (1 << PB4);
    DDRH |= (1 << PH6) | (1 << PH5) | (1 << PH4) | (1 << PH3);
    DDRE |= (1 << PE3);

   
    DDRF = 0x00; 
    DDRK &= ~((1 << PK0) | (1 << PK1));

    
    DDRA &= ~((1 << PA2) | (1 << PA1) | (1 << PA0)); 
    PORTA |= (1 << PA2) | (1 << PA1) | (1 << PA0);   
    DDRA |= (1 << PA4) | (1 << PA6);                 
    DDRC |= (1 << PC7) | (1 << PC5);
    PORTA |= (1 << PA4) | (1 << PA6);                
    PORTC |= (1 << PC7) | (1 << PC5);

    
    DDRG &= ~((1 << PG0) | (1 << PG2));              
    DDRC &= ~(1 << PC0);
    PORTG |= (1 << PG0) | (1 << PG2);                
    PORTC |= (1 << PC0);
    DDRL |= (1 << PL6) | (1 << PL4) | (1 << PL2) | (1 << PL0); 
    PORTL |= (1 << PL6) | (1 << PL4) | (1 << PL2) | (1 << PL0); 
}

void moveUp() {
    PORTB |= (1 << PB7);  
    PORTB |= (1 << PB6);  
    PORTB &= ~(1 << PB5); 
    PORTE |= (1 << PE5);  
    PORTG &= ~(1 << PG5); 
}

void moveDown() {
    PORTB |= (1 << PB7);  
    PORTB &= ~(1 << PB6); 
    PORTB |= (1 << PB5);  
    PORTE &= ~(1 << PE5); 
    PORTG |= (1 << PG5);  
}

void stopElevator() {
    PORTB &= ~(1 << PB7); 
    PORTB &= ~(1 << PB6); 
    PORTB &= ~(1 << PB5); 
    PORTE &= ~(1 << PE5); 
    PORTG &= ~(1 << PG5); 
}

int currentFloor = 0;   
int targetFloor  = 0;   
unsigned long lastKeyPressTime = 0; 
const int debounceTime = 200; 
uint8_t insideTaskActive = 0; 

void updateDisplay() {
    lcd_setCursor(0, 0);
    lcd_print("Floor: "); lcd_print_int(currentFloor); lcd_print("  ");
    lcd_setCursor(0, 1);
    lcd_print("Dest : "); lcd_print_int(targetFloor); lcd_print("    ");
}

int main(void) {
    init_hardware();
    init_millis();
    lcd_init();

    lcd_print("hello");
    _delay_ms(1000);
    lcd_command(0x01);

    while (1) {
        
        for (int i = 0; i < 8; i++) {
            if (PINF & (1 << i)) currentFloor = i;
        }
        if (PINK & (1 << PK0)) currentFloor = 8;
        if (PINK & (1 << PK1)) currentFloor = 9;

        
        if (millis() - lastKeyPressTime > debounceTime) {
            char keyIn  = scanInsideKeypad();
            char keyOut = scanOutsideKeypad();

            if (keyIn != '\0' && keyIn >= '0' && keyIn <= '9') {
                targetFloor = keyIn - '0';
                insideTaskActive = 1; 
                lastKeyPressTime = millis();
                
                lcd_setCursor(13, 0);
                lcd_print(" IN"); 
            } 
            else if (keyOut != '\0' && keyOut >= '0' && keyOut <= '9') {
                if (!insideTaskActive) {
                    targetFloor = keyOut - '0';
                    lastKeyPressTime = millis();
                    
                    lcd_setCursor(13, 0);
                    lcd_print("OUT");
                }
            }
        }

        
        if (currentFloor < targetFloor) {
            moveUp();
        } 
        else if (currentFloor > targetFloor) {
            moveDown();
        } 
        else {
            stopElevator();
            insideTaskActive = 0; 
            lcd_setCursor(13, 0);
            lcd_print("   ");
        }

        updateDisplay();
    }
    
    return 0;
}