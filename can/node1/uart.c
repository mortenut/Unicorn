/*------------------------------------------------*/
/* UART functions                                 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"
#include "uart.h"

void USART0_Transmit( unsigned char txData )
{
   /* Wait for empty transmit buffer */
   while ( !( UCSR0A & (1<<UDRE0)) );
   /* Put data into buffer, sends the data */
   UDR0 = txData;
}

void USART1_Transmit( unsigned char txData )
{
   /* Wait for empty transmit buffer */
   while ( !( UCSR1A & (1<<UDRE1)) );
   /* Put data into buffer, sends the data */
   UDR1 = txData;
}

void USART_set_baud_rate(double baudrate0, double baudrate1)
{
   // calculate division factor for requested baud rate, and set it
   int bauddiv0 = ((F_CPU+(baudrate0*8L))/(baudrate0*16L)-1);
   int bauddiv1 = ((F_CPU+(baudrate1*8L))/(baudrate1*16L)-1);
   UBRR0L= bauddiv0;
#ifdef UBRR0H
   UBRR0H= (bauddiv0>>8);
#endif

   UBRR1L= bauddiv1;
#ifdef UBRR1H
   UBRR1H= (bauddiv1>>8);
#endif
}

/* Initialize UART */

void uart_init()
{
   UCSR0B = (1<<RXEN0)|(1<<TXEN0);      // Turn on U(S)ART port
   UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);   // Set frame format: 8 data bits, 1 stop bit, no parity

   UCSR1B = (1<<RXEN0)|(1<<TXEN0);      // Turn on U(S)ART port
   UCSR1C = (1<<UCSZ01)|(1<<UCSZ00);   // Set frame format: 8 data bits, 1 stop bit, no parity
   
   USART_set_baud_rate(BAUD0, BAUD1); //Set baud rate
}

/* Get a received character */
uint8_t uart_get ()
{
   unsigned char d;
   while ((UCSR0A & (1 << RXC0)) == 0) {}; // Do nothing until data have been recieved and is ready to be read from UDR
   d=UDR0;
   return d;
}

/* Transmit a character */
void uart_put(uint8_t d)
{
   
   USART0_Transmit( d );
}

/* Transmit a string */
void uart_puts(const char *s)
{
   while (*s)
      USART0_Transmit( *s++ );
}

void uart1_put(uint8_t d)
{
        USART1_Transmit(d);
}

void uart1_puts(const char *s)
{
    while (*s)
            USART1_Transmit(*s++);
}
