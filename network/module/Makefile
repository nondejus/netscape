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

DEPTH			= ../..

MODULE			= netlib
LIBRARY_NAME		= netlib

CPPSRCS			= \
			  nsNetService.cpp    \
			  nsRelatedLinks.cpp  \
			  nsNetStream.cpp     \
			  nsURL.cpp           \
			  nsHttpUrl.cpp       \
			  nsStubContext.cpp   \
			  nsNetStubs.cpp      \
			  nsNetIDs.cpp        \
			  $(NULL)

REQUIRES		= raptor js dbm nspr security pref xpcom util img \
			  layer network netcache fileurl ftpurl abouturl \
			  httpurl gophurl remoturl netcnvts

EXPORTS = nsIStreamListener.h           \
          nsINetService.h               \
	  nsINetSupport.h		\
          nsIURL.h                      \
          nsIRelatedLinks.h             \
          nsIPostToServer.h             \
          nsIHttpUrl.h                  \
          nsINetContainerApplication.h  \
          $(NULL)

include $(DEPTH)/config/config.mk

include $(DEPTH)/config/rules.mk

$(LIBRARY): \
	$(OBJS)

install::
	$(INSTALL) res/gopher-audio.gif   $(DIST)/bin/res/network
	$(INSTALL) res/gopher-binary.gif  $(DIST)/bin/res/network
	$(INSTALL) res/gopher-find.gif    $(DIST)/bin/res/network
	$(INSTALL) res/gopher-image.gif   $(DIST)/bin/res/network
	$(INSTALL) res/gopher-menu.gif    $(DIST)/bin/res/network
	$(INSTALL) res/gopher-movie.gif   $(DIST)/bin/res/network
	$(INSTALL) res/gopher-telnet.gif  $(DIST)/bin/res/network
	$(INSTALL) res/gopher-text.gif    $(DIST)/bin/res/network
	$(INSTALL) res/gopher-unknown.gif $(DIST)/bin/res/network

clobber::
	rm -f $(DIST)/bin/res/network/gopher-audio.gif
	rm -f $(DIST)/bin/res/network/gopher-binary.gif
	rm -f $(DIST)/bin/res/network/gopher-find.gif
	rm -f $(DIST)/bin/res/network/gopher-image.gif
	rm -f $(DIST)/bin/res/network/gopher-menu.gif
	rm -f $(DIST)/bin/res/network/gopher-movie.gif
	rm -f $(DIST)/bin/res/network/gopher-telnet.gif
	rm -f $(DIST)/bin/res/network/gopher-text.gif
	rm -f $(DIST)/bin/res/network/gopher-unknown.gif

