/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
 *
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
/*******************************************************************************
 * Source date: 9 Apr 1997 21:45:13 GMT
 * netscape/fonts/cstrm module C header file
 * Generated by jmc version 1.8 -- DO NOT EDIT
 ******************************************************************************/

#ifndef _Mcstrm_H_
#define _Mcstrm_H_

#include "jmc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
 * cstrm
 ******************************************************************************/

/* The type of the cstrm interface. */
struct cstrmInterface;

/* The public type of a cstrm instance. */
typedef struct cstrm {
	const struct cstrmInterface*	vtable;
} cstrm;

/* The inteface ID of the cstrm interface. */
#ifndef JMC_INIT_cstrm_ID
extern EXTERN_C_WITHOUT_EXTERN const JMCInterfaceID cstrm_ID;
#else
EXTERN_C const JMCInterfaceID cstrm_ID = { 0x3b686461, 0x5c714911, 0x4c06346e, 0x1c47186a };
#endif /* JMC_INIT_cstrm_ID */
/*******************************************************************************
 * cstrm Operations
 ******************************************************************************/

#define cstrm_getInterface(self, a, exception)	\
	(((self)->vtable->getInterface)(self, cstrm_getInterface_op, a, exception))

#define cstrm_addRef(self, exception)	\
	(((self)->vtable->addRef)(self, cstrm_addRef_op, exception))

#define cstrm_release(self, exception)	\
	(((self)->vtable->release)(self, cstrm_release_op, exception))

#define cstrm_hashCode(self, exception)	\
	(((self)->vtable->hashCode)(self, cstrm_hashCode_op, exception))

#define cstrm_equals(self, a, exception)	\
	(((self)->vtable->equals)(self, cstrm_equals_op, a, exception))

#define cstrm_clone(self, exception)	\
	(((self)->vtable->clone)(self, cstrm_clone_op, exception))

#define cstrm_toString(self, exception)	\
	(((self)->vtable->toString)(self, cstrm_toString_op, exception))

#define cstrm_finalize(self, exception)	\
	(((self)->vtable->finalize)(self, cstrm_finalize_op, exception))

#define cstrm_IsWriteReady(self, exception)	\
	(((self)->vtable->IsWriteReady)(self, cstrm_IsWriteReady_op, exception))

#define cstrm_Write(self, a, a_length, exception)	\
	(((self)->vtable->Write)(self, cstrm_Write_op, a, a_length, exception))

#define cstrm_Abort(self, a, exception)	\
	(((self)->vtable->Abort)(self, cstrm_Abort_op, a, exception))

#define cstrm_Complete(self, exception)	\
	(((self)->vtable->Complete)(self, cstrm_Complete_op, exception))

/*******************************************************************************
 * cstrm Interface
 ******************************************************************************/

struct netscape_jmc_JMCInterfaceID;
struct java_lang_Object;
struct java_lang_String;

struct cstrmInterface {
	void*	(*getInterface)(struct cstrm* self, jint op, const JMCInterfaceID* a, JMCException* *exception);
	void	(*addRef)(struct cstrm* self, jint op, JMCException* *exception);
	void	(*release)(struct cstrm* self, jint op, JMCException* *exception);
	jint	(*hashCode)(struct cstrm* self, jint op, JMCException* *exception);
	jbool	(*equals)(struct cstrm* self, jint op, void* a, JMCException* *exception);
	void*	(*clone)(struct cstrm* self, jint op, JMCException* *exception);
	const char*	(*toString)(struct cstrm* self, jint op, JMCException* *exception);
	void	(*finalize)(struct cstrm* self, jint op, JMCException* *exception);
	jint	(*IsWriteReady)(struct cstrm* self, jint op, JMCException* *exception);
	jint	(*Write)(struct cstrm* self, jint op, jbyte* a, jsize a_length, JMCException* *exception);
	void	(*Abort)(struct cstrm* self, jint op, jint a, JMCException* *exception);
	void*	(*Complete)(struct cstrm* self, jint op, JMCException* *exception);
};

/*******************************************************************************
 * cstrm Operation IDs
 ******************************************************************************/

typedef enum cstrmOperations {
	cstrm_getInterface_op,
	cstrm_addRef_op,
	cstrm_release_op,
	cstrm_hashCode_op,
	cstrm_equals_op,
	cstrm_clone_op,
	cstrm_toString_op,
	cstrm_finalize_op,
	cstrm_IsWriteReady_op,
	cstrm_Write_op,
	cstrm_Abort_op,
	cstrm_Complete_op
} cstrmOperations;

/*******************************************************************************
 * Writing your C implementation: "cstrm.h"
 * *****************************************************************************
 * You must create a header file named "cstrm.h" that implements
 * the struct cstrmImpl, including the struct cstrmImplHeader
 * as it's first field:
 * 
 * 		#include "Mcstrm.h" // generated header
 * 
 * 		struct cstrmImpl {
 * 			cstrmImplHeader	header;
 * 			<your instance data>
 * 		};
 * 
 * This header file will get included by the generated module implementation.
 ******************************************************************************/

/* Forward reference to the user-defined instance struct: */
typedef struct cstrmImpl	cstrmImpl;


/* This struct must be included as the first field of your instance struct: */
typedef struct cstrmImplHeader {
	const struct cstrmInterface*	vtablecstrm;
	jint		refcount;
} cstrmImplHeader;

/*******************************************************************************
 * Instance Casting Macros
 * These macros get your back to the top of your instance, cstrm,
 * given a pointer to one of its interfaces.
 ******************************************************************************/

#undef  cstrmImpl2nfstrm
#define cstrmImpl2nfstrm(cstrmImplPtr) \
	((nfstrm*)((char*)(cstrmImplPtr) + offsetof(cstrmImplHeader, vtablecstrm)))

#undef  nfstrm2cstrmImpl
#define nfstrm2cstrmImpl(nfstrmPtr) \
	((cstrmImpl*)((char*)(nfstrmPtr) - offsetof(cstrmImplHeader, vtablecstrm)))

#undef  cstrmImpl2cstrm
#define cstrmImpl2cstrm(cstrmImplPtr) \
	((cstrm*)((char*)(cstrmImplPtr) + offsetof(cstrmImplHeader, vtablecstrm)))

#undef  cstrm2cstrmImpl
#define cstrm2cstrmImpl(cstrmPtr) \
	((cstrmImpl*)((char*)(cstrmPtr) - offsetof(cstrmImplHeader, vtablecstrm)))

/*******************************************************************************
 * Operations you must implement
 ******************************************************************************/


extern JMC_PUBLIC_API(void*)
_cstrm_getBackwardCompatibleInterface(struct cstrm* self, const JMCInterfaceID* iid,
	JMCException* *exception);

extern JMC_PUBLIC_API(void)
_cstrm_init(struct cstrm* self, JMCException* *exception, struct nfrc* a, const char* b, const char* c);

extern JMC_PUBLIC_API(void*)
_cstrm_getInterface(struct cstrm* self, jint op, const JMCInterfaceID* a, JMCException* *exception);

extern JMC_PUBLIC_API(void)
_cstrm_addRef(struct cstrm* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(void)
_cstrm_release(struct cstrm* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(jint)
_cstrm_hashCode(struct cstrm* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(jbool)
_cstrm_equals(struct cstrm* self, jint op, void* a, JMCException* *exception);

extern JMC_PUBLIC_API(void*)
_cstrm_clone(struct cstrm* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(const char*)
_cstrm_toString(struct cstrm* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(void)
_cstrm_finalize(struct cstrm* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(jint)
_cstrm_IsWriteReady(struct cstrm* self, jint op, JMCException* *exception);

extern JMC_PUBLIC_API(jint)
_cstrm_Write(struct cstrm* self, jint op, jbyte * a, jsize a_length, JMCException* *exception);

extern JMC_PUBLIC_API(void)
_cstrm_Abort(struct cstrm* self, jint op, jint a, JMCException* *exception);

extern JMC_PUBLIC_API(void*)
_cstrm_Complete(struct cstrm* self, jint op, JMCException* *exception);

/*******************************************************************************
 * Factory Operations
 ******************************************************************************/

JMC_PUBLIC_API(cstrm*)
cstrmFactory_Create(JMCException* *exception, struct nfrc* a, const char* b, const char* c);

/******************************************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _Mcstrm_H_ */
