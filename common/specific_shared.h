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

#define FIFO_SETUP_SPECIFIC (uint32)(0xffff3010)

//inherits what is defined in: ipcfifoTGDS.h
#ifndef __specific_shared_h__
#define __specific_shared_h__

#include "dsregs.h"
#include "dsregs_asm.h"
#include "ipcfifoTGDS.h"
#include "memoryHandleTGDS.h"

struct sIPCSharedTGDSSpecific{
	uint32 * IPC_ADDR;
    uint8 * ROM;   		//pointer to ROM page
    int rom_size;   	//rom total size
	
	//APU Sync
	u8 apu_busy;
	
	//APU setup
	bool apu_ready;	//true yes / false no
	uint32 fiforeply;	//for ret status
};

// Project Specific

//GDB stub support
//#define GDB_ENABLE

//NESDS special
#define FIFO_APU_PAUSE 0x1201
#define FIFO_UNPAUSE 0x1202
#define FIFO_APU_RESET 0x1204
#define FIFO_SOUND_RESET 0x1206
#define FIFO_APU_BUSY 1
#define fifo_nesapuwrite_ext	(uint32)(0xffff1019)

#endif


#ifdef __cplusplus
extern "C" {
#endif

//NOT weak symbols : the implementation of these is project-defined (here)
extern void HandleFifoNotEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4);
extern void HandleFifoEmptyWeakRef(uint32 cmd1,uint32 cmd2,uint32 cmd3,uint32 cmd4);

//project specific
extern struct sIPCSharedTGDSSpecific* getsIPCSharedTGDSSpecific();
extern void apusetup();

extern bool getAPUStatus();
extern void setAPUStatus(bool status);

#ifdef __cplusplus
}
#endif