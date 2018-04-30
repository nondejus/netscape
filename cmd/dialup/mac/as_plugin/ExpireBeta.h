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

//=========================================================================================
//ExpireBeta.h header file
//=========================================================================================

#pragma once


//unsigned long GetFileCreationDate(short refNum);
//Boolean CheckIfExpired(void);

#define NumDaysStrID		7000	//the 'STR ' resource that holds the number of days until expiration - if not found, expires immediately
#define kVersDevelopment	0x20
#define kVersAlpha		0x40
#define kVersBeta		0x60	
#define kVersRelease	0x80

#define kIsExpired		true
#define kIsNotExpired	false
