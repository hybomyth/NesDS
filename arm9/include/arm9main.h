#ifdef __cplusplus
extern "C" {
#endif

extern int soft_frameskip;
extern int global_playcount;				//used for NTSC/PAL
extern int subscreen_stat;				//short-cuts will change its value.
extern int argc;
extern char **argv;

extern u32 interrupts_to_wait_arm9;
extern int nesds_framecount;

extern int (*__hblankhook)();
#ifdef __cplusplus
}
#endif
