#include <avr/interrupt.h>
#include <avr/io.h>
#include "led.h"

volatile char LED_REG1 = 0;
volatile char LED_REG2 = 0;

ISR(TIMER0_OVF_vect)
{
        PORTE |= LED_REG1;
        PORTB |= LED_REG2;
}

ISR(TIMER0_COMP_vect)
{
        PORTE = 0;
        PORTB = 0;
}
