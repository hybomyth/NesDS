#include "specific_shared.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "InterruptsARMCores_h.h"
#include "wifi_arm7.h"
#include "main.h"

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
	soundinterrupt();
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
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void VblankUser(){
}

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void VcounterUser(){
}
