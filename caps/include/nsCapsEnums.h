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

#ifndef _NS_CAPS_ENUMS_H_
#define _NS_CAPS_ENUMS_H_

typedef enum nsSetComparisonType {
  nsSetComparisonType_ProperSubset=-1,
  nsSetComparisonType_Equal=0,
  nsSetComparisonType_NoSubset=1
} nsSetComparisonType;

/**
 * number of possible permissions (allowed, forbidden, or blank)
 */
typedef enum nsPermissionState {
    nsPermissionState_Forbidden = 0, 
    nsPermissionState_Allowed,
    nsPermissionState_Blank,
    nsPermissionState_NumberOfPermissions
} nsPermissionState;

/**
 * number of possible durations (scope, session, or forever)
 */
typedef enum nsDurationState {
    nsDurationState_Scope=0, 
    nsDurationState_Session,
    nsDurationState_Forever, 
    nsDurationState_NumberOfDurations
} nsDurationState;


/* The following should match what is in nsJVM plugin's java security code */
typedef enum nsPrincipalType {
  nsPrincipalType_Unknown=-1, 
  nsPrincipalType_CodebaseExact=10,
  nsPrincipalType_CodebaseRegexp,
  nsPrincipalType_Cert,
  nsPrincipalType_CertFingerPrint,
  nsPrincipalType_CertKey
} nsPrincipalType;

#endif /* _NS_CAPS_ENUMS_H_ */
