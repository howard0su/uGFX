/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_OS_CHIBIOS

#include <string.h>

#if CH_KERNEL_MAJOR < 2 || CH_KERNEL_MAJOR > 5
	#error "GOS: Unsupported version of ChibiOS"
#endif

#if CH_KERNEL_MAJOR <= 2
	#if !CH_USE_MUTEXES
		#error "GOS: CH_USE_MUTEXES must be defined in chconf.h"
	#endif
	#if !CH_USE_SEMAPHORES
		#error "GOS: CH_USE_SEMAPHORES must be defined in chconf.h"
	#endif
#else
	#if !CH_CFG_USE_MUTEXES
		#error "GOS: CH_CFG_USE_MUTEXES must be defined in chconf.h"
	#endif
	#if !CH_CFG_USE_SEMAPHORES
		#error "GOS: CH_CFG_USE_SEMAPHORES must be defined in chconf.h"
	#endif
#endif

void _gosInit(void)
{
	#if !GFX_OS_NO_INIT
		/* Don't Initialize if the user already has */
		#if CH_KERNEL_MAJOR <= 2
			if (!chThdSelf()) {
				halInit();
				chSysInit();
			}
		#else
			if (!chThdGetSelfX()) {
				halInit();
				chSysInit();
			}
		#endif
	#elif !GFX_OS_INIT_NO_WARNING
		#if GFX_COMPILER_WARNING_TYPE == GFX_COMPILER_WARNING_DIRECT
			#warning "GOS: Operating System initialization has been turned off. Make sure you call halInit() and chSysInit() before gfxInit() in your application!"
		#elif GFX_COMPILER_WARNING_TYPE == GFX_COMPILER_WARNING_MACRO
			COMPILER_WARNING("GOS: Operating System initialization has been turned off. Make sure you call halInit() and chSysInit() before gfxInit() in your application!")
		#endif
	#endif
}

void _gosPostInit(void)
{
}

void _gosDeinit(void)
{
	/* ToDo */
}

void *gfxRealloc(void *ptr, size_t oldsz, size_t newsz)
{
	void *np;

	if (newsz <= oldsz)
		return ptr;

	np = gfxAlloc(newsz);
	if (!np)
		return 0;

	if (oldsz)
		memcpy(np, ptr, oldsz);

	return np;
}

void gfxSleepMilliseconds(gDelay ms)
{
	switch(ms) {
		case TIME_IMMEDIATE:	chThdYield();				return;
		case TIME_INFINITE:		chThdSleep(TIME_INFINITE);	return;
		default:				chThdSleepMilliseconds(ms);	return;
	}
}

void gfxSleepMicroseconds(gDelay ms)
{
	switch(ms) {
		case TIME_IMMEDIATE:								return;
		case TIME_INFINITE:		chThdSleep(TIME_INFINITE);	return;
		default:				chThdSleepMicroseconds(ms);	return;
	}
}

void gfxSemInit(gfxSem *psem, gSemcount val, gSemcount limit)
{
	if (val > limit)
		val = limit;

	psem->limit = limit;
	
	#if CH_KERNEL_MAJOR <= 2
		chSemInit(&psem->sem, val);
	#else
		chSemObjectInit(&psem->sem, val);
	#endif
}

void gfxSemDestroy(gfxSem *psem)
{
	chSemReset(&psem->sem, 1);
}

gBool gfxSemWait(gfxSem *psem, gDelay ms)
{
	#if CH_KERNEL_MAJOR <= 2
		switch(ms) {
		case TIME_IMMEDIATE:	return chSemWaitTimeout(&psem->sem, TIME_IMMEDIATE) != RDY_TIMEOUT;
		case TIME_INFINITE:		chSemWait(&psem->sem);	return gTrue;
		default:				return chSemWaitTimeout(&psem->sem, gfxMillisecondsToTicks(ms)) != RDY_TIMEOUT;
		}
	#else
		switch(ms) {
		case TIME_IMMEDIATE:	return chSemWaitTimeout(&psem->sem, TIME_IMMEDIATE) != MSG_TIMEOUT;
		case TIME_INFINITE:		chSemWait(&psem->sem);	return gTrue;
		default:				return chSemWaitTimeout(&psem->sem, gfxMillisecondsToTicks(ms)) != MSG_TIMEOUT;
		}
	#endif
}

gBool gfxSemWaitI(gfxSem *psem)
{
	#if CH_KERNEL_MAJOR <= 3
		if (psem->sem.s_cnt <= 0)
			return gFalse;
	#else
		if (psem->sem.cnt <= 0)
			return gFalse;
	#endif
	chSemFastWaitI(&psem->sem);
	return gTrue;
}

void gfxSemSignal(gfxSem *psem)
{
	chSysLock();

	#if CH_KERNEL_MAJOR <= 3
		if (psem->sem.s_cnt < psem->limit)
			chSemSignalI(&psem->sem);
	#else
		if (psem->sem.cnt < psem->limit)
			chSemSignalI(&psem->sem);
	#endif

	chSchRescheduleS();
	chSysUnlock();
}

void gfxSemSignalI(gfxSem *psem)
{
	#if CH_KERNEL_MAJOR <= 3
		if (psem->sem.s_cnt < psem->limit)
			chSemSignalI(&psem->sem);
	#else
		if (psem->sem.cnt < psem->limit)
			chSemSignalI(&psem->sem);
	#endif
}

gfxThreadHandle gfxThreadCreate(void *stackarea, size_t stacksz, gThreadpriority prio, DECLARE_THREAD_FUNCTION((*fn),p), void *param)
{
	if (!stackarea) {
		if (!stacksz) stacksz = 256;
		#if CH_KERNEL_MAJOR <= 3
			return chThdCreateFromHeap(0, stacksz, prio, (tfunc_t)fn, param);
		#else
			return chThdCreateFromHeap(0, stacksz, "ugfx", prio, (tfunc_t)fn, param);
		#endif
	}

	if (!stacksz)
		return 0;

	return chThdCreateStatic(stackarea, stacksz, prio, (tfunc_t)fn, param);
}

#endif /* GFX_USE_OS_CHIBIOS */
