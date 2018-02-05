#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "ds_misc.h"
#include "c_defs.h"
#include "videoTGDS.h"
#include "consoleTGDS.h"

extern u16 font;
extern u16 fontpal;
char cusfont[] = {
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00,
	0x55, 0x55, 0x55, 0x55,
	0x44, 0x44, 0x44, 0x44,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 
	
	0x00, 0x50, 0x04, 0x00,
	0x00, 0x50, 0x04, 0x00,
	0x00, 0x50, 0x04, 0x00,
	0x00, 0x50, 0x04, 0x00,
	0x00, 0x50, 0x04, 0x00,
	0x00, 0x50, 0x04, 0x00,
	0x00, 0x50, 0x04, 0x00,
	0x00, 0x50, 0x04, 0x00,

	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x45,
	0x00, 0x00, 0x55, 0x44,
	0x00, 0x00, 0x45, 0x00,
	0x00, 0x50, 0x04, 0x00, 
	0x00, 0x50, 0x04, 0x00, 

	0x00, 0x50, 0x04, 0x00, 
	0x00, 0x50, 0x04, 0x00, 
	0x00, 0x00, 0x45, 0x00,
	0x00, 0x00, 0x55, 0x44,
	0x00, 0x00, 0x00, 0x45,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00,

	0x00, 0x50, 0x04, 0x00, 
	0x00, 0x50, 0x04, 0x00, 
	0x00, 0x45, 0x00, 0x00,
	0x44, 0x55, 0x00, 0x00,
	0x45, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00,
	0x45, 0x00, 0x00, 0x00,
	0x44, 0x55, 0x00, 0x00,
	0x00, 0x45, 0x00, 0x00,
	0x00, 0x50, 0x04, 0x00, 
	0x00, 0x50, 0x04, 0x00, 

};

/*****************************
* name:			showconsole
* function:		show the console displayed on the screen below.
* argument:		none
* description:		prepare to show the screen. Called when you touch the screen.
******************************/
void showconsole() {
	//clearconsole();
	if(__emuflags & ALLPIXEL) {
		SETDISPCNT_SUB(MODE_0_2D);
		DISABLE_BG_SUB(3);
		dmaFillWord(3,0, (uint32)BG_GFX_SUB, 192 * 256);	//clear the sub screen
		dmaTransferHalfWord(3,&font,(u16*)(SUB_CHR+0x400),1024*4);
		dmaTransferHalfWord(3,cusfont,(u16*)(SUB_CHR+0x1400),6*32);
		dmaTransferHalfWord(3,&fontpal,(u16*)BG_PALETTE_SUB,128);
	}
	__emuflags &= ~ALLPIXEL;

	powerON(POWER_2D_B);
	powerON(PM_BACKLIGHT_BOTTOM | PM_BACKLIGHT_TOP);
	SET_MAIN_TOP_LCD();
	screen_swap = 0;
	REG_DISPCNT_SUB=MODE_0_2D|DISPLAY_BG0_ACTIVE|DISPLAY_BG1_ACTIVE|DISPLAY_WIN0_ON;
	REG_BG0CNT_SUB=BG_TILE_BASE(1)|BG_64x32;
	REG_BG1CNT_SUB=BG_TILE_BASE(1)|BG_64x32;
	REG_BG0HOFS_SUB=0;
	REG_BG0VOFS_SUB=0;
	REG_BG1HOFS_SUB=0;
	REG_BG1VOFS_SUB=0;

	REG_BG3Y_SUB = 0;

	SUB_WIN0_X0=0;
	SUB_WIN0_X1=255;
	SUB_WIN0_Y0=24;
	SUB_WIN0_Y1=191;
	SUB_WIN_IN=1;
	SUB_WIN_OUT=2;

	IRQVBlankWait();
}

/*****************************
* name:			hideconsole
* function:		hide the screen below to save power.
* argument:		none
* description:		hide the screen. Called when you click 'exit'.
******************************/
void hideconsole() {
	IRQVBlankWait();
	//powerOFF(POWER_2D_B);
	if((!(__emuflags & ALLPIXELON))) {
		__emuflags &= ~ALLPIXEL;
		if(!(__emuflags & SCREENSWAP)) {
			powerOFF(PM_BACKLIGHT_BOTTOM);			//This cannot be accessed directly.
			powerON(PM_BACKLIGHT_TOP);
			SET_MAIN_TOP_LCD();
		} else {
			powerON(PM_BACKLIGHT_BOTTOM);
			powerOFF(PM_BACKLIGHT_TOP);
			SET_MAIN_BOTTOM_LCD();
		}
	} else {
		int pos = ad_ypos / (1 << 16);

		__emuflags |= ALLPIXEL;
		dmaFillWord(3,0, BG_GFX_SUB , 192 * 256);		//clear the sub screen
		dmaTransferWord(3,(uint32)BG_PALETTE, (uint32)BG_PALETTE_SUB, 0x400);		//copy the palette
		powerON(PM_BACKLIGHT_BOTTOM);
		powerON(PM_BACKLIGHT_TOP);

		if(pos < -(240 - 192)/2) {
			__emuflags |= SCREENSWAP;
			all_pix_start = 0;
			all_pix_end = 1 - pos;
			SET_MAIN_BOTTOM_LCD();
		} else {
			__emuflags &= ~SCREENSWAP;
			all_pix_start = 192 - pos;
			if(all_pix_start > 239)
				all_pix_start = 239;
			all_pix_end = 240;
			SET_MAIN_TOP_LCD();
		}
		SETDISPCNT_SUB(MODE_5_2D);
		ENABLE_BG_SUB(3);
		BGCTRL_SUB[3] = (typeof(BGCTRL_SUB[3]))(BgSize_B8_256x256 | BG_MAP_BASE(0) | BG_TILE_BASE(0));
		IRQVBlankWait();
		SETDISPCNT_SUB(MODE_5_2D);
		ENABLE_BG_SUB(3);
		BGCTRL_SUB[3] = (typeof(BGCTRL_SUB[3]))(BgSize_B8_256x256  | BG_MAP_BASE(0) | BG_TILE_BASE(0));
		rescale(ad_scale, ad_ypos);
	}
}

/*****************************
* name:			clearconsole
* function:		clear the screen below.
* argument:		none
* description:		none
******************************/
void clearconsole() {
	memset((void *)(SUB_BG+64*4),0,64*20);
	REG_BG0VOFS=0;
}

/*****************************
* name:			consoleinit
* function:		init the screen below to show text.
* argument:		none
* description:		none
******************************/
void consoleinit() {
	dmaTransferHalfWord(3,(uint32)&font,(uint32)(SUB_CHR+0x400),1024*4);
	dmaTransferHalfWord(3,(uint32)cusfont,(uint32)(SUB_CHR+0x1400),6*32);
	dmaTransferHalfWord(3,(uint32)&fontpal,(uint32)BG_PALETTE_SUB,128);
	showconsole();
}

/*****************************
* name:			menutext
* function:		to show one line in a menu, selected or not, file or dir.
* argument:		line: where the text(just a line) located.
			s: chars to display on the line.
			selected: the line type.
				0 for not selected
				1 for selected
				2 for dir selected
* description:		none
******************************/
void menutext(int line,char *s,int selected) {
	int color=selected?0x1000:0;
	u16 *p=(u16*)(SUB_BG+line*64);
	u16 *p2=p+32;
	u16 c = 0;
	
	if(!selected) c=' ';
	else if(selected==1) c='*'|0x1000;	//normal selected look
	else if(selected==2) c='<'|0x1000;	//directory selected
	*p++=c;
	
	while(*s && p<p2) {
		*p++ = (*s++)|color;
	}
	if(selected==2)
		*p++ = '>'|color;
	while(p<p2)
		*p++=' ';
}

/*****************************
* name:			consoletext
* function:		to show one line.
* argument:		offset: offset to the left-top. One line(row) for 64.
			s: chars to display on the line.
			color: difine the color
* description:		none
******************************/
void consoletext(int offset,char *s,int color) {
	u16 *p;
	p=(u16*)(SUB_BG+offset);
	while(*s) {
		if(*s == '\n') {
			p = (u16 *)(((int)p + 62)&(~63));
			s++;
		}
		else if(*s == '\t') {
			p = (u16 *)(((int)p + 14)&(~15));
			s++;
		}
		else if(*s == '\r') {
			p = (u16 *)((((int)p)&~63) + (offset&63) + 64);
			s++;
		}
		if(!*s)
			break;
		*p++ = (*s++)|color;
	}
}

/*****************************
* name:			hex
* function:		to show a hex number.
* argument:		offset: offset to the left-top. One line(row) for 64.
			d: the number.
			n: amount of characters
* description:		none
******************************/
void hex(int offset,int d,int n) {
	u16 *p;
	u16 c;
	p=(u16*)(SUB_BG+offset);
	do {
		c=d&0x0f;
		if(c<10) c+='0';
		else c=c-10+'A';
		d>>=4;
		p[n--]=c;
	} while(n>=0);
}
