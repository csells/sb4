/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1998, Chris Sells
// All rights reserved.
// 3/17/98: 1st release.
//
// NO WARRANTIES ARE EXTENDED. USE AT YOUR OWN RISK.
//
// To contact the author with suggestions or comments, use csells@sellsbrothers.com.
/////////////////////////////////////////////////////////////////////////////
// This file contains the implementation of CoGetGit that exposes
// the single instance of the IGlobalInterfaceTable interface.
// Include this file in exactly one place in DLL or EXE.

#include "githelp.h"

HRESULT CoGetGit(IGlobalInterfaceTable** ppgit)
{
    HRESULT hr = S_OK;
    static IGlobalInterfaceTable*   pgit = 0;
    if( !pgit )
    {
        hr = CoCreateInstance(CLSID_StdGlobalInterfaceTable,
                              0,
                              CLSCTX_INPROC_SERVER,
                              IID_IGlobalInterfaceTable,
                              (void**)&pgit);
    }

    if( *ppgit = pgit ) (*ppgit)->AddRef();
    return hr;
}
