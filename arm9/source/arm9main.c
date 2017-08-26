#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "ds_misc.h"
#include "c_defs.h"

#include "arm9main.h"
#include "multi.h"
#include "dswifi9.h"
#include "nifi.h"
#include "wifi_shared.h"
#include "wifi_arm9.h"
#include "utils.h"

#include "interrupts.h"
#include "touch_ipc.h"
#include "common_shared.h"

#include "http_utils.h"
#include "client_http_handler.h"

//frameskip min = 1, max = xxxxxx....
int soft_frameskip = 3;

#define SOFT_FRAMESKIP soft_frameskip

#ifdef ROM_EMBEDED
extern u8 __bss_end__[];
extern void do_romebd();
extern char romebd_s[];
void do_romebd()
{
	freemem_start = ((u32)(&__bss_end__) + 3) & ~3;
	freemem_end = 0x23D8000;
	initcart(romebd_s);
}	
#endif

void showversion()
{
	memset((void *)(SUB_BG),0,64*3);
	//consoletext(64*2-32,"     nesDS 1.3d ________________________________",0);
	//char buf[16];
	//sprintf(buf, "framecount %d ", nesds_framecount);
	//consoletext(64*2-32,buf,0);
}

/*****************************
* name:			vblankinterrupt
* function:		to provide a 60fps interruption. It is the most simple way and can save power.
* argument:		none
* description:	NTSC for 60fps, PAL for 50fps. Call this function to sync with NES emulation.
******************************/
/*
void vblankinterrupt() {
	debuginfo[6]++;
	EMU_VBlank();
	irqDisable(IRQ_VCOUNT);			//we should disable this...
}
*/

void aliveinterrupt(u32 msg, void *none)
{
	IPC_ALIVE = msg;		//if arm7 still alive, a non-zero value will be received.
}

/*****************************
* name:			hblankinterrupt
* function:		a function to deal with hblank interrupts.
* argument:		none
* description:	for emulation
******************************/
//extern void  (*__hblankhook)(void);
extern void hblankinterrupt();			//something wrong... I dont know why...
/*
{
	(*__hblankhook)();
}*/

/*****************************
* name:			DS_init
* function:		a function to init DS env.
* argument:		none
* description:	none
******************************/
void DS_init() {
/*
  VRAM    SIZE  MST  OFS   ARM9, Plain ARM9-CPU Access (so-called LCDC mode)
  A       128K  2    0     6060000-607FFFFh MAIN BG
  B       128K  1    2     6040000-605FFFFh MAIN BG
  C       128K  1    0     6000000-601FFFFh MAIN BG
  D       128K  1    1     6020000-603FFFFh MAIN BG

  E       64K   0    0     6400000-641FFFFh MAIN OBJ
  F       (not used)
  G       (not used)
  H       32K   3    0     6200000-6207FFFFh(not enabled)

  I       16K   1    0     6208000-62087FFFh
*/
	VRAM_ABCD=0x89819199;			//see notes.txt for VRAM layout
	VRAM_EFG=0x03000082;
	VRAM_HI=0x0081;
}

/*****************************
* name:			EMU_Init
* function:		a function to init DS emulation.
* argument:		none
* description:	none
******************************/
void EMU_Init() {
	PPU_init();
	rescale(0xe000,-0x00060000);
	REG_DISPCNT=0x38810000;
}

/*****************************
* name:			arm9main
* function:		program starts form here
* argument:		none
* description:	none
******************************/
int global_playcount = 0;				//used for NTSC/PAL
int subscreen_stat=-1;				//short-cuts will change its value.
int argc;
char **argv;


u32 interrupts_to_wait_arm9 = 0;
__attribute__((section(".dtcm")))
int nesds_framecount=0;	//global framecount emulator

//new irqs
int main(int _argc, char **_argv) {
	
	int sramcount=0;

	argc=_argc, argv=_argv;
	defaultExceptionHandler();
	
	//fifo setups
    irqInitExt(IntrMainExt);
	
	irqSet(IRQ_VBLANK, Vblank);
	irqSet(IRQ_HBLANK,Hblank);
	irqSet(IRQ_VCOUNT,vcounter);
	irqSet(IRQ_FIFO_NOT_EMPTY,HandleFifoNotEmpty);
    
	interrupts_to_wait_arm9 = IRQ_HBLANK|IRQ_VBLANK|IRQ_VCOUNT| IRQ_FIFO_NOT_EMPTY;
	irqEnable(interrupts_to_wait_arm9);
	
	REG_IPC_SYNC = 0;
    REG_IPC_FIFO_CR = IPC_FIFO_RECV_IRQ | IPC_FIFO_ENABLE;	//10    R/W  Receive Fifo Not Empty IRQ  (0=Disable, 1=Enable)
    
    //set up ppu: do irq on hblank/vblank/vcount/and vcount line is 159
    REG_DISPSTAT = REG_DISPSTAT | DISP_HBLANK_IRQ | DISP_VBLANK_IRQ | DISP_YTRIGGER_IRQ | (VCOUNT_LINE_INTERRUPT << 15);
    
	DS_init(); //DS init.
#ifndef ROM_EMBEDED
	active_interface = fatInitDefault(); //init file operations to your external card.
	
#endif
	EMU_Init(); //emulation init.
	apusetup();
	
	IPC_ALIVE = 0;
	IPC_APUIRQ = 0;
	IPC_REG4015 = 0;

	consoleinit(); //init subscreen to show chars.
	crcinit();	//init the crc table.

	//pre-alocate memory....
	//IPC_FILES = malloc(MAXFILES * 256 + MAXFILES * 4);
	//IPC_ROM = malloc(ROM_MAX_SIZE);

#ifndef ROM_EMBEDED
	if(!bootext()) {
		//chdir("/");
		do_rommenu(); //show a menu selecting rom files.
	}
#else
	do_romebd();
#endif
	
	//__emuflags |= PALSYNC;
	
	//idle: default startup
	switch_dswnifi_mode((u8)dswifi_idlemode);
	//switch_dswnifi_mode((u8)dswifi_wifimode);
	
	//test case #1: write external uint32 ..ok
	/*
	volatile uint32 buf;
	volatile char writebuf[256];
	if(Writeuint32WordACK((uint32)buf, (uint32)0xc070c171) == true){
		sprintf((char*)writebuf,"     write ok:%x",(unsigned int)MyIPC->fiforeply);
		consoletext(64*2-32,(char*)writebuf,0);
	}
	else{
		sprintf((char*)writebuf,"     write phail:%x",(unsigned int)MyIPC->fiforeply);
		consoletext(64*2-32,(char*)writebuf,0);
	}
	
	while(1){};
	*/
	
	//test case #2: write external uint16 ..ok
	/*
	volatile uint32 buf;
	volatile char writebuf[256];
	if(Writeuint16WordACK((uint32)buf, (uint16)0xc070) == true){
		sprintf((char*)writebuf,"     write ok:%x",(unsigned int)MyIPC->fiforeply);
		consoletext(64*2-32,(char*)writebuf,0);
	}
	else{
		sprintf((char*)writebuf,"     write phail:%x",(unsigned int)MyIPC->fiforeply);
		consoletext(64*2-32,(char*)writebuf,0);
	}
	
	while(1){};
	*/
	
	//test case #3: write external uint8 ..ok
	/*
	volatile uint32 buf;
	volatile char writebuf[256];
	if(Writeuint8WordACK((uint32)buf, (uint8)0x71) == true){
		sprintf((char*)writebuf,"     write ok:%x",(unsigned int)MyIPC->fiforeply);
		consoletext(64*2-32,(char*)writebuf,0);
	}
	else{
		sprintf((char*)writebuf,"     write phail:%x",(unsigned int)MyIPC->fiforeply);
		consoletext(64*2-32,(char*)writebuf,0);
	}
	
	while(1){};
	*/
	//test case #4: read external uint8 ..ok
	/*
	volatile char writebuf[256];
	sprintf((char*)writebuf,"     read8:%x",(unsigned int)Readuint8WordACK(0x037f8000));
	consoletext(64*2-32,(char*)writebuf,0);
	while(1);
	*/
	
	//test case #5: read external uint16 ..ok
	/*
	volatile char writebuf[256];
	sprintf((char*)writebuf,"     read16:%x",(unsigned int)Readuint16WordACK(0x037f8000));
	consoletext(64*2-32,(char*)writebuf,0);
	while(1);
	*/
	
	//test case #6: read external uint32 ..ok
	/*
	volatile char writebuf[256];
	sprintf((char*)writebuf,"     read32:%x",(unsigned int)Readuint32WordACK(0x037f8000));
	consoletext(64*2-32,(char*)writebuf,0);
	while(1);
	*/
	
	//test case : WriteAPUNESACK
	/*
	bool res = SendMultipleWordACK(fifo_nesapuwrite_ext,(u32)(0),(u32)(0),(u32)(0));
	volatile uint32 buf;
	volatile char writebuf[256];
	if(res == true){
		sprintf((char*)writebuf,"WriteAPUNESACK:OK:%x",(unsigned int)MyIPC->fiforeply);
		consoletext(64*2-32,(char*)writebuf,0);
	}
	else{
		sprintf((char*)writebuf,"WriteAPUNESACK:ERROR:%x",(unsigned int)MyIPC->fiforeply);
		consoletext(64*2-32,(char*)writebuf,0);
	}
	
	while(1){}
	*/
	/*
	//test case : SendMultipleWordByFifo
	volatile char writebuf[256];
	uint32 reply = SendMultipleWordByFifo((uint32)fifo_writeuint32_ack, (uint32)fifo_apunes_write, (uint32)0x4000, (uint32)0x8, (uint32)0);
	sprintf((char*)writebuf,"SendMultipleWordByFifo:%x",(unsigned int)reply);
	consoletext(64*2-32,(char*)writebuf,0);
	while(1){}
	*/
	
	while(1) { // main loop to do the emulation
		
		if(__emuflags & PALTIMING && global_playcount == 0) {
			nesds_framecount--;
		}
		
		if(debuginfo[VBLS]>59) {
			debuginfo[VBLS]=nesds_framecount;
			debuginfo[1] = debuginfo[0];
			debuginfo[0] = 0;
			debuginfo[FPS]=nesds_framecount;	
		}
		
		//scanKeys();
		IPC_KEYS = keyscurr_ipc();

		//change nsf states
		if(__emuflags & NSFFILE) {
			static int oldkey = 0;
			int keydown = IPC_KEYS & (~oldkey);
			oldkey = IPC_KEYS;

			if(keydown & KEY_LEFT) {
				if(__nsfsongno == 0) {
					__nsfsongno = nsfheader.TotalSong-1;
				} else {
					__nsfsongno--;
				}
			}
			if(keydown & KEY_RIGHT) {
				if(++__nsfsongno > nsfheader.TotalSong-1) {
					__nsfsongno = 0;
				}
			}		
			if(keydown & KEY_UP) {
				__nsfplay = 1;
				__nsfinit = 1;
			}		
			if(keydown & KEY_DOWN) {
				__nsfplay = 0;
				Sound_reset();
			}
		}
			
		do_shortcuts();
		if((__emuflags & AUTOSRAM)) {
			if(__emuflags & NEEDSRAM) {
				sramcount = 1;
				__emuflags&=~NEEDSRAM;
			}
			if(sramcount > 0)
				sramcount++;
			if(sramcount > 120) {		//need auto save for sram.
				sramcount = 0;
				save_sram();
			}
		}

		touch_update(); // do menu functions.
		do_menu();	//do control menu.
		bool dswnifi_frame = do_multi();	//add multi dswifi support
		
		
		//single player (for idle, nifi)
		if((MyIPC->dswifiSrv.dsnwifisrv_mode == dswifi_idlemode) || (MyIPC->dswifiSrv.dsnwifisrv_mode == dswifi_nifimode)){
			if(nifi_stat == 0){
				play(); //emulate a frame of the NES game.
			}
		}
		
		//multi player	(for nifi)
		if(MyIPC->dswifiSrv.dsnwifisrv_mode == dswifi_nifimode){
			
			if((nifi_stat == 5) || (nifi_stat == 6)){
				
				if(getintdiff(host_framecount,guest_framecount) > 0){
					int diff_framecount = getintdiff(host_framecount,guest_framecount);
					int top = topvalue(nesds_framecount,diff_framecount);
					int bottom = bottomvalue(nesds_framecount,diff_framecount);
					//host: read only 	guest_framecount
					if(nifi_stat == 5){
						nesds_framecount = top - bottom;
					}
					//guest: read only 	host_framecount
					if(nifi_stat == 6){
						nesds_framecount = top - bottom;
					}
				}
				
				//nds sync ppus
				int framediff = getintdiff(host_vcount,guest_vcount);
				int calc_vcount = 0;
				if(framediff > 0){
					//#1 sync LY VCOUNT
					int top2 = topvalue(host_vcount,framediff);
					int bottom2 = bottomvalue(host_vcount,framediff);	
					calc_vcount = top2 - bottom2;
				}
				
				int nds_vc = (REG_VCOUNT&0xff);
				if((nds_vc >= 202 ) && (nds_vc <= 212) ){
					if(framediff > 0){
						//update only if we have a valid nifi frame
						REG_VCOUNT = calc_vcount;
					}
					swiIntrWait(1,IRQ_VCOUNT);
				}
				
				nifi_keys =  nifi_keys_sync;
				play();
				
			}
		}
		
		//multi player	(for wifi)
		else if (MyIPC->dswifiSrv.dsnwifisrv_mode == dswifi_wifimode){
			if(dswnifi_frame == true){
				nifi_keys =  nifi_keys_sync;
				play();
			}
			//just for debugging WIFI activity, leave uncommented for wifi gameplay
			/*
			else{
				nifi_stat = 0;
				switch_dswnifi_mode((u8)dswifi_idlemode);
			}
			//debug stability when connected in WIFI mode
			uint8 val = rand()&0xf;
			volatile char buf[128];
			if(dswnifi_frame == true){
				sprintf((char*)buf,"frameok:%d! ________________________________",val);
				consoletext(64*2-32,(char*)buf,0);
			}
			else{
				sprintf((char*)buf,"framephail:%d! ________________________________",val);
				consoletext(64*2-32,(char*)buf,0);
				
				
			}
			*/
		}
		
		swiWaitForVBlank();
		
	}
}

int lastsave;
int firstsave;
int maxsaves;

/*****************************
* name:			recorder_reset
* function:		to set the status of savestate.
* argument:		none
* description:	none
******************************/
void recorder_reset() {
	maxsaves=(freemem_end-freemem_start)/SAVESTATESIZE; //cal the frames that you can save..
	lastsave=0; //2 pointers to use the memory of savestate as a FIFO.
	firstsave=0;
}

//run 1 NES frame with FF/REW control
void play() {
  	static int framecount=0;
	static int fcount = 0;
	int forward = 0;
	int backward = 0;

	do_cheat();

	global_playcount++;
	if(global_playcount > 6)
		global_playcount = 0;

	if(nifi_stat){
		__emuflags &= ~(FASTFORWARD | REWIND);		//when nifi enabled, disable the fastforward & rewind.
	}
	
	forward = __emuflags & FASTFORWARD;
	backward = __emuflags & REWIND;
	
	if(backward) { // for rolling back... a nice function?
		//swiWaitForVBlank();
		framecount++;
		if(framecount>2) {
			framecount-=3;
			if(firstsave!=lastsave) {
				lastsave--;
				if(lastsave<0)
					lastsave=maxsaves-1;
				loadstate(freemem_start+SAVESTATESIZE*lastsave);
				EMU_Run();
			}
		}
	} else {
		if(__emuflags & SOFTRENDER) {
			if(!(forward) && (fcount >= debuginfo[6] && fcount - debuginfo[6] < 10) ){ // disable VBlank to speed up emulation.
				//swiWaitForVBlank();
			}
		} else {
			if(!(forward)) {
				if(__emuflags & PALSYNC) {
					if(__emuflags & (SOFTRENDER | PALTIMING))
						__emuflags ^= PALSYNC;
					if(REG_VCOUNT < 190) {
						//swiWaitForVBlank();
					}
				}
				else {
					if((!(__emuflags & ALLPIXEL)) || (all_pix_start != 0)){
						//swiWaitForVBlank();
					}
				}
			}
		}

		if(!(__emuflags & PALTIMING && global_playcount == 6)) {
			EMU_Run(); //run a frame
			framecount++;
			if(framecount>8) {	//save state every 9th frame
				framecount-=9;
				savestate(freemem_start+SAVESTATESIZE*lastsave);
				lastsave++;
				if(lastsave>=maxsaves)
					lastsave=0;
				if(lastsave==firstsave) {
					firstsave++;
					if(firstsave>=maxsaves)
						firstsave=0;
				}
			}
		}
		else {
			if((__emuflags & PALTIMING) && (__emuflags & ALLPIXEL) && !(__emuflags & SOFTRENDER)){
				//swiWaitForVBlank();
			}
		}
	}
	
	if(__emuflags & SOFTRENDER) {
		__emuflags &= ~AUTOSRAM;
		__rendercount++;
		if(SOFT_FRAMESKIP <= 1 ||__rendercount == 1) {
			if(__emuflags & ALLPIXEL)
				render_sub();
			render_all();
		}
		if(!(forward) && __rendercount >= SOFT_FRAMESKIP)
			__rendercount = 0;
		if((forward) && __rendercount > 16)
			__rendercount = 0;
	} else if(__emuflags & ALLPIXEL) {
		render_sub();
	}

	fcount++;
	if(fcount > 59)
		fcount = 0;

	__emuflags &= ~(FASTFORWARD | REWIND);
}
