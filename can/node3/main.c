//_____ I N C L U D E S ________________________________________________________
#include "config.h"
#include "can_lib.h"
#include "uart_lib.h"

//_____ D E F I N I T I O N S __________________________________________________
#define ID_BASE 0x80
#define NODE 3

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
    CLKPR = 0x80;  CLKPR = 0x00;  // Clock prescaler Reset

    // Init CAN, UART, I/O
    init();

    // --- Init variables
    tx_remote_msg.pt_data = &tx_remote_buffer[0];
    tx_remote_msg.status = 0;

    response_msg.pt_data = &response_buffer[0];
    response_msg.status = 0;

    while (1)
    {
        // Venter på datakald
        dataType = wait_CAN_request();

        uart_mini_printf("DataType: %d \n\r", dataType);

        // Sætter udgående sensordata
        set_sensor_data(dataType);

        // Transmittere sensordata
        CAN_transmit(); 
    }

    return 0;
}

void init(void)
{
    // CAN Init
    can_init(0);

    // UART Init
    uart_init(CONF_8BIT_NOPAR_1STOP,UART_BAUDRATE);

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

return response_buffer[0];}

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
