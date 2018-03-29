/*
			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA
*/

//TGDS IPC Version: 1.3
#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "ipcfifoTGDS.h"
#include "specific_shared.h"


#ifdef ARM7
#include <string.h>
#include "main.h"
#include "arm7.h"
#include "audiosys.h"
#include "s_apu.h"
#include "wifi_arm7.h"
#include "spifwTGDS.h"
#include "c_defs.h"

#endif

#ifdef ARM9
#include <stdbool.h>
#include "dswnifi_lib.h"
#include "main.h"
#include "InterruptsARMCores_h.h"
#include "wifi_arm9.h"
#include "c_defs.h"
#endif

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
struct sIPCSharedTGDSSpecific* getsIPCSharedTGDSSpecific(){
	struct sIPCSharedTGDSSpecific* sIPCSharedTGDSSpecificInst = (__attribute__((packed)) struct sIPCSharedTGDSSpecific*)(getUserIPCAddress());
	return sIPCSharedTGDSSpecificInst;
}

//inherits what is defined in: common_shared.c
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoNotEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4){
	
	switch (cmd1) {
		//ARM7 project specific command handler
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
		//update apu from nds irq
		case(fifo_nesapuwrite_ext):{
			//send each chunk separately 
			#ifdef ARM7
			//APUSoundWrite(cmd2&0xffff, cmd3&0xffff);
			//IPC_APUR = IPC_APUW = (int)cmd4;
			#endif
		}
		break;
		#endif
		//ARM9 project specific command handler
		#ifdef ARM9
		#endif
	}
	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4){
}


bool getAPUStatus(){
	return (bool)getsIPCSharedTGDSSpecific()->apu_ready;
}
void setAPUStatus(bool status){
	getsIPCSharedTGDSSpecific()->apu_ready = status;
}

//Project Specific
#ifdef ARM9
void apusetup(){
	getsIPCSharedTGDSSpecific()->IPC_ADDR = (u32*)ipc_region;
	setAPUStatus(true);
}
#endif
