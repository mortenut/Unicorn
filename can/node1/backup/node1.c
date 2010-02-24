//******************************************************************************
//! Via the CAN bus, the collector periodically asks to the sensor nodes
//! some specific data, and displays them.
//******************************************************************************

//_____ I N C L U D E S ________________________________________________________
#include "config.h"
#include "can_lib.h"
#include "uart_lib.h"
#include "rtc_drv.h"

#define ID_TAG_BASE 128       	// 0x80
#define NB_TARGET   1			// Antal noder

extern void display_sensor_values(void);
extern void req_sensor_data(U8 pakke, U8 node);

//------------------------------------------------------------------------------
//! This program performs some remote frames (c.f. NB_TARGET).
//! The response must be a data frame that contents (in the order) the addressed
//! node temperature, luminosity and VCC values.
//------------------------------------------------------------------------------
int main (void)
{	
    U8 i,j;    
    CLKPR = 0x80;  CLKPR = 0x00;  // Clock prescaler Reset
   
	uart_init(CONF_8BIT_NOPAR_1STOP,UART_BAUDRATE);
    
    can_init(0);

	DDRA = 0xFF; // LED'er output
	PORTA = 0xFF; // LED'er tændt

    DDRD = 0xFF;
    PORTD |= 0b1000000;


    // Interrupt
    
    
    sei();

    CANIE2 = 0xFF;
    CANIE1 = 0xFF;

    //CANGIE |=(1<<ENRX);
    //CANGIE |=(1<<ENTX);

    CANGIE |=(1<<ENIT);


    display_sensor_values();

    return 0;
}

//------------------------------------------------------------------------------
//! Periodically, the collector sends remote frames (IDs from ID_TAG_BASE up to
//!     ID_TAG_BASE+NB_TARGET) and waits for response
//!     (with time-out) from all the DVK90CAN1 of the CAN bus.
//!     The response is a data frame that contents (in the order) the
//!     local temperature, the local luminosity and the VCC values.
//!     When the DVK90CAN1 receives an response, it formates the data and
//!     sends it to a terminal.
//------------------------------------------------------------------------------
void display_sensor_values(void)
{
    U8 i, j=0;
    U8 k = 0;

    U8 response_buffer[9];
    st_cmd_t response_msg;

    // --- Init variables
    response_msg.pt_data = &response_buffer[0];
    response_msg.status = 0;

    // UART
    uart_mini_printf("GOGO");

    while (1)
    {
        wait_for(100);  // x ms between refreshed screen
        for(j=0; j<NB_TARGET; j++)
        {
            //CANGIE |=(1<<ENIT);
            // --- Init Rx Commands
      	    for(i=0; i<9; i++) response_buffer[i]=0; // Nulstiller buffer
            response_msg.id.std = ID_TAG_BASE + j;
            response_msg.ctrl.ide = 0;
            response_msg.ctrl.rtr = 0;
            response_msg.dlc = 8;
            response_msg.cmd = CMD_RX_DATA_MASKED;
            // --- Rx Command
            while(can_cmd(&response_msg) != CAN_CMD_ACCEPTED);

	    req_sensor_data(k,3);
	    k += 1;
	    if(k == 3)
		k = 0;
            wait_for(10); // Wait x ms for a response if exits

            if (can_get_status(&response_msg) == CAN_STATUS_COMPLETED){
                // --- Node ID
		uart_mini_printf("Node: %d",response_msg.id.std-127);
                
                // --- Data               
               	uart_mini_printf(", Data1: %03d", response_buffer[0]);

		uart_mini_printf(", Data2: %03d", response_buffer[1]);
		uart_mini_printf(", Data3: %03d", response_buffer[2]);
		uart_mini_printf(", Data4: %03d", response_buffer[3]);
		uart_mini_printf(", Data5: %03d", response_buffer[4]);
		uart_mini_printf(", Data6: %03d", response_buffer[5]);
		uart_mini_printf(", Data7: %03d", response_buffer[6]);                
		uart_mini_printf(", Data8: %03d", response_buffer[7]);
		uart_mini_printf("\r\n");  
            }else{
                response_msg.cmd = CMD_ABORT;
          	    while (can_cmd(&response_msg) != CAN_CMD_ACCEPTED);
            }
        }
    }
}

void
req_sensor_data(U8 pakke, U8 node){
	U8 tx_remote_buffer[9];
    	st_cmd_t tx_remote_msg;

	tx_remote_msg.pt_data = &tx_remote_buffer[0];
	tx_remote_msg.status = 0;
	
	tx_remote_buffer[0]=pakke; // Nulstiller buffer

        tx_remote_msg.id.std = ID_TAG_BASE+node;
        tx_remote_msg.ctrl.ide = 0;
        tx_remote_msg.ctrl.rtr = 1;
        tx_remote_msg.dlc = 8; // Antal data bytes der skal modtages 
        tx_remote_msg.cmd = CMD_TX_DATA;
        // --- Tx Command
        while(can_cmd(&tx_remote_msg) != CAN_CMD_ACCEPTED);

	// --- Wait for Tx remote completed
        while(can_get_status(&tx_remote_msg) == CAN_STATUS_NOT_COMPLETED);
}

/*
ISR(CANIT_vect)
{
    U8 i,j=0;
    CANGIE &=~(1<<ENIT);
    CANGIE |=(1<<ENIT);

    if (can_get_status(&response_msg) == CAN_STATUS_COMPLETED){
                // --- Node ID
				uart_mini_printf("Node: %d",response_msg.id.std-127);
                
                // --- Data               
               	uart_mini_printf(", Data1: %03d", response_buffer[0]);

				uart_mini_printf(", Data2: %03d", response_buffer[1]);
				uart_mini_printf(", Data3: %03d", response_buffer[2]);
				uart_mini_printf(", Data4: %03d", response_buffer[3]);
				uart_mini_printf(", Data5: %03d", response_buffer[4]);
				uart_mini_printf(", Data6: %03d", response_buffer[5]);
				uart_mini_printf(", Data7: %03d", response_buffer[6]);                
				uart_mini_printf(", Data8: %03d", response_buffer[7]);
				uart_mini_printf("\r\n");  

                for(i=0; i<9; i++) response_buffer[i]=0; // Nulstiller buffer
                response_msg.id.std = ID_TAG_BASE + j;
                response_msg.ctrl.ide = 0;
                response_msg.ctrl.rtr = 0;
                response_msg.dlc = 5;
                response_msg.cmd = CMD_RX_DATA_MASKED;
    }else{
        response_msg.cmd = CMD_ABORT;
  	    while (can_cmd(&response_msg) != CAN_CMD_ACCEPTED);
    }
}
*/
