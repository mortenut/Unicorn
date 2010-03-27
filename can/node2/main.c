//_____ I N C L U D E S ________________________________________________________
#include "config.h"
#include "can_lib.h"
#include <util/delay.h>
#include "led.h"

//_____ D E F I N I T I O N S __________________________________________________
#define ID_BASE 0x80
#define NODE 3
#define RPM_LED_POWER 255 /* on in RPM_LED_POWER/255 % of the time */
//_____ D E C L A R A T I O N S ________________________________________________
extern void display_sensor_values(void);
void init(void);
unsigned short int wait_CAN_request(void);
void CAN_transmit(void);
void set_sensor_data(unsigned short int dataType);

//_____ VARIABLES ______________________________________________________________
unsigned short int dataType = 0;

// Transmit buffer
U8 tx_remote_buffer[8];
st_cmd_t tx_remote_msg;

// Recieve buffer
U8 response_buffer[8];
st_cmd_t response_msg;

int main (void)
{	
    int i=0;
    CLKPR = 0x80;  CLKPR = 0x00;  // Clock prescaler Reset

 //    Init CAN, UART, I/O
    init();

    /* Init timer for led power interrupt */
    TIMSK0 |= 1<<TOIE0 | 1<<OCIE0A;
    TCCR0A |= 1<<CS02;
    OCR0A = RPM_LED_POWER;
    sei();

    DDRE = 0xFF;
    //PORTE = 0xFF;

    DDRB = 0xFF;
    //PORTB = 0xFF;

    DDRC = 0xFF;
    //PORTC = 0xFF;

    DDRA = 0b11100000;
    //PORTA = 0xFF;
    // --- Init variables
    tx_remote_msg.pt_data = &tx_remote_buffer[0];
    tx_remote_msg.status = 0;

    response_msg.pt_data = &response_buffer[0];
    response_msg.status = 0;

    while (1)
    {
        // Venter på datakald
//        dataType = wait_CAN_request();
    LED_REG1 = 0xFF;
    LED_REG2 = 0xFF;
    for (i=0;i<256;i++) {
        OCR0A = i;
        _delay_ms(50);
    }
    
    for (i=0;i<9;i++){
                _delay_ms(100);
                LED_REG1 = 7<<i;
        }
        for (i=0;i<9;i++){
                LED_REG2 = 7<<i;
                _delay_ms(100);
        }
        for (i=0;i<8;i++){
                PORTC = 1<<i;
                _delay_ms(100);
        }
        for (i=5;i<9;i++){
                PORTA = 1<<i;
                _delay_ms(100);
        }
        PORTC = SEG_1;
        _delay_ms(300);
        PORTC = SEG_2;
        _delay_ms(300);
        PORTC = SEG_3;
        _delay_ms(300);
        PORTC = SEG_4;
        _delay_ms(300);
        PORTC = SEG_5;
        _delay_ms(300);
        PORTC = SEG_6;
        _delay_ms(300);
        PORTC = SEG_7;
        _delay_ms(300);
        PORTC = SEG_8;
        _delay_ms(300);
        PORTC = SEG_9;
        _delay_ms(300);
        
        LED15_ON;
        LED14_ON;
        _delay_ms(2000);
        LED16_ON;
    // Sætter udgående sensordata
//        set_sensor_data(dataType);

        // Transmittere sensordata
//        CAN_transmit(); 
    }

    return 0;
}

void init(void)
{
    // CAN Init
    can_init(0);

    // IO Init
    DDRA = 0xFF;
}

unsigned short int wait_CAN_request(void)
{
    unsigned short int i = 0;
    // --- Init Rx Commands        
    for(i=0; i<8; i++)  // Nulstiller buffer
        response_buffer[i]=0;

    response_msg.id.std = ID_BASE + NODE;
    response_msg.ctrl.ide = 0;
    response_msg.dlc = 1;               //Antal bytes der skal modtages
    response_msg.cmd = CMD_RX_DATA_MASKED;
   
    // --- Rx Command
    while(can_cmd(&response_msg) != CAN_CMD_ACCEPTED);

    // Venter på data er modtaget
    while(can_get_status(&response_msg) == CAN_STATUS_NOT_COMPLETED);

return response_buffer[0];}

void CAN_transmit(void)
{
    tx_remote_msg.id.std = ID_BASE;
    tx_remote_msg.ctrl.ide = 0;
    tx_remote_msg.ctrl.rtr = 1;
    tx_remote_msg.dlc = 8;
    tx_remote_msg.cmd = CMD_TX_DATA;

    // --- Tx Command
    while(can_cmd(&tx_remote_msg) != CAN_CMD_ACCEPTED);

    // --- Wait for Tx remote completed
    while(can_get_status(&tx_remote_msg) == CAN_STATUS_NOT_COMPLETED);   
}

void set_sensor_data(unsigned short int dataType)
{
    unsigned short int i = 0;

    // --- Init Tx Commands
    for(i=0; i<8; i++) 
        tx_remote_buffer[i]=0;

    if(dataType == 0)
        tx_remote_buffer[0] = 112;
    else if(dataType == 1)
        tx_remote_buffer[0] = 155;
    else if(dataType == 2)
        tx_remote_buffer[0] = 44;
}
