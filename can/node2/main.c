//_____ I N C L U D E S ________________________________________________________
#include "config.h"
#include "can_lib.h"
#include <util/delay.h>
#include "led.h"
#include "../lib/can_defs.h"

//_____ D E F I N I T I O N S __________________________________________________
#define ID_BASE 0x80
//_____ D E C L A R A T I O N S ________________________________________________
void init(void);
unsigned short int can_check( U8 msg_id);
unsigned short int can_update_rx_msg(st_cmd_t* msg, U8 msg_id, U8 data_length);

//_____ VARIABLES ______________________________________________________________
// Recieve buffer
U8 rpm_response_buffer[8];
st_cmd_t rpm_msg;

U8 gear_status_response_buffer[8];
st_cmd_t gear_status_msg;

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
    rpm_msg.pt_data = &rpm_response_buffer[0];
    rpm_msg.status = 0;

    gear_status_msg.pt_data = &gear_status_response_buffer[0];
    gear_status_msg.status = 0;

    fade_in(1000, 50);
    SEG_0;
    
    can_update_rx_msg(&rpm_msg, rpm_msgid, 8);
    can_update_rx_msg(&gear_status_msg, gear_status_msgid, 1);   
    while (1)
    {
            if (can_get_status(&rpm_msg) == CAN_STATUS_COMPLETED) {  // check for rpm_msg
                    i = rpm_response_buffer[0];                     
                    can_update_rx_msg(&rpm_msg, rpm_msgid, 8);      // update rpm_msg to accept a new msg
                    if (i <= 7) {
                        LED_REG1 = (1<<i)-1;
                        LED_REG2 = 0;
                    }
                    if (i>=8) {
                        LED_REG1 = 255;
                        LED_REG2 = (1<<(i-8))-1;
                    }
 /*                   if (i == 0) {
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
                    }*/
            }
            if (can_get_status(&gear_status_msg) == CAN_STATUS_COMPLETED) {  // check for gear_status_msg
                    i = gear_status_response_buffer[0];                     
                    can_update_rx_msg(&gear_status_msg, gear_status_msgid, 1);      // update gear_status_msg to accept a new msg
 
                    switch (i) {
                            case 0:
                                    SEG_0;
                                    break;
                            case 1:
                                    SEG_1;
                                    break;
                            case 2:
                                    SEG_2;
                                    break;
                            case 3:
                                    SEG_3;
                                    break;
                            case 4:
                                    SEG_4;
                                    break;
                            case 5:
                                    SEG_5;
                                    break;
                            case 6:
                                    SEG_6;
                                    break;
                            case 7:
                                    SEG_7;
                                    break;
                            case 8:
                                    SEG_8;
                                    break;
                            case 9:
                                    SEG_9;
                                    break;
                    }
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

unsigned short int can_check( U8 msg_id)
{
    unsigned short int i = 0;
    // --- Init Rx Commands        
    for(i=0; i<8; i++)  // Nulstiller buffer
        rpm_response_buffer[i]=255;

    rpm_msg.id.std = msg_id;
    rpm_msg.ctrl.ide = 0;
    rpm_msg.ctrl.rtr = 0;
    rpm_msg.dlc = 8;               //Antal bytes der skal modtages
    rpm_msg.cmd = CMD_RX_DATA_MASKED;
    // --- Rx Command
    while(can_cmd(&rpm_msg) != CAN_CMD_ACCEPTED);
    // Venter på data er modtaget
    while(can_get_status(&rpm_msg) == CAN_STATUS_NOT_COMPLETED); 

return rpm_response_buffer[0];
}

unsigned short int can_update_rx_msg(st_cmd_t* msg, U8 msg_id, U8 dlc)
{
        U8 i;
    
        for (i=0; i<dlc; i++) {
                *(msg->pt_data+i) = 255;
        }
        
        msg->id.std = msg_id;
        msg->ctrl.ide = 0;
        msg->ctrl.rtr = 0;
        msg->dlc = dlc;
        msg->cmd = CMD_RX_DATA_MASKED;

        while(can_cmd(msg) != CAN_CMD_ACCEPTED);
}
