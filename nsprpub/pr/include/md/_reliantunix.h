/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "NPL"); you may not use this file except in
 * compliance with the NPL.  You may obtain a copy of the NPL at
 * http://www.mozilla.org/NPL/
 * 
 * Software distributed under the NPL is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
 * for the specific language governing rights and limitations under the
 * NPL.
 * 
 * The Initial Developer of this code under the NPL is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation.  All Rights
 * Reserved.
 */

/*
 * reliantunix.h
 * 5/18/96 Taken from nec.h -- chrisk@netscape.com
 * 3/14/97 Modified for nspr20 -- chrisk@netscape.com
 */
#ifndef nspr_reliantunix_defs_h___
#define nspr_reliantunix_defs_h___

/*
 * Internal configuration macros
 */

#define PR_LINKER_ARCH	"sinix"
#define _PR_SI_SYSNAME	"SINIX"
#define _PR_SI_ARCHITECTURE "mips"
#define PR_DLL_SUFFIX ".so"

#define _PR_VMBASE		0x30000000
#define _PR_STACK_VMBASE	0x50000000
#define _MD_DEFAULT_STACK_SIZE	(2*65536L)
#define _MD_MMAP_FLAGS		MAP_PRIVATE|MAP_FIXED

#undef  HAVE_STACK_GROWING_UP
#define HAVE_DLL
#define USE_DLFCN
#define NEED_STRFTIME_LOCK
#define NEED_TIME_R
#define HAVE_NETCONFIG
#define HAVE_WEAK_IO_SYMBOLS
#define HAVE_WEAK_MALLOC_SYMBOLS
#define _PR_RECV_BROKEN /* recv doesn't work on Unix Domain Sockets */

/*
 * Mike Patnode indicated that it is possibly safe now to use context-switching
 * calls that do not change the signal mask, like setjmp vs. sigsetjmp.
 * So we'll use our homegrown, getcontext/setcontext-compatible stuff which 
 * will save us the getcontext/setcontext system calls at each context switch.
 * It already works in FastTrack 2.01, so it should do it here :-)
 *  - chrisk 040497
 */
#define USE_SETCXT /* temporarily disabled... */

#include <ucontext.h>
 
#ifdef USE_SETCXT
/* use non-syscall machine language replacement */
#define _GETCONTEXT		getcxt
#define _SETCONTEXT		setcxt
/* defined in os_ReliantUNIX.s */
extern int getcxt(ucontext_t *);
extern int setcxt(ucontext_t *);
#else
#define _GETCONTEXT		getcontext
#define _SETCONTEXT		setcontext
#endif

#define _MD_GET_SP(_t)		(_t)->md.context.uc_mcontext.gpregs[CXT_SP]
#define _PR_CONTEXT_TYPE	ucontext_t
#define _PR_NUM_GCREGS		NGREG
 
#define CONTEXT(_thread) (&(_thread)->md.context)

#define _PR_IS_NATIVE_THREAD_SUPPORTED() 0
 
/*
** Machine-dependent (MD) data structures.
*/
struct _MDThread {
    _PR_CONTEXT_TYPE context;
    int id;
    int errcode;
};

struct _MDThreadStack {
    PRInt8 notused;
};

struct _MDLock {
    PRInt8 notused;
};

struct _MDSemaphore {
    PRInt8 notused;
};

struct _MDCVar {
    PRInt8 notused;
};

struct _MDSegment {
    PRInt8 notused;
};

struct _MDCPU {
	struct _MDCPU_Unix md_unix;
};

#define _MD_INIT_LOCKS()
#define _MD_NEW_LOCK(lock) PR_SUCCESS
#define _MD_FREE_LOCK(lock)
#define _MD_LOCK(lock)
#define _MD_UNLOCK(lock)
#define _MD_INIT_IO()
#define _MD_IOQ_LOCK()
#define _MD_IOQ_UNLOCK()

/*
** Initialize the thread context preparing it to execute "_main()"
** - get a nice, fresh context
** - set its SP to the stack we allcoated for it
** - set it to start things at "e"
*/
#define _MD_INIT_CONTEXT(thread, _sp, _main, status)                \
    PR_BEGIN_MACRO                                                  \
    *status = PR_TRUE;                                              \
    _GETCONTEXT(CONTEXT(thread));                                   \
    /* this is supposed to point to the stack BASE, not to SP */    \
    CONTEXT(thread)->uc_stack.ss_sp = thread->stack->stackBottom;   \
    CONTEXT(thread)->uc_stack.ss_size = thread->stack->stackSize;   \
    CONTEXT(thread)->uc_mcontext.gpregs[CXT_SP] = ((unsigned long)_sp - 128) & 0xfffffff8; \
    CONTEXT(thread)->uc_mcontext.gpregs[CXT_T9] = _main;            \
    CONTEXT(thread)->uc_mcontext.gpregs[CXT_EPC] = _main;           \
    CONTEXT(thread)->uc_mcontext.gpregs[CXT_RA] = 0;                \
    thread->no_sched = 0;                                           \
    PR_END_MACRO
 
/*
** Save current context as it is scheduled away
*/
#define _MD_SWITCH_CONTEXT(_thread)       \
    PR_BEGIN_MACRO                        \
    if (!_GETCONTEXT(CONTEXT(_thread))) { \
	_MD_SAVE_ERRNO(_thread);          \
	_MD_SET_LAST_THREAD(_thread);     \
        _PR_Schedule();                   \
    }                                     \
    PR_END_MACRO
 
/*
** Restore a thread context, saved by _MD_SWITCH_CONTEXT or set up
**  by _MD_INIT_CONTEXT
**  CXT_V0 is the register that holds the return value.
**   We must set it to 1 so that we can see if the return from
**   getcontext() is the result of calling getcontext() or
**   setcontext()...
**   setting a context got with getcontext() appears to
**   return from getcontext(), too!
**  CXT_A3 is the register that holds status when returning
**   from a syscall. It is set to 0 to indicate success,
**   because we want getcontext() on the other side of the magic
**   door to be ok.
*/
#define _MD_RESTORE_CONTEXT(_thread)   \
    PR_BEGIN_MACRO                     \
    ucontext_t *uc = CONTEXT(_thread); \
    uc->uc_mcontext.gpregs[CXT_V0] = 1;\
    uc->uc_mcontext.gpregs[CXT_A3] = 0;\
    _MD_RESTORE_ERRNO(_thread);        \
    _MD_SET_CURRENT_THREAD(_thread);   \
    _SETCONTEXT(uc);                   \
    PR_END_MACRO

#define _MD_SAVE_ERRNO(t)	 (t)->md.errcode = errno;
#define _MD_RESTORE_ERRNO(t)	 errno = (t)->md.errcode;

#define _MD_GET_INTERVAL	_PR_UNIX_GetInterval
#define _MD_INTERVAL_PER_SEC	_PR_UNIX_TicksPerSecond

#define _MD_EARLY_INIT		_MD_EarlyInit
#define _MD_FINAL_INIT		_PR_UnixInit
#define _MD_INIT_RUNNING_CPU(cpu) _MD_unix_init_running_cpu(cpu)
#define _MD_INIT_THREAD		_MD_InitializeThread
#define _MD_EXIT_THREAD(thread)
#define _MD_SUSPEND_THREAD(thread)
#define _MD_RESUME_THREAD(thread)
#define _MD_CLEAN_THREAD(_thread)

#if !defined(S_ISSOCK) && defined(S_IFSOCK)
#define S_ISSOCK(mode)   ((mode&0xF000) == 0xC000)
#endif
#if !defined(S_ISLNK) && defined(S_IFLNK)
#define S_ISLNK(mode)   ((mode&0xA000) == 0xC000)
#endif

#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
extern int _select(int nfds, fd_set *readfds, fd_set *writefds,
	fd_set *execptfds, struct timeval *timeout);
#define _MD_SELECT(nfds,r,w,e,tv) _select(nfds,r,w,e,tv)
#define _MD_POLL _poll

#endif /* nspr_reliantunix_defs_h___ */
