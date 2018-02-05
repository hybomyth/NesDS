#ifndef AUDIOSYS_H__
#define AUDIOSYS_H__

#include "nestypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*AUDIOHANDLER2)(Int32 *p);
typedef Int32 (*AUDIOHANDLER)(void);
typedef struct NES_AUDIO_HANDLER_TAG {
	Uint fMode;
	AUDIOHANDLER Proc;
	AUDIOHANDLER2 Proc2;
	struct NES_AUDIO_HANDLER_TAG *next;
} NES_AUDIO_HANDLER;

typedef void (*VOLUMEHANDLER)(Uint volume);
typedef struct NES_VOLUME_HANDLER_TAG {
	VOLUMEHANDLER Proc;
	struct NES_VOLUME_HANDLER_TAG *next;
} NES_VOLUME_HANDLER;

enum
{
   NES_AUDIO_FILTER_NONE,
   NES_AUDIO_FILTER_LOWPASS,
   NES_AUDIO_FILTER_WEIGHTED
};

void APU4015Reg(void);
void APUSoundInstall(void);
void NESAudioRender(Int16 *bufp, Uint buflen);
void NESAudioHandlerInstall(NES_AUDIO_HANDLER *ph);
void NESAudioFrequencySet(Uint freq);
Uint NESAudioFrequencyGet(void);
void NESAudioChannelSet(Uint ch);
Uint NESAudioChannelGet(void);
void NESAudioHandlerInitialize(void);
void NESVolumeHandlerInstall(NES_VOLUME_HANDLER *ph);
void NESVolume(Uint volume);
void NESAudioFilterSet(Uint filter);
extern void (*FDSSoundWriteHandler)(Uint address, Uint value);
void FDSSoundInstall(void);
extern const short logtable[1024];
#ifdef __cplusplus
}
#endif

#endif /* AUDIOSYS_H__ */
