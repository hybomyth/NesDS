#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include <string.h>
#include "c_defs.h"
#include "ds_misc.h"

#include "audiosys.h"
#include "handler.h"
#include "arm7.h"
#include "specific_shared.h"

#include "timerTGDS.h"
#include "CPUARMTGDS.h"
#include "audiosys.h"

static int chan = 0;

static short soundconvert(short output, int sft)
{
	if(output >= 0) {
		output = logtable[output << sft];
	} else {
		output = -logtable[(-output) << sft];
	}
	return output << 5;
}
	
void restartsound(int ch) {
	chan = ch;

	SCHANNEL_CR(0)=SCHANNEL_ENABLE|SOUND_REPEAT |SOUND_VOL(0x3F)|SOUND_PAN(0x40)|SOUND_FORMAT_16BIT;
	SCHANNEL_CR(1)=SCHANNEL_ENABLE|SOUND_REPEAT |SOUND_VOL(0x3F)|SOUND_PAN(0x40)|SOUND_FORMAT_16BIT;
	SCHANNEL_CR(2)=SCHANNEL_ENABLE|SOUND_REPEAT |SOUND_VOL(0x3F)|SOUND_PAN(0x40)|SOUND_FORMAT_16BIT;
	SCHANNEL_CR(3)=SCHANNEL_ENABLE|SOUND_REPEAT |SOUND_VOL(0x2F)|SOUND_PAN(0x40)|SOUND_FORMAT_16BIT;
	SCHANNEL_CR(4)=SCHANNEL_ENABLE|SOUND_REPEAT |SOUND_VOL(0x7F)|SOUND_PAN(0x40)|SOUND_FORMAT_16BIT;
	SCHANNEL_CR(5)=SCHANNEL_ENABLE|SOUND_REPEAT |SOUND_VOL(0x60)|SOUND_PAN(0x40)|SOUND_FORMAT_16BIT;
	SCHANNEL_CR(6)=SCHANNEL_ENABLE|SOUND_REPEAT |SOUND_VOL(0x7F)|SOUND_PAN(0x40)|SOUND_FORMAT_16BIT;
	SCHANNEL_CR(7)=SCHANNEL_ENABLE|SOUND_REPEAT |SOUND_VOL(0x7F)|SOUND_PAN(0x40)|SOUND_FORMAT_16BIT;
	SCHANNEL_CR(8)=SCHANNEL_ENABLE|SOUND_REPEAT |SOUND_VOL(0x7F)|SOUND_PAN(0x40)|SOUND_FORMAT_16BIT;

	SCHANNEL_CR(10)=SCHANNEL_ENABLE|SOUND_REPEAT |SOUND_VOL(0x7F)|SOUND_PAN(0x40)|SOUND_FORMAT_8BIT;

	TIMERXCNT(0) = TIMER_ENABLE; 
	TIMERXCNT(1) = TIMER_CASCADE | TIMER_IRQ_REQ | TIMER_ENABLE;
}

void stopsound() {
	SCHANNEL_CR(0)=0;
	SCHANNEL_CR(1)=0;
	SCHANNEL_CR(2)=0;
	SCHANNEL_CR(3)=0;
	SCHANNEL_CR(4)=0;
	SCHANNEL_CR(5)=0;
	SCHANNEL_CR(6)=0;
	SCHANNEL_CR(7)=0;
	SCHANNEL_CR(8)=0;
	SCHANNEL_CR(10)=0;
	TIMERXCNT(0) = 0;
	TIMERXCNT(1) = 0;
}

int pcmpos = 0;
int APU_paused=0;

Int32 NESAPUSoundSquareRender1();
Int32 NESAPUSoundSquareRender2();
Int32 NESAPUSoundTriangleRender1();
Int32 NESAPUSoundNoiseRender1();
Int32 NESAPUSoundDpcmRender1();
Int32 FDSSoundRender1();
Int32 FDSSoundRender2();
Int32 FDSSoundRender3();
Int32 VRC6SoundRender1();
Int32 VRC6SoundRender2();
Int32 VRC6SoundRender3();
void VRC6SoundInstall();

void mix(int chan) {
	int mapper = IPC_MAPPER;
	if(!APU_paused) {
		int i;
		s16 *dst = (s16*)&buffer[chan*MIXBUFSIZE];
		for(i = 0; i < MIXBUFSIZE; i++) {
			static Int32 preval = 0;
			Int32 output = soundconvert(NESAPUSoundSquareRender1(), 6);
			*dst++ = ((preval + output) / 2);
			preval = output;
		}
		dst+=MIXBUFSIZE;
		for(i = 0; i < MIXBUFSIZE; i++) {
			static Int32 preval = 0;
			Int32 output = soundconvert(NESAPUSoundSquareRender2(), 6);
			*dst++ = ((preval + output) / 2);
			preval = output;
		}
		dst+=MIXBUFSIZE;
		for(i = 0; i < MIXBUFSIZE; i++) {
			static Int32 preval = 0;
			Int32 output = soundconvert(NESAPUSoundTriangleRender1(), 7);
			*dst++ = ((preval + output) / 2);
			preval = output;
		}
		dst+=MIXBUFSIZE;
		for(i = 0; i < MIXBUFSIZE; i++) {
			static Int32 preval = 0;
			Int32 output = soundconvert(NESAPUSoundNoiseRender1(), 6);
			output = ((preval + output) / 2);
			*dst++ = output;
			preval = output;
		}
		dst+=MIXBUFSIZE;
		for(i = 0; i < MIXBUFSIZE; i++) {
			static Int32 preval = 0;
			Int32 output = soundconvert(NESAPUSoundDpcmRender1(), 4);
			output = ((preval + output) / 2);
			*dst++ = output;
			preval = output;
		}
		dst+=MIXBUFSIZE;
		if(mapper == 20 || mapper == 256) {
			for(i = 0; i < MIXBUFSIZE; i++) {
				static Int32 preval = 0;
				Int32 output = soundconvert(FDSSoundRender3(), 0);
				output = ((preval + output) / 2);
				*dst++ = output;
				preval = output;
			}
		} else {
			dst+=MIXBUFSIZE;
		}
		dst+=MIXBUFSIZE;
		if(mapper == 24 || mapper == 26 || mapper == 256) {
			for(i = 0; i < MIXBUFSIZE; i++) {
				static Int32 preval = 0;
				Int32 output = VRC6SoundRender1() << 11;
				output = ((preval + output) / 2);
				*dst++ = output;
				preval = output;
			}
			dst+=MIXBUFSIZE;
			for(i = 0; i < MIXBUFSIZE; i++) {
				static Int32 preval = 0;
				Int32 output = VRC6SoundRender2() << 11;
				output = ((preval + output) / 2);
				*dst++ = output;
				preval = output;
			}
			dst+=MIXBUFSIZE;
			for(i = 0; i < MIXBUFSIZE; i++) {
				static Int32 preval = 0;
				Int32 output = VRC6SoundRender3() << 10;
				output = ((preval + output) / 2);
				*dst++ = output;
				preval = output;
			}
		}

		dealrawpcm((u8 *)&buffer[chan*(MIXBUFSIZE/2) + MIXBUFSIZE*18]);
	}
	
	APU4015Reg();	//to refresh reg4015.
}

void initsound() { 		
	int i;
	powerON(POWER_SOUND); 
	REG_SOUNDCNT = SOUND_ENABLE | SOUND_VOL(0x7F);
	for(i = 0; i < 16; i++) {
		SCHANNEL_CR(i) = 0;
	}
	SCHANNEL_SOURCE(0)=(u32)&buffer[0];
	SCHANNEL_SOURCE(1)=(u32)&buffer[2*MIXBUFSIZE];
	SCHANNEL_SOURCE(2)=(u32)&buffer[4*MIXBUFSIZE];
	SCHANNEL_SOURCE(3)=(u32)&buffer[6*MIXBUFSIZE];
	SCHANNEL_SOURCE(4)=(u32)&buffer[8*MIXBUFSIZE];
	SCHANNEL_SOURCE(5)=(u32)&buffer[10*MIXBUFSIZE];
	SCHANNEL_SOURCE(6)=(u32)&buffer[12*MIXBUFSIZE];
	SCHANNEL_SOURCE(7)=(u32)&buffer[14*MIXBUFSIZE];
	SCHANNEL_SOURCE(8)=(u32)&buffer[16*MIXBUFSIZE];
	SCHANNEL_SOURCE(10)=(u32)&buffer[18*MIXBUFSIZE];
	SCHANNEL_TIMER(0)=-0x2b9; 
	SCHANNEL_TIMER(1)=-0x2b9; 
	SCHANNEL_TIMER(2)=-0x2b9; 
	SCHANNEL_TIMER(3)=-0x2b9; 
	SCHANNEL_TIMER(4)=-0x2b9; 
	SCHANNEL_TIMER(5)=-0x2b9; 
	SCHANNEL_TIMER(6)=-0x2b9; 
	SCHANNEL_TIMER(7)=-0x2b9; 
	SCHANNEL_TIMER(8)=-0x2b9; 
	SCHANNEL_TIMER(10)=-0x2b9; 
	SCHANNEL_LENGTH(0)=MIXBUFSIZE;
	SCHANNEL_LENGTH(1)=MIXBUFSIZE;
	SCHANNEL_LENGTH(2)=MIXBUFSIZE;
	SCHANNEL_LENGTH(3)=MIXBUFSIZE;
	SCHANNEL_LENGTH(4)=MIXBUFSIZE;
	SCHANNEL_LENGTH(5)=MIXBUFSIZE;
	SCHANNEL_LENGTH(6)=MIXBUFSIZE;
	SCHANNEL_LENGTH(7)=MIXBUFSIZE;
	SCHANNEL_LENGTH(8)=MIXBUFSIZE;
	SCHANNEL_LENGTH(10)=MIXBUFSIZE / 2;
	SCHANNEL_REPEAT_POINT(0) = 0; 
	SCHANNEL_REPEAT_POINT(1) = 0; 
	SCHANNEL_REPEAT_POINT(2) = 0; 
	SCHANNEL_REPEAT_POINT(3) = 0; 
	SCHANNEL_REPEAT_POINT(4) = 0; 
	SCHANNEL_REPEAT_POINT(5) = 0; 
	SCHANNEL_REPEAT_POINT(6) = 0; 
	SCHANNEL_REPEAT_POINT(1) = 0; 
	SCHANNEL_REPEAT_POINT(8) = 0; 
	SCHANNEL_REPEAT_POINT(10) = 0; 
	
	TIMERXDATA(0) = -0x572;
	TIMERXDATA(1) = 0x10000 - MIXBUFSIZE;
	memset((u32*)buffer, 0, sizeof(buffer));

	memset(IPC_PCMDATA, 0, 512);
} 

void lidinterrupt(void)
{
	stopsound();
	restartsound(1);
}

void soundinterrupt(void)
{
	chan^=1;
	mix(chan);
	if(REG_IF & IRQ_TIMER1) {
		lidinterrupt();
		chan = 1;
		REG_IF = IRQ_TIMER1;
	}

}

static unsigned char pcm_out = 0x3F;
int pcm_line = 120;
int pcmprevol = 0x3F;
void dealrawpcm(unsigned char *out) 
{
	unsigned char *in = IPC_PCMDATA;
	int i;
	int count = 0;
	int line = 0;
	unsigned char *outp = out;

	pcm_line = REG_VCOUNT;

	if(1) {
		for(i = 0; i < MIXBUFSIZE; i++) {
			if(in[pcm_line] & 0x80) {
				pcm_out = in[pcm_line] & 0x7F;
				in[pcm_line] = 0;
				count++;
			}
			*out++ = (pcm_out + pcmprevol - 0x80);
			pcmprevol = pcm_out;
			line += 100;
			if(line >= 152) {
				line -= 152;
				pcm_line++;
				if(pcm_line > 262) {
					pcm_line = 0;
				}
			}
		}
	}
	if(count < 10) {		//not a playable raw pcm.
		for(i = 0; i < MIXBUFSIZE; i++) {
			*outp++ = 0;
			pcmprevol = 0x3F;
			pcm_out = 0x3F;
		}
	}
}

void resetAPU() {
	NESReset();
	IPC_APUW = 0;
	IPC_APUR = 0;
}

void nesmain() {
	NESAudioFrequencySet(MIXFREQ);
	NESTerminate();
	NESHandlerInitialize();
	NESAudioHandlerInitialize();
	APUSoundInstall();
	FDSSoundInstall();
	VRC6SoundInstall();
	
	resetAPU();
	NESVolume(0);
	
	IRQVBlankWait();
	initsound();
	restartsound(1);

	//fifoSetValue32Handler(FIFO_USER_08, fifointerrupt, 0);		//use the last IPC channel to comm..
	//irqSet(IRQ_TIMER1, soundinterrupt);
	
}
