/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
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

#include <xp_core.h>
#include <xp_str.h>

#include "nsCacheModule.h"
#include "nsCacheTrace.h"
#include "nsCacheIterator.h"

/* 
 * nsCacheModule
 *
 * Gagan Saksena 02/02/98
 * 
 */


#define DEFAULT_SIZE 10*0x100000L

nsCacheModule::nsCacheModule(const PRUint32 i_size=DEFAULT_SIZE):
    m_Size(i_size),
    m_pNext(0),
    m_Entries(0)
{
    m_pIterator = new nsCacheIterator(this);
}

nsCacheModule::~nsCacheModule()
{
    if (m_pNext)
    {
        delete m_pNext;
        m_pNext = 0;
    }
    if (m_pIterator)
    {
        delete m_pIterator;
        m_pIterator = 0;
    }
    if (m_pEnumeration)
    {
        delete m_pEnumeration;
        m_pEnumeration = 0;
    }
}

void nsCacheModule::GarbageCollect(void) 
{
}

PRBool nsCacheModule::RemoveAll(void)
{
    PRBool status = PR_TRUE;
    while (m_Entries > 0)
    {
        status &= Remove(--m_Entries);
    }
    return status;
}

const char* nsCacheModule::Trace() const
{
    char linebuffer[128];
    char* total;

    sprintf(linebuffer, "nsCacheModule: Objects = %d\n", Entries());

    total = new char[strlen(linebuffer) + 1];
    strcpy(total, linebuffer);

    return total;
}