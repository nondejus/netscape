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

#include "nscore.h"
#include "nsIFactory.h"
#include "nsISupports.h"

#include "nsPluginsCID.h"
#include "nsPluginHostImpl.h"

static NS_DEFINE_IID(kCPluginHost, NS_PLUGIN_HOST_CID);

static NS_DEFINE_IID(kISupportsIID, NS_ISUPPORTS_IID);
static NS_DEFINE_IID(kIFactoryIID, NS_IFACTORY_IID);

class nsPluginFactory : public nsIFactory
{   
  public:   
    // nsISupports methods   
    NS_IMETHOD QueryInterface(const nsIID &aIID,    
                                       void **aResult);   
    NS_IMETHOD_(nsrefcnt) AddRef(void);   
    NS_IMETHOD_(nsrefcnt) Release(void);   

    // nsIFactory methods   
    NS_IMETHOD CreateInstance(nsISupports *aOuter,   
                              const nsIID &aIID,   
                              void **aResult);   

    NS_IMETHOD LockFactory(PRBool aLock);   

    nsPluginFactory(const nsCID &aClass);   

  protected:
    virtual ~nsPluginFactory();   

  private:   
    nsrefcnt  mRefCnt;   
    nsCID     mClassID;
};   

nsPluginFactory :: nsPluginFactory(const nsCID &aClass)   
{   
  mRefCnt = 0;
  mClassID = aClass;
}   

nsPluginFactory :: ~nsPluginFactory()   
{   
  NS_ASSERTION(mRefCnt == 0, "non-zero refcnt at destruction");   
}   

nsresult nsPluginFactory :: QueryInterface(const nsIID &aIID,   
                                       void **aResult)   
{   
  if (aResult == NULL) {   
    return NS_ERROR_NULL_POINTER;   
  }   

  // Always NULL result, in case of failure   
  *aResult = NULL;   

  if (aIID.Equals(kISupportsIID)) {   
    *aResult = (void *)(nsISupports*)this;   
  } else if (aIID.Equals(kIFactoryIID)) {   
    *aResult = (void *)(nsIFactory*)this;   
  }   

  if (*aResult == NULL) {   
    return NS_NOINTERFACE;   
  }   

  AddRef(); // Increase reference count for caller   
  return NS_OK;   
}   

nsrefcnt nsPluginFactory :: AddRef()   
{   
  return ++mRefCnt;   
}   

nsrefcnt nsPluginFactory :: Release()   
{   
  if (--mRefCnt == 0) {   
    delete this;   
    return 0; // Don't access mRefCnt after deleting!   
  }   
  return mRefCnt;   
}  

nsresult nsPluginFactory :: CreateInstance(nsISupports *aOuter,  
                                          const nsIID &aIID,  
                                          void **aResult)  
{  
  if (aResult == NULL) {  
    return NS_ERROR_NULL_POINTER;  
  }  

  *aResult = NULL;  
  
  nsISupports *inst = nsnull;

  if (mClassID.Equals(kCPluginHost)) {
    inst = (nsISupports *)(nsIPluginManager *)new nsPluginHostImpl();
  }

  if (inst == NULL) {  
    return NS_ERROR_OUT_OF_MEMORY;  
  }  

  nsresult res = inst->QueryInterface(aIID, aResult);

  if (res != NS_OK) {  
    // We didn't get the right interface, so clean up  
    delete inst;  
  }  

  return res;  
}  

nsresult nsPluginFactory :: LockFactory(PRBool aLock)  
{  
  // Not implemented in simplest case.  
  return NS_OK;
}  

// return the proper factory to the caller
extern "C" NS_PLUGIN nsresult NSGetFactory(const nsCID &aClass, nsIFactory **aFactory)
{
  if (nsnull == aFactory) {
    return NS_ERROR_NULL_POINTER;
  }

  *aFactory = new nsPluginFactory(aClass);

  if (nsnull == aFactory) {
    return NS_ERROR_OUT_OF_MEMORY;
  }

  return (*aFactory)->QueryInterface(kIFactoryIID, (void**)aFactory);
}
