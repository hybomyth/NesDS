/*---------------------------------------------------------------------------------
	derived from the default ARM7 core
---------------------------------------------------------------------------------*/

#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "ds_misc.h"
#include "c_defs.h"

#include "arm7.h"
#include "interrupts.h"
#include "specific_shared.h"

void nesmain();

volatile s16 buffer[MIXBUFSIZE*20];

volatile bool exitflag = false;

u8 *bootstub;
u32 ndstype;
typedef void (*type_void)();
type_void bootstub_arm7;
static void sys_exit(){
	/*
	if(!bootstub_arm7){
		if(ndstype>=2)writePowerManagement(0x10, 1);
		else writePowerManagement(0, PM_SYSTEM_PWR);
	}
	*/
	bootstub_arm7(); //won't return
}

extern int APU_paused;
extern void dealrawpcm(unsigned char *out);

//ARM9 maps this ipc_region
int ipc_region = 0;

/*
//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	readUserSettings();
	
    int i   = 0;
	REG_IME = 0;

	// Reset the clock if needed
    rtcReset();

    interrupts_to_wait_arm7 = IRQ_TIMER1 | IRQ_HBLANK | IRQ_VBLANK | IRQ_VCOUNT | IRQ_FIFO_NOT_EMPTY;    
	
    //fifo setups
    irqInitExt(IntrMainExt);
	fifoInit();
    
	installWifiFIFO();
	
    irqSet(IRQ_HBLANK,hblank);
	irqSet(IRQ_VBLANK, vblank);
	irqSet(IRQ_VCOUNT,vcounter);
    irqSet(IRQ_TIMER1, soundinterrupt);
    
	irqSet(IRQ_FIFO_NOT_EMPTY,HandleFifoNotEmpty);
	irqEnable(interrupts_to_wait_arm7);
    
    REG_IPC_SYNC = 0;
    REG_IPC_FIFO_CR = IPC_FIFO_RECV_IRQ | IPC_FIFO_ENABLE;			//10    R/W  Receive Fifo Not Empty IRQ  (0=Disable, 1=Enable)
	
    //set up ppu: do irq on hblank/vblank/vcount/and vcount line is 159
    REG_DISPSTAT = REG_DISPSTAT | DISP_HBLANK_IRQ | DISP_VBLANK_IRQ | DISP_YTRIGGER_IRQ | (VCOUNT_LINE_INTERRUPT << 15);
    
	{
		ndstype=0;
		u32 myself = readPowerManagement(4); //(PM_BACKLIGHT_LEVEL);
		if(myself & (1<<6))
			ndstype=(myself==readPowerManagement(5))?1:2;
	}

	bootstub=(u8*)0x02ff4000;
	bootstub_arm7=(*(u64*)bootstub==0x62757473746F6F62ULL)?(*(type_void*)(bootstub+0x0c)):0;
	setPowerButtonCB(sys_exit); 
	
	REG_IF = ~0;
    REG_IME = 1;
	
	//wait for APU to be setup
	while(MyIPC->apu_ready != true){}
	ipc_region = (int)(u32*)MyIPC->IPC_ADDR;
	
	nesmain();
	
	while(1){
		IRQVBlankWait();
	}
}
*/


//tgds
//---------------------------------------------------------------------------------
int main() {
//---------------------------------------------------------------------------------
	IRQInit();
	installWifiFIFO();		//use DSWIFI
	
    int i   = 0;
	//REG_IME = 0;

    //interrupts_to_wait_arm7 = IRQ_TIMER1 | IRQ_HBLANK | IRQ_VBLANK | IRQ_VCOUNT | IRQ_FIFO_NOT_EMPTY;    
	
    //override these in TGDS user handlers
    //irqSet(IRQ_HBLANK,hblank);	//empty user handler
	//irqSet(IRQ_VBLANK, vblank);	//empty user handler
	//irqSet(IRQ_VCOUNT,vcounter);	//empty user handler
    //irqSet(IRQ_TIMER1, soundinterrupt);	//timer1 userhandler
	//irqSet(IRQ_FIFO_NOT_EMPTY,HandleFifoNotEmpty);
	
	//irqEnable(interrupts_to_wait_arm7);
    //REG_IPC_SYNC = 0;
    //REG_IPC_FIFO_CR = IPC_FIFO_RECV_IRQ | IPC_FIFO_ENABLE;			//10    R/W  Receive Fifo Not Empty IRQ  (0=Disable, 1=Enable)
	
    //set up ppu: do irq on hblank/vblank/vcount/and vcount line is 159
    //REG_DISPSTAT = REG_DISPSTAT | DISP_HBLANK_IRQ | DISP_VBLANK_IRQ | DISP_YTRIGGER_IRQ | (VCOUNT_LINE_INTERRUPT << 15);
    /*
	{
		ndstype=0;
		u32 myself = readPowerManagement(4); //(PM_BACKLIGHT_LEVEL);
		if(myself & (1<<6))
			ndstype=(myself==readPowerManagement(5))?1:2;
	}
	*/
	//bootstub=(u8*)0x02ff4000;
	//bootstub_arm7=(*(u64*)bootstub==0x62757473746F6F62ULL)?(*(type_void*)(bootstub+0x0c)):0;
	//setPowerButtonCB(sys_exit); 
	
	//REG_IF = ~0;
    //REG_IME = 1;
	
	//wait for APU to be setup
	setAPUStatus(false);
	while(getAPUStatus() == false){}
	
	//ok
	//while(getAPUStatus() == true){
	//	SendMultipleWordACK(DEBUG7, (uint32)getAPUAddress(), 0, NULL);
	//}
	
	ipc_region = (int)(u32*)getsIPCSharedTGDSSpecific()->IPC_ADDR;
	nesmain();
	
	while(1){
		IRQVBlankWait();
	}
}
