#include "InterruptsARMCores_h.h"
#include "specific_shared.h"
#include "guiTGDS.h"
#include "dsregs_asm.h"
#include "main.h"
#include "keypadTGDS.h"
#include "interrupts.h"
#include "dswnifi_lib.h"
#include "ds_misc.h"
#include "c_defs.h"

//User Handler Definitions
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer0handlerUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer1handlerUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer2handlerUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Timer3handlerUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HblankUser(){
    //(*__hblankhook)();
    //iprintf("hblank! \n");
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void VblankUser(){
	
	//update when idle
	if((getMULTIMode() == dswifi_idlemode) || (getMULTIMode() == dswifi_localnifimode)){
		EMU_VBlank();		
	}
	//only update screen when connected
	else if(getMULTIMode() == dswifi_udpnifimode){
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
		}
	}
	
	//NDS -> NES PPU frame counter
	if(nesds_framecount > 59) {
		nesds_framecount=0;
	}
	else{
		nesds_framecount++;
	}
	debuginfo[VBLS] = nesds_framecount;
	
	//printf("vblank! \n");	
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void VcounterUser(){
}
