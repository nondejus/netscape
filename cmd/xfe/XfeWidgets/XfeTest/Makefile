#!gmake
#
# The contents of this file are subject to the Netscape Public License
# Version 1.0 (the "NPL"); you may not use this file except in
# compliance with the NPL.  You may obtain a copy of the NPL at
# http://www.mozilla.org/NPL/
#
# Software distributed under the NPL is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
# for the specific language governing rights and limitations under the
# NPL.
#
# The Initial Developer of this code under the NPL is Netscape
# Communications Corporation.  Portions created by Netscape are
# Copyright (C) 1998 Netscape Communications Corporation.  All Rights
# Reserved.


##########################################################################
#
# Name:			Makefile
# Description:	Makefile for XfeTest library
# Author:		Ramiro Estrugo <ramiro@netscape.com>
#
##########################################################################

DEPTH			= ../../../..

# XfeTest headers (only one) are exported to dist/public/XfeTest/Xfe
MODULE			= XfeTest/Xfe

LIBRARY_NAME	= XfeTest

CSRCS			= \
				TestAnim.c \
				TestApp.c \
				TestCallbacks.c \
				TestCreate.c \
				TestIcons.c \
				TestMenu.c \
				TestPixmap.c \
				TestToolBar.c \
				TestUtil.c \
				$(NULL)

REQUIRES		= \
				XfeTest \
				XfeWidgets \
				XfeBm

EXPORTS			= \
				XfeTest.h \
				$(NULL)

include $(DEPTH)/config/rules.mk

include $(DEPTH)/cmd/xfe/XfeWidgets/XfeWidgets.mk

INCLUDES	+= -I../icons
