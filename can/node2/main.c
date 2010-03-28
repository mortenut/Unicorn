//_____ I N C L U D E S ________________________________________________________
#include "config.h"
#include "can_lib.h"
#include <util/delay.h>
#include "led.h"

//_____ D E F I N I T I O N S __________________________________________________
#define ID_BASE 0x80
//_____ D E C L A R A T I O N S ________________________________________________
void init(void);
unsigned short int wait_CAN_request(void);

//_____ VARIABLES ______________________________________________________________
// Recieve buffer
U8 response_buffer[8];
st_cmd_t response_msg;

int main (void)
{	
    U8 i=0;
    int j=0;
    int k=12;
    CLKPR = 0x80;  CLKPR = 0x00;  // Clock prescaler Reset

//  Init CAN, UART, I/O
    init();

    init_led();
    sei();

    // --- Init variables
    response_msg.pt_data = &response_buffer[0];
    response_msg.status = 0;

    fade_in(2000, 50);
    SEG_0;
    while (1)
    {
            i = wait_CAN_request();
            if (i == 0) {
                if (j>0) {
                        j--;
                        if (j<8)
                            LED_REG1 ^=  (1<<j);
                        else
                            LED_REG2 ^=  (1<<(j-8));
                }
            }
            else if (i == 1) {
                if (j<16) {
                        if (j<8)
                            LED_REG1 |= (1<<j);
                        else
                            LED_REG2 |= (1<<(j-8));
                        j++;
                }
            }
            else if (i == 2) {
                if (k<=11) {
                        k++;
                        OCR0A = k*20;
                }
            }
            else if (i == 3) {
                if (k>1) {
                        k--;
                        OCR0A = k*20;
                }
                if (k==1) 
                        OCR0A = 2;
            }
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

    response_msg.id.std = 150;
    response_msg.ctrl.ide = 0;
    response_msg.ctrl.rtr = 0;
    response_msg.dlc = 8;               //Antal bytes der skal modtages
    response_msg.cmd = CMD_RX_DATA_MASKED;
    // --- Rx Command
    while(can_cmd(&response_msg) != CAN_CMD_ACCEPTED);
    // Venter på data er modtaget
    while(can_get_status(&response_msg) == CAN_STATUS_NOT_COMPLETED);
return response_buffer[0];
}
