/*----------------------------------------------------------------------*/
/* FAT file system sample project for FatFs            (C)ChaN, 2009    */
/*----------------------------------------------------------------------*/

#include "config.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include "uart.h"
#include "xitoa.h"
#include "ff.h"
#include "diskio.h"
#include "rtc.h"
#include "can_lib.h"
#include "can_func.h"
#include "../lib/can_defs.h"

#define NB_TARGET 1
#define ID_TAG_BASE 128

void can(FIL *file_to_log);
void req_sensor_data(U8 pakke, U8 node);

DWORD acc_size;				/* Work register for fs command */
WORD acc_files, acc_dirs;
FILINFO Finfo;
#if _USE_LFN
char Lfname[_MAX_LFN+1];
#endif


FATFS Fatfs[1];				/* File system object for each logical drive */
BYTE Buff[1024];			/* Working buffer */

volatile WORD Timer;		/* 100Hz increment timer */


#if _MULTI_PARTITION != 0
const PARTITION Drives[] = { {0,0}, {0,1} };
#endif

/*---------------------------------------------------------*/
/* 100Hz timer interrupt generated by OC2                  */
/*---------------------------------------------------------*/


ISR(TIMER2_COMP_vect)
{
	Timer++;			/* Performance counter for this module */
	disk_timerproc();	/* Drive timer procedure of low level disk I/O module */
}


/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */
/* This is not required in read-only configuration.        */


DWORD get_fattime ()
{
	RTC rtc;


	/* Get local time */
	rtc_gettime(&rtc);

	/* Pack date and time into a DWORD variable */
	return	  ((DWORD)(rtc.year - 1980) << 25)
			| ((DWORD)rtc.month << 21)
			| ((DWORD)rtc.mday << 16)
			| ((DWORD)rtc.hour << 11)
			| ((DWORD)rtc.min << 5)
			| ((DWORD)rtc.sec >> 1);
}

static
void IoInit ()
{
   OCR2A = 90-1;      // Timer2: 100Hz interval (OC2)
   TCCR2A = 0b00001101;

   TIMSK2 = 0b00000010;   // Enable TC2.oc interrupt

   rtc_init();         // Initialize RTC
   can_init(0);
   uart_init();
}


/*-----------------------------------------------------------------------*/
/* Main                                                                  */


int main (void)
{
	DWORD p1, p2, p3;
	BYTE res, b1;
	WORD w1;
	UINT s1, s2, cnt;
	DWORD ofs, sect = 0;
	RTC rtc;
	FATFS *fs;
	DIR dir;				/* Directory object */
	FIL file1;			/* File object */
    U8 i;
    U8 open = 0;

	IoInit();

    	uart_put(84);
	/* Join xitoa module to uart module */
	xfunc_out = (void (*)(char))uart_put;
    _delay_ms(2000);
	xprintf(PSTR("System startet\n"));
    xprintf(PSTR("Initialize disk 0\n"));    
	xprintf(PSTR("rc=%d\n"), (WORD)disk_initialize(0));
    xprintf(PSTR("Initialize logical drice 0\n"));
    xprintf(PSTR("rc=%d\n"), (WORD)f_mount(0, &Fatfs[0]));
    xprintf(PSTR("Opening file hej\n"));
    xprintf(PSTR("rc=%d\n"), (WORD)f_open(&file1, "hej",FA_WRITE)); 
    open = 1;
    _delay_ms(2000);
    init_can_data_mobs();

    for (i=0; i<num_of_response_mobs; i++) {
            can_data_mob_setup(i);
    }
    tx_remote_msg.pt_data = &tx_remote_buffer[0];
    tx_remote_msg.status = 0;

    while (1) {
           while (!(UCSR0A & 1<<RXC0));
           i = UDR0;
           switch (i) {
                case 'a':
                    can_send(rpm_msgid, 0);
                    break;
                case 'd':
                    can_send(rpm_msgid, 1);
                    break;
                case 'w':
                    can_send(rpm_msgid, 2);
                    break;
                case 's':
                    can_send(rpm_msgid, 3);
                    break;
                case '0':
                    can_send(gear_status_msgid, 0);
                    break;
                case '1':
                    can_send(gear_status_msgid, 1);
                    break;
                case '2':
                    can_send(gear_status_msgid, 2);
                    break;
                case '3':
                    can_send(gear_status_msgid, 3);
                    break;
                case '4':
                    can_send(gear_status_msgid, 4);
                    break;
                case '5':
                    can_send(gear_status_msgid, 5);
                    break;
                case '6':
                    can_send(gear_status_msgid, 6);
                    break;
                case '7':
                    can_send(gear_status_msgid, 7);
                    break;
                case '8':
                    can_send(gear_status_msgid, 8);
                    break;
                case '9':
                    can_send(gear_status_msgid, 9);
                    break;
           }
    }

    while(1) {
        if ((UCSR0A & _BV(RXC0))) {
                if (UDR0 == 'c') {
                        if (f_close(&file1) != 0) {
                            xprintf(PSTR("Error on file closing\r\n"));
                        }
                        open = 0;
                }
        }
        if (open == 1) {
            can(&file1);
        }
    }
}

void can(FIL *file)
{
    U8 i,j;
    char e;

    for (j=0; j<num_of_response_mobs; j++){
        if (can_get_status(&response_msg[j]) == CAN_STATUS_COMPLETED){
            if (bufferindex >= data_buffer_num){
                xprintf(PSTR("Buffer full error\n"));
            } else {
                for (i=0; i<9; i++) {
                        databuffer[bufferindex][i] = response_buffer[j][i];
                        response_buffer[j][i] = 0;
                }
                bufferindex++;
            }
            can_data_mob_setup(i);
        }
    }

    for (i=0; i<bufferindex; i++) {         
        xprintf(PSTR("B %d"), i); 
        /*xprintf(PSTR(", Data1: %03d"), databuffer[i][0]);
	    xprintf(PSTR(", Data2: %03d"), databuffer[i][1]);
        xprintf(PSTR(", Data3: %03d"), databuffer[i][2]);
    	xprintf(PSTR(", Data4: %03d"), databuffer[i][3]);
        xprintf(PSTR(", Data5: %03d"), databuffer[i][4]);
        xprintf(PSTR(", Data6: %03d"), databuffer[i][5]);
	    xprintf(PSTR(", Data7: %03d"), databuffer[i][6]);                
    	xprintf(PSTR(", Data8: %03d"), databuffer[i][7]);
	    xprintf(PSTR("\r\n"));*/
    }
    if (f_write(file, databuffer, 9*bufferindex, e) != 0)
            xprintf(PSTR("Write error\r\n"));
    _delay_us(1700);
    bufferindex = 0;
}
