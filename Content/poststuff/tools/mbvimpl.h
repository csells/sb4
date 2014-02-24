// mbvimpl.h: Jonathan Borden's Marshal-by-Value implementation
// Copyright (c) 1997 JABR Technology Corporation, all rights reserved
// Jonathan Borden 3/26/97
// jborden@MEDIAONE.NET

#pragma once

template <class T>
class ATL_NO_VTABLE IMarshalByValueImpl : public IMarshal
{
    // this template is modified from don box's aggregatable MBV ... some MSJ issue
    // the template obviates the need for an aggregatable objectpublic:
    
    // IUnknown
    STDMETHOD(QueryInterface)(REFIID riid, void ** ppvObject) = 0;
    _ATL_DEBUG_ADDREF_RELEASE_IMPL(IMarshalByValueImpl)

    // IMarshal 
    STDMETHODIMP GetUnmarshalClass(REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags, CLSID *pCid)
    {
        T* pT = static_cast<T*>(this);
        *pCid = pT->GetObjectCLSID();
        return S_OK;
    }
    
    STDMETHODIMP GetMarshalSizeMax(REFIID riid, void *pv, DWORD dwDestContext, void *pvDestContext, DWORD mshlflags, DWORD *pSize)
    {
        // IPersistStream::GetSizeMax
        T* pT = static_cast<T*>(this);
        HRESULT hr = E_FAIL;
        ULARGE_INTEGER uli;
        hr = pT->GetSizeMax(&uli);
        if (SUCCEEDED(hr))
            *pSize = uli.LowPart;
        return hr;
    }
    
    STDMETHODIMP MarshalInterface(IStream *pStm, REFIID riid, void *pv, DWORD dwDestContext, void *pvDestCtx, DWORD mshlflags)
    {
        // save to the stream
        T* pT = static_cast<T*>(this);
        HRESULT hr = E_FAIL;
        hr = pT->Save(pStm, FALSE);
        return hr;
    }
    
    STDMETHODIMP UnmarshalInterface(IStream *pStm, REFIID riid, void **ppv)
    {
        // load from the stream
        T* pT = static_cast<T*>(this);
        HRESULT hr = pT->Load(pStm);
        if (FAILED(hr)) return hr;
        return pT->QueryInterface(riid,ppv);
    }
    
    STDMETHODIMP ReleaseMarshalData(IStream *pStm)
    {
        // no-op
        return S_OK;
    }
    
    STDMETHODIMP DisconnectObject(DWORD dwReserved)
    {
        return S_OK;
    }
};

