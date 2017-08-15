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

//Coto: these are my FIFO handling libs. Works fine with NIFI (trust me this is very tricky to do without falling into freezes).
//Use it at your will, just make sure you read WELL the descriptions below.

//fifo ipc driver: revision 1.1

#include <nds.h>
#include "common_shared.h"

#ifdef ARM7

#include <string.h>

#include "wifi_arm7.h"
#include "arm7.h"
#include "audiosys.h"
#include "handler.h"
#include "c_defs.h"
#include "ds_misc.h"

#endif

#ifdef ARM9

#include "c_defs.h"
#include "ds_misc.h"

#endif
//Software FIFO calls, Rely on Hardware FIFO calls so it doesnt matter if they are in different maps 
#ifdef ARM9
__attribute__((section(".dtcm")))
#endif    
volatile int FIFO_SOFT_PTR = 0;
#ifdef ARM9
__attribute__((section(".dtcm")))
#endif    
volatile u32 FIFO_BUF_SOFT[FIFO_NDS_HW_SIZE/4];

//GetSoftFIFO: Stores up to FIFO_NDS_HW_SIZE. Exposed to usercode for fetching 64 bytes sent from other core, until it returns false (empty buffer).

//Example: 
//u32 n = 0;
//while(GetSoftFIFO(&n)== true){
//	//n has 4 bytes from the other ARM Core.
//}
#ifdef ARM9
__attribute__((section(".itcm")))
#endif    
inline bool GetSoftFIFO(u32 * var)
{
	if(FIFO_SOFT_PTR > 0){
		FIFO_SOFT_PTR--;
		
		*var = (u32)FIFO_BUF_SOFT[FIFO_SOFT_PTR];
		FIFO_BUF_SOFT[FIFO_SOFT_PTR] = (u32)0;
		
		return true;
	}
	else
		return false;
}

//SetSoftFIFO == false means FULL
#ifdef ARM9
__attribute__((section(".itcm")))
#endif    
//returns ammount of inserted U32 blocks into FIFO hardware regs
inline bool SetSoftFIFO(u32 value)
{
	if(FIFO_SOFT_PTR < (int)(FIFO_NDS_HW_SIZE/4)){
		FIFO_BUF_SOFT[FIFO_SOFT_PTR] = value;
		FIFO_SOFT_PTR++;
		return true;
	}
	else
		return false;
}

// Ensures a SendArm[7/9]Command (FIFO message) command to be forcefully executed at target ARM Core, while the host ARM Core awaits. 
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
inline void FIFO_DRAINWRITE(){
	while (!(REG_IPC_FIFO_CR & IPC_FIFO_SEND_EMPTY)){}
}


#ifdef ARM9
__attribute__((section(".itcm")))
#endif
inline bool SendMultipleWordACK(uint32 data0, uint32 data1, uint32 data2, uint32 data3){
	uint32 reply = SendMultipleWordByFifo(fifo_requires_ack, data0, data1, data2, data3);
	if(reply == fifo_requires_ack_execok){
		return true;
	}
	return false;
}


#ifdef ARM9
__attribute__((section(".itcm")))
#endif
inline uint8 Readuint8WordACK(uint32 data0){			//data0 address -> return code: uint8 value from address
	
	//invalidate cache here (arm9)
	#ifdef ARM9
	//Prevent Cache problems.
	DC_FlushRange((u32*)data0, (int)4);
	#endif
	
	return (uint8)SendMultipleWordByFifo((uint32)fifo_readuint8_ack, (uint32)data0, (uint32)0, (uint32)0, (uint32)0);
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
inline uint16 Readuint16WordACK(uint32 data0){			//data0 address -> return code: uint8 value from address
	
	//invalidate cache here (arm9)
	#ifdef ARM9
	//Prevent Cache problems.
	DC_FlushRange((u32*)data0, (int)4);
	#endif
	
	return (uint16)SendMultipleWordByFifo((uint32)fifo_readuint16_ack, (uint32)data0, (uint32)0, (uint32)0, (uint32)0);
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
inline uint32 Readuint32WordACK(uint32 data0){			//data0 address -> return code: uint8 value from address
	
	//invalidate cache here (arm9)
	#ifdef ARM9
	//Prevent Cache problems.
	DC_FlushRange((u32*)data0, (int)4);
	#endif
	
	return (uint32)SendMultipleWordByFifo((uint32)fifo_readuint32_ack, (uint32)data0, (uint32)0, (uint32)0, (uint32)0);
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
inline bool Writeuint8WordACK(uint32 data0, uint8 data1){			//data0 address / data1 value *uint32)
	
	//invalidate cache here (arm9)
	#ifdef ARM9
	//Prevent Cache problems.
	DC_FlushRange((u32*)data0, (int)4);
	#endif
	
	uint32 reply = SendMultipleWordByFifo((uint32)fifo_writeuint8_ack, (uint32)data0, (uint32)data1, (uint32)0, (uint32)0);
	if(reply == data1){
		return true;
	}
	return false;
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
inline bool Writeuint16WordACK(uint32 data0, uint16 data1){			//data0 address / data1 value *uint32)
	
	//invalidate cache here (arm9)
	#ifdef ARM9
	//Prevent Cache problems.
	DC_FlushRange((u32*)data0, (int)4);
	#endif
	
	uint32 reply = SendMultipleWordByFifo((uint32)fifo_writeuint16_ack, (uint32)data0, (uint32)data1, (uint32)0, (uint32)0);
	if(reply == data1){
		return true;
	}
	return false;
}


#ifdef ARM9
__attribute__((section(".itcm")))
#endif
inline bool Writeuint32WordACK(uint32 data0, uint32 data1){			//data0 address / data1 value *uint32)
	
	//invalidate cache here (arm9)
	#ifdef ARM9
	//Prevent Cache problems.
	DC_FlushRange((uint32*)data0, (int)4);
	#endif
	
	uint32 reply = SendMultipleWordByFifo((uint32)fifo_writeuint32_ack, (uint32)data0, (uint32)data1, (uint32)0, (uint32)0);
	if(reply == data1){
		return true;
	}
	return false;
}

//Internal, not for usercode
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
inline uint32 SendMultipleWordByFifo(uint32 data0, uint32 data1, uint32 data2, uint32 data3, uint32 data4)	//channel / multiple words / err reporting
{	
	FIFO_DRAINWRITE();
	
	//data4 to be used from other core
	REG_IPC_FIFO_TX =	(uint32)data0; 	//raise irq here
	REG_IPC_FIFO_TX = 	(uint32)data1;
	REG_IPC_FIFO_TX = 	(uint32)data2;
	REG_IPC_FIFO_TX = 	(uint32)data3;
	REG_IPC_FIFO_TX = 	(uint32)data4;
	
	FIFO_DRAINWRITE();
	 
	REG_IPC_FIFO_CR |= (1<<3); //3     W    Send Fifo Clear             (0=Nothing, 1=Flush Send Fifo)
	
	return MyIPC->fiforeply;
}

//project specific

//extern void APUSoundWrite(Uint address, Uint value);
#ifdef ARM9 //arm9 only
__attribute__((section(".itcm")))
inline bool WriteAPUNESACK(uint32 data0, uint32 data1){			
	
	//invalidate cache here (arm9)
	//Prevent Cache problems.
	DC_FlushRange((uint32*)data0, (int)4);
	
	uint32 reply = SendMultipleWordByFifo((uint32)fifo_apunes_write, (uint32)data0, (uint32)data1, (uint32)0, (uint32)0);
	if(reply == data1){
		return true;
	}
	return false;
}
#endif



//FIFO HANDLER INIT
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
inline void HandleFifoEmpty(){
	
}

//FIFO HANDLER INIT
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
inline void HandleFifoNotEmpty(){
	volatile uint32 cmd1 = 0,cmd2 = 0,cmd3 = 0,cmd4 = 0,cmd5 = 0,cmd6 = 0,cmd7 = 0,cmd8 = 0,cmd9 = 0,cmd10 = 0,cmd11 = 0,cmd12 = 0,cmd13 = 0,cmd14 = 0,cmd15 = 0;
	
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
		cmd1 = REG_IPC_FIFO_RX;
	}
	
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
		cmd2 = REG_IPC_FIFO_RX;
	}
	
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
		cmd3 = REG_IPC_FIFO_RX;
	}
	
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
		cmd4 = REG_IPC_FIFO_RX;
	}
	
	if(!(REG_IPC_FIFO_CR & IPC_FIFO_RECV_EMPTY)){
		cmd5 = REG_IPC_FIFO_RX;
	}
	
	if(cmd1 == fifo_requires_ack){
		switch (cmd2) {
			//ARM7 command handler
			#ifdef ARM7
			case FIFO_APU_PAUSE:
				APU_paused=1;
				memset((u32*)buffer,0,sizeof(buffer));
			break;
			case FIFO_UNPAUSE:
				APU_paused=0;
				break;
			case FIFO_APU_RESET:
				memset((u32*)buffer,0,sizeof(buffer));
				APU_paused=0;
				resetAPU();
			break;
			case FIFO_SOUND_RESET:
				lidinterrupt();
			break;
		
			//arm9 wants to WifiSync
			case(WIFI_SYNC):{
				Wifi_Sync();
			}
			break;
			
			
			
			//arm9 wants to send a WIFI context block address / userdata is always zero here
			case(0xc1710101):{
				//	wifiAddressHandler( void * address, void * userdata )
				wifiAddressHandler((Wifi_MainStruct *)(u32)cmd3, 0);
			}
			break;
			#endif
			
			//ARM9 command handler
			#ifdef ARM9
			case(WIFI_SYNC):{
				Wifi_Sync();
			}
			break;
			#endif
		}
		MyIPC->fiforeply = (uint32)fifo_requires_ack_execok;
	}
	
	else if(cmd1 == fifo_writeuint8_ack){
		
		//invalidate cache here (arm9)
		#ifdef ARM9
		//Prevent Cache problems.
		DC_FlushRange((u32*)cmd2, (int)4);
		#endif
		
		//data0 address / data1 value *uint32)
		*(uint8*)cmd2 = (uint8)cmd3;
		
		MyIPC->fiforeply = (uint32)*(uint8*)cmd2;	//works with cmd2 passed directly
	}
	
	else if(cmd1 == fifo_writeuint16_ack){
		
		//invalidate cache here (arm9)
		#ifdef ARM9
		//Prevent Cache problems.
		DC_FlushRange((u32*)cmd2, (int)4);
		#endif
		
		//data0 address / data1 value *uint32)
		*(uint16*)cmd2 = (uint16)cmd3;
		
		MyIPC->fiforeply = (uint32)*(uint16*)cmd2;	//works with cmd2 passed directly
	}
	
	else if(cmd1 == fifo_writeuint32_ack){
		
		//invalidate cache here (arm9)
		#ifdef ARM9
		//Prevent Cache problems.
		DC_FlushRange((u32*)cmd2, (int)4);
		#endif
		
		//data0 address / data1 value *uint32)
		*(uint32*)cmd2 = (uint32)cmd3;
		
		MyIPC->fiforeply = *(uint32*)cmd2;	//works with cmd2 passed directly
	}
	
	else if(cmd1 == fifo_readuint8_ack){
		//invalidate cache here (arm9)
		#ifdef ARM9
		//Prevent Cache problems.
		DC_FlushRange((u32*)cmd2, (int)4);
		#endif
		
		MyIPC->fiforeply = *(uint8*)cmd2;	//works with cmd2 passed directly
	}
	
	else if(cmd1 == fifo_readuint16_ack){
		//invalidate cache here (arm9)
		#ifdef ARM9
		//Prevent Cache problems.
		DC_FlushRange((u32*)cmd2, (int)4);
		#endif
		
		MyIPC->fiforeply = *(uint16*)cmd2;	//works with cmd2 passed directly
	}
	
	else if(cmd1 == fifo_readuint32_ack){
		//invalidate cache here (arm9)
		#ifdef ARM9
		//Prevent Cache problems.
		DC_FlushRange((u32*)cmd2, (int)4);
		#endif
		
		MyIPC->fiforeply = *(uint32*)cmd2;	//works with cmd2 passed directly
	}
	
	//project specific
	#ifdef ARM7
	//APUSoundWrite(Uint address, Uint value);
	else if(cmd1 == fifo_apunes_write){
		APUSoundWrite(cmd2, cmd3);
	}
	#endif
	
	//ok, send empty signal
	REG_IPC_FIFO_CR |= IPC_FIFO_ERROR;
	
}


//FIFO HANDLER END





#ifdef ARM9
void apusetup(){
	MyIPC->IPC_ADDR = (u32*)ipc_region;
	MyIPC->apu_ready = true;
}
#endif



//coto: humble ipc clock opcodes
u8 gba_get_yearbytertc(){
	return (u8)(u32)MyIPC->clockdata[0];
}

u8 gba_get_monthrtc(){
	return (u8)(u32)MyIPC->clockdata[1];
}

u8 gba_get_dayrtc(){
	return (u8)(u32)MyIPC->clockdata[2];
}

u8 gba_get_dayofweekrtc(){
	return (u8)(u32)MyIPC->clockdata[3];
}


u8 gba_get_hourrtc(){
	return (u8)(u32)MyIPC->clockdata[4];
}

u8 gba_get_minrtc(){
	return (u8)(u32)MyIPC->clockdata[5];
}

u8 gba_get_secrtc(){
	return (u8)(u32)MyIPC->clockdata[6];
}
