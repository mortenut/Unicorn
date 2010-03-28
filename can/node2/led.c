#include "config.h"
#include "compiler.h"
#include "led.h"
#include <util/delay.h>

void init_led()
{
        DDRE = 0xFF; // Første 8 rpm dioder til output
        DDRB = 0xFF; // Næste 8     ----- | | ----
        DDRC = 0xFF; // 7-seg dioder som output
        DDRA = 0b11100000;  // Temp dioder som output

        TIMSK0 |= 1<<TOIE0 | 1<<OCIE0A; // Slår timer interrupt til
        TCCR0A |= 1<<CS02;              // prescaler til 256
        OCR0A = RPM_LED_POWER;          // Sætter start styrke
}

void fade_in(int time, U8 step)
{
    U8 i,j;
    U8 delay;
    float factor;
    
    delay = time/step;
    factor = (256.0/step);
    
    LED_REG1 = 0xFF;
    LED_REG2 = 0xFF;
    for (i=1;i<step;i++) {
            OCR0A = (int)i*factor;
            _delay_ms(delay);
    } 
    LED_REG1 = 0;
    LED_REG2 = 0;
}
