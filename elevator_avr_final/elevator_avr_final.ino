
#define F_CPU 16000000UL

#include <lcd_custom.h>
#include <keypad_custom.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint32_t timer0_millis = 0;

ISR(TIMER0_COMPA_vect) {
    timer0_millis++;
}

void init_millis(void) {
    TCCR0A = (1 << WGM01);              
    TCCR0B = (1 << CS01) | (1 << CS00); 
    OCR0A = 249;                        
    TIMSK0 |= (1 << OCIE0A);            
    sei();                              
}

uint32_t millis(void) {
    uint32_t m;
    cli(); 
    m = timer0_millis;
    sei(); 
    return m;
}

void init_elevator_hardware(void) {
    DDRB |= (1 << PB7) | (1 << PB6) | (1 << PB5);
    DDRG |= (1 << PG5);
    DDRE |= (1 << PE5);
    DDRF = 0x00; 
    DDRK &= ~((1 << PK0) | (1 << PK1));
}

void moveUp(void) {
    PORTB |= (1 << PB7) | (1 << PB6); PORTB &= ~(1 << PB5); 
    PORTE |= (1 << PE5); PORTG &= ~(1 << PG5);             
}

void moveDown(void) {
    PORTB |= (1 << PB7) | (1 << PB5); PORTB &= ~(1 << PB6); 
    PORTE &= ~(1 << PE5); PORTG |= (1 << PG5);            
}

void stopElevator(void) {
    PORTB &= ~((1 << PB7) | (1 << PB6) | (1 << PB5));      
    PORTE &= ~(1 << PE5); PORTG &= ~(1 << PG5);            
}

int currentFloor = 0;   
int targetFloor  = 0;   
uint8_t insideTaskActive = 0; 
unsigned long lastKeyPressTime = 0; 
const int debounceTime = 200; 

void updateDisplay(void) {
    lcd_setCursor(0, 0);
    lcd_print("Floor: "); lcd_print_int(currentFloor); lcd_print("  ");
    lcd_setCursor(0, 1);
    lcd_print("Dest : "); lcd_print_int(targetFloor); lcd_print("    ");
}

int main(void) {
    init_elevator_hardware();
    keypad_init();
    lcd_init();
    init_millis();

    lcd_print("hello team");
    _delay_ms(1000);
    lcd_clear();

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