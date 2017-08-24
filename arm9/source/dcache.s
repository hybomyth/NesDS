.align 4
.data 32

//---------------------------------------------------------------------------------
	.global	drainwrite
	.type	drainwrite STT_FUNC
//---------------------------------------------------------------------------------
drainwrite:
	push {r0,lr}
	mov     r0, #0
	mcr     p15, 0, r0, c7, c10, 4		@ drain write buffer
	pop {r0,lr}
	bx	lr
	
.pool
.end