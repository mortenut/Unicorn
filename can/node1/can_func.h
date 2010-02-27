#define num_of_response_mobs 5
#define data_buffer_num 5 

extern U8 response_buffer[num_of_response_mobs][8];
extern st_cmd_t response_msg[num_of_response_mobs];

extern U8 databuffer[data_buffer_num][8];
extern U8 bufferindex;

void init_can_data_mobs(void);
void can_data_mob_setup(U8 mob_num);
