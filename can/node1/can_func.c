#include "config.h"
#include "can_lib.h"
#include "can_func.h"

U8 response_buffer[num_of_response_mobs][8];
st_cmd_t response_msg[num_of_response_mobs];

U8 databuffer[data_buffer_num][8];

U8 bufferindex = 0;

U8 tx_remote_buffer[8];
st_cmd_t tx_remote_msg;

void init_can_data_mobs(void)
{
    U8 i,j;

    for (i=0; i<num_of_response_mobs; i++) {
        response_msg[i].pt_data = &response_buffer[i][0];
        response_msg[i].status = 0;
        for (j=0; j<9; j++) {
                response_buffer[i][j] = 0;
        }
    }
}

void can_data_mob_setup(U8 mob_num)
{
    response_msg[mob_num].id.std = 128;
    response_msg[mob_num].ctrl.ide = 0;
    response_msg[mob_num].ctrl.rtr = 0;
    response_msg[mob_num].dlc = 8;
    response_msg[mob_num].cmd = CMD_RX_DATA_MASKED;
    
    while (can_cmd(&response_msg[mob_num]) != CAN_CMD_ACCEPTED);
}

void can_send(U8 msg_id, U8 msg, U8 dlc)
{
    tx_remote_buffer[0] = msg; 

    tx_remote_msg.id.std = msg_id;
    tx_remote_msg.ctrl.ide = 0;
    tx_remote_msg.ctrl.rtr = 1;
    tx_remote_msg.dlc = 8;
    tx_remote_msg.cmd = CMD_TX_DATA;

    while (can_cmd(&tx_remote_msg) != CAN_CMD_ACCEPTED);

    while (can_get_status(&tx_remote_msg) == CAN_STATUS_NOT_COMPLETED);
}
