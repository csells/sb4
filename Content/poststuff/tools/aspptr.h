// comdate.h
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1998 Chris Sells
// All rights reserved.
// 12/30/98 - Initial release.
//
// NO WARRANTIES ARE EXTENDED. USE AT YOUR OWN RISK.
//
// To contact the author with suggestions or comments, use csells@sellsbrothers.com.
/////////////////////////////////////////////////////////////////////////////
// The file defines CAspPtr, a class derived from ATL's CComPtr to manage
// retrieving the interfaces from the five intrinsic ASP components. This
// class was inspired by Jon Flanders, jfland@tc.umn.edu, who wanted a wrapper
// around the IGetObjectProperties->GetProperty("<ASP Object Name>") code.
//
// template <typename T> class CAspPtr;
//
// When creating an instance of a CAspPtr, only one of the five interface
// pointers on the ASP objects can be used as T, i.e. IRequest, IResponse, 
// IServer, IApplicationObject and ISessionObject. Use of other interface
// types will cause a compiler error.
//
// This class depends on the GetObjectContext function defined in mtx.h
// and the IGetObjectProperties interface defined in asptlb.h. Only IIS4
// and beyond allow access to the ASP instrinsics objects using this technique.
/////////////////////////////////////////////////////////////////////////////
// Usage:
//
/*
STDMETHODIMP CJon::DoIt()
{
    CAspPtr<IResponse> spResponse;
    if( spResponse ) 
    {
        spResponse->Write(CComVariant(OLESTR("Baby got <b><i>Response</i></b>.")));
    }
    
    return S_OK;
}
*/

#pragma once
#ifndef __ASPPTR_H__
#define __ASPPTR_H__

#include <mtx.h>
#include <asptlb.h>

template <typename T>
class CAspPtr : public CComPtr<T> {
public:
    // NOTE: If you're getting a compiler error here, it's likely you're
    // trying to create a CAspPtr with an interface not supported by ASP, e.g.
    // CAspPtr<IFoo> spFoo; // ERR
    CAspPtr() { GetInterface(&p); }
    
private:
    // Template parameter switch statement...
    HRESULT GetInterface(IResponse** ppItf)
    { return GetInterface(CComBSTR(OLESTR("Response")), __uuidof(*ppItf), (void**)ppItf); }
    
    HRESULT GetInterface(IRequest** ppItf)
    { return GetInterface(CComBSTR(OLESTR("Request")), __uuidof(*ppItf), (void**)ppItf); }
    
    HRESULT GetInterface(IServer** ppItf)
    { return GetInterface(CComBSTR(OLESTR("Server")), __uuidof(*ppItf), (void**)ppItf); }
    
    HRESULT GetInterface(ISessionObject** ppItf)
    { return GetInterface(CComBSTR(OLESTR("Session")), __uuidof(*ppItf), (void**)ppItf); }
    
    HRESULT GetInterface(IApplicationObject** ppItf)
    { return GetInterface(CComBSTR(OLESTR("Application")), __uuidof(*ppItf), (void**)ppItf); }

    // Use the name of the object to obtain the interface from the object context
    HRESULT GetInterface(BSTR bstr, REFIID riid, void** ppv)
    {
        *ppv = 0;
        HRESULT hr = E_FAIL;

        CComPtr<IObjectContext> spContext;
        hr = GetObjectContext(&spContext);
        if( FAILED(hr) ) return hr;
        
        CComQIPtr<IGetContextProperties> spProperties = spContext;
        if( !spProperties ) return E_NOINTERFACE;
        
        CComVariant varItf;
        hr = spProperties->GetProperty(bstr, &varItf);
        if( hr != S_OK || (varItf.vt != VT_DISPATCH && varItf.vt != VT_UNKNOWN) )
        {
            return E_UNEXPECTED;
        }

        return varItf.punkVal->QueryInterface(riid, ppv);
    }
};

#endif  // __ASPPTR_H__
