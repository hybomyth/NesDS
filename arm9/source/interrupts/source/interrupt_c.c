#include <nds/interrupts.h>
#include <nds/system.h>
#include <nds/ipc.h>
#ifdef ARM7
#include <nds/arm7/i2c.h>
#endif

#include <stdio.h>
#include <nds/touch.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <nds.h>

#include "common_shared.h" 
#include "interrupts.h"
#include "dswifi9.h"

#include "ds_misc.h"
#include "c_defs.h"
#include "arm9main.h"
#include "multi.h"
#include "nifi.h"

#include "http_utils.h"
#include "client_http_handler.h"

//jumps here when desync
__attribute__((section(".itcm")))
void vcounter(){
	
	//stuff that need to run only once per frames
	//sendcmd();	//exception here cant
		
	//consoletext(64*2-32,"     vcount!__________________________",0);	//works
}

bool nds_vblank = false;
//---------------------------------------------------------------------------------
__attribute__((section(".itcm")))
void Vblank() {
//---------------------------------------------------------------------------------
	
	//update when idle
	if((MyIPC->dswifiSrv.dsnwifisrv_mode == dswifi_idlemode) || (MyIPC->dswifiSrv.dsnwifisrv_mode == dswifi_nifimode)){
		EMU_VBlank();
	}
	
	//only update when connected
	else if(MyIPC->dswifiSrv.dsnwifisrv_mode == dswifi_wifimode){
		if( (nifi_stat == 5) || (nifi_stat == 6) ) {
			
			//update each host/guest framecounts
			if(nifi_stat == 5){
				host_framecount 	= nesds_framecount;
			}
			
			if(nifi_stat == 6){
				guest_framecount	= nesds_framecount;
			}
			
			EMU_VBlank();
			//iprintf("vblank! \n");
			
			if(nifi_stat == 5){
			
				//NDS -> NES PPU frame counter
				if(nesds_framecount > 59) {
					nesds_framecount=0;
				}
				else{
					nesds_framecount++;
				}
				debuginfo[VBLS] = nesds_framecount;
			
			}
			
		}
	}
	
}

//---------------------------------------------------------------------------------
__attribute__((section(".itcm")))
void Hblank() {
//---------------------------------------------------------------------------------
    
	//(*__hblankhook)();
    //iprintf("hblank! \n");
    
}




/*---------------------------------------------------------------------------------
	Copyright (C) 2005
		Dave Murphy (WinterMute)
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any
	damages arising from the use of this software.
	Permission is granted to anyone to use this software for any
	purpose, including commercial applications, and to alter it and
	redistribute it freely, subject to the following restrictions:
	1.	The origin of this software must not be misrepresented; you
		must not claim that you wrote the original software. If you use
		this software in a product, an acknowledgment in the product
		documentation would be appreciated but is not required.
	2.	Altered source versions must be plainly marked as such, and
		must not be misrepresented as being the original software.
	3.	This notice may not be removed or altered from any source
		distribution.
---------------------------------------------------------------------------------*/


//we need the irq handler exposed (so other projects that use custom IRQ handler can benefit..)

//---------------------------------------------------------------------------------
void irqInitExt(IntFn handler) {
//---------------------------------------------------------------------------------
	int i;

	irqInitHandler(handler);

	// Set all interrupts to dummy functions.
	for(i = 0; i < MAX_INTERRUPTS; i ++)
	{
		irqTable[i].handler = irqDummy;
		irqTable[i].mask = 0;
	}

#ifdef ARM7
	if (isDSiMode()) {
		irqSetAUX(IRQ_I2C, i2cIRQHandler);
		irqEnableAUX(IRQ_I2C);
	}
#endif
	REG_IME = 1;			// enable global interrupt
}