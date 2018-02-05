#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "touch_ipc.h"
#include "specific_shared.h"
#include "keypadTGDS.h"

u32 keyscurr_ipc(){
	struct sIPCSharedTGDS* sIPCSharedTGDSInst = getsIPCSharedTGDS();
	return (( ((~sIPCSharedTGDSInst->KEYINPUT7)&0x3ff) | (((~sIPCSharedTGDSInst->buttons7)&3)<<10) | (((~sIPCSharedTGDSInst->buttons7)<<6) & (KEY_TOUCH|KEY_LID) ))^KEY_LID);
}