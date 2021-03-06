/*
Copyright (C) 2015-2017  Coto

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

//fifo ipc driver: revision 1.2
#ifndef nesds13d_commonipc
#define nesds13d_commonipc


#include "wifi_shared.h"
#include "dswnifi.h"

#define fifo_requires_ack	(uint32)(0xffff1010)
#define fifo_requires_ack_execok	(uint32)(0xe1e2e3e4)
#define fifo_requires_ack_invalid	(uint32)(0xe1e4e5e7)

#define fifo_writeuint32_ack	(uint32)(0xffff1011)
#define fifo_writeuint16_ack	(uint32)(0xffff1012)
#define fifo_writeuint8_ack	(uint32)(0xffff1013)

#define fifo_readuint32_ack	(uint32)(0xffff1014)
#define fifo_readuint16_ack	(uint32)(0xffff1015)
#define fifo_readuint8_ack	(uint32)(0xffff1016)

//FIFO Hardware -> FIFO Software: GetSoftFIFO / SetSoftFIFO	/ 
#define FIFO_NDS_HW_SIZE (16*4)
#define fifo_write_ext	(uint32)(0xffff1017)
#define fifo_read_ext	(uint32)(0xffff1018)

//Project Specific
#define fifo_nesapuwrite_ext	(uint32)(0xffff1019)

//---------------------------------------------------------------------------------
typedef struct sMyIPC {
//---------------------------------------------------------------------------------
    //uint32 heartbeat;          // counts frames
    int16 touchX,   touchY;   // raw x/y
    int16 touchXpx, touchYpx; // TFT x/y pixel

    int16 touchZ1,  touchZ2;  // TSC x-panel measurements
    uint16 tdiode1,  tdiode2;  // TSC temperature diodes
    uint32 temperature;        // TSC computed temperature

    uint16 buttons;            // keypad buttons
    uint16 buttons_xy_folding;  // X, Y, /PENIRQ buttons
    
    union {
        uint8 curtime[8];        // current time response from RTC

        struct {
                u8 rtc_command;
                u8 rtc_year;           //add 2000 to get 4 digit year
                u8 rtc_month;          //1 to 12
                u8 rtc_day;            //1 to (days in month)

                u8 rtc_incr;
                u8 rtc_hours;          //0 to 11 for AM, 52 to 63 for PM
                u8 rtc_minutes;        //0 to 59
                u8 rtc_seconds;        //0 to 59
        };
    };
    u8 touched;				    //TSC touched?
    u8 touch_pendown;           //TSC already held before?
    uint16 battery;            
    uint16 aux;                

    vuint8 mailBusy;
    
	//IPC Clock
    //[0]; //yy
    //[1]; //mth
    //[2]; //dd
    //[3]; //wat - day of week?
    //[4]; //hh
    //[5]; //mm
    //[6]; //ss
    u8 clockdata[0x20];
	
	//APU Sync
	u8 apu_busy;
	
	//APU setup
	bool apu_ready;	//true yes / false no
	
    //WIFI
    bool wifi_enabled;
    bool wifi_working;    //true = yes / false = no

    //transfer queue
    u8 status; //see processor ipc read/writes flags
    u32 buf_queue[0x10];
  
	u32 * IPC_ADDR;
    char * ROM;   //pointer to ROM page
    int rom_size;   //rom total size
	
	//dswifi specific
	TdsnwifisrvStr dswifiSrv;
	
	uint32 fiforeply;	//for ret status
} tMyIPC;

//Shared Work     027FF000h 4KB    -     -    -    R/W

//IPC Struct
#define MyIPC ((tMyIPC volatile *)(0x027FF000))

//irqs
#define VCOUNT_LINE_INTERRUPT 0

//arm7 specific
#define KEY_XARM7 (1<<0)
#define KEY_YARM7 (1<<1)
#define KEY_HINGE (1<<7)

#ifdef ARM7
#define 	BIOS_IRQFLAGS   *(__irq_flags)
#endif

#define     irq_vector_addr (__irq_vector)

//NESDS special
#define FIFO_APU_PAUSE 0x1201
#define FIFO_UNPAUSE 0x1202
#define FIFO_APU_RESET 0x1204
#define FIFO_SOUND_RESET 0x1206
#define FIFO_APU_BUSY 1

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ARM9

#endif

#ifdef ARM7
#endif

//data0 = address
extern uint32 Readuint32WordACK(uint32 data0);
extern uint16 Readuint16WordACK(uint32 data0);
extern uint8 Readuint8WordACK(uint32 data0);

//data0 = address / data1 = value
extern bool Writeuint8WordACK(uint32 data0, uint8 data1);
extern bool Writeuint16WordACK(uint32 data0, uint16 data1);
extern bool Writeuint32WordACK(uint32 data0, uint32 data1);

extern bool SendMultipleWordACK(uint32 data0, uint32 data1, uint32 data2, uint32 data3);
extern uint32 SendMultipleWordByFifo(uint32 data0, uint32 data1, uint32 data2, uint32 data3, uint32 data4);

extern u8 gba_get_yearbytertc();
extern u8 gba_get_monthrtc();
extern u8 gba_get_dayrtc();
extern u8 gba_get_dayofweekrtc();
extern u8 gba_get_hourrtc();
extern u8 gba_get_minrtc();
extern u8 gba_get_secrtc();

//FIFO IPC 1.2
extern void FIFO_DRAINWRITE();
extern void FIFO_RELEASE();

extern int GetSoftFIFOCount();
extern bool SetSoftFIFO(u32 value);
extern bool GetSoftFIFO(u32 * var);

extern volatile int FIFO_SOFT_PTR;
extern volatile u32 FIFO_BUF_SOFT[FIFO_NDS_HW_SIZE/4];
extern volatile u32 FIFO_IN_BUF[FIFO_NDS_HW_SIZE/4];

extern void HandleFifoNotEmpty();
extern void HandleFifoEmpty();

//Project Specific
extern void apusetup();

#ifdef __cplusplus
}
#endif
