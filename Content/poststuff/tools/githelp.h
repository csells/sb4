// githelp.h
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1998, Chris Sells
// All rights reserved.
// 3/17/98: 1st release.
//
// NO WARRANTIES ARE EXTENDED. USE AT YOUR OWN RISK.
//
// To contact the author with suggestions or comments, use csells@sellsbrothers.com.
/////////////////////////////////////////////////////////////////////////////
// This file global wrapper functions around each of the IGlobalInterfaceTable
// methods. It uses a static variable held the global function CoGetGit. Since
// CoGetGit should have exactly one implementation, the githelp.cpp file must
// be included in exactly one place, e.g. in stdafx.cpp.
//
// HRESULT CoGetGit(IGlobalInterfaceTable** ppgit);
//  Accesses the single instance of the GIT.
//
// HRESULT CoRegisterInterfaceInGlobal(IUnknown* punk, REFIID riid, DWORD* pdwCookie);
//  Global wrapper for IGlobalInterfaceTable::RegisterInterfaceInGlobal.
//
// HRESULT CoRevokeInterfaceFromGlobal(DWORD dwCookie);
//  Global wrapper for IGlobalInterfaceTable::RevokeInterfaceFromGlobal.
//
// HRESULT CoGetInterfaceFromGlobal(DWORD dwCookie, REFIID riid, void** ppv);
//  Global wrapper for IGlobalInterfaceTable::GetInterfaceFromGlobal.
//
// HRESULT CoMarshalInterThreadInterfaceInGIT(REFIID riid, IUnknown* punk, DWORD* pdwCookie);
//  Marshals an interface into the GIT ala CoMarshalInterThreadInterfaceInStream.
//
// HRESULT CoGetInterfaceAndRevokeFromGIT(DWORD dwCookie, REFIID riid, void** ppv);
//  Unmarshals interface from the GIT and removes it ala CoGetInterfaceAndReleaseStream.
//
// Usage:
/*
HRESULT SpawnThread()
{
    IMouse* pm = 0;
    HRESULT hr = CoCreateInstance(CLSID_Mouse, 0, CLSCTX_INPROC_SERVER, IID_IMouse, (void**)&pm);
    if( SUCCEEDED(hr) )
    {
        // DONE: Marshalled itf into GIT
        DWORD   dwCookie;
        hr = CoMarshalInterThreadInterfaceInGIT(IID_IMouse, pm, &dwCookie);
        if( SUCCEEDED(hr) )
        {
            DWORD   tid;
            HANDLE  hThread = CreateThread(0, 0, ThreadProc, (void*)dwCookie, 0, &tid);

            if( hThread ) CloseHandle(hThread);
            // DONE: If thread doesn't start, remove itf from GIT
            else hr = CoRevokeInterfaceFromGlobal(dwCookie);
        }
        pm->Release();
    }
    return hr;
}

DWORD WINAPI ThreadProc(void* pv)
{
    CoInitialize(0);

    // DONE: Pull pointer out of GIT
    DWORD   dwCookie = (DWORD)pv;
    IMouse* pm = 0;
    HRESULT hr = CoGetInterfaceAndRevokeFromGIT(dwCookie, IID_IMouse, (void**)&pm);
    if( SUCCEEDED(hr) )
    {
        // Use pm
        pm->Release();
    }

    CoUninitialize();
    return hr;
}
*/

#pragma once

// IGlobalInterfaceTable wrappers
HRESULT CoGetGit(IGlobalInterfaceTable** ppgit);
HRESULT CoRegisterInterfaceInGlobal(IUnknown* punk, REFIID riid, DWORD* pdwCookie);
HRESULT CoRevokeInterfaceFromGlobal(DWORD dwCookie);
HRESULT CoGetInterfaceFromGlobal(DWORD dwCookie, REFIID riid, void** ppv);

// Single marshal helpers
HRESULT CoMarshalInterThreadInterfaceInGIT(REFIID riid, IUnknown* punk, DWORD* pdwCookie);
HRESULT CoGetInterfaceAndRevokeFromGIT(DWORD dwCookie, REFIID riid, void** ppv);

// Inline implementations
inline
HRESULT CoRegisterInterfaceInGlobal(IUnknown* punk, REFIID riid, DWORD* pdwCookie)
{
    *pdwCookie = 0;

    IGlobalInterfaceTable*  pgit = 0;
    HRESULT                 hr = CoGetGit(&pgit);
    if( SUCCEEDED(hr) )
    {
        hr = pgit->RegisterInterfaceInGlobal(punk, riid, pdwCookie);
        pgit->Release();
    }

    return hr;
}

inline
HRESULT CoRevokeInterfaceFromGlobal(DWORD dwCookie)
{
    IGlobalInterfaceTable*  pgit = 0;
    HRESULT                 hr = CoGetGit(&pgit);
    if( SUCCEEDED(hr) )
    {
        hr = pgit->RevokeInterfaceFromGlobal(dwCookie);
        pgit->Release();
    }

    return hr;
}

inline
HRESULT CoGetInterfaceFromGlobal(DWORD dwCookie, REFIID riid, void** ppv)
{
    IGlobalInterfaceTable*  pgit = 0;
    HRESULT                 hr = CoGetGit(&pgit);
    if( SUCCEEDED(hr) )
    {
        hr = pgit->GetInterfaceFromGlobal(dwCookie, riid, ppv);
        pgit->Release();
    }

    return hr;
}

inline
HRESULT CoMarshalInterThreadInterfaceInGIT(REFIID riid, IUnknown* punk, DWORD* pdwCookie)
{
    return CoRegisterInterfaceInGlobal(punk, riid, pdwCookie);
}

inline
HRESULT CoGetInterfaceAndRevokeFromGIT(DWORD dwCookie, REFIID riid, void** ppv)
{
    HRESULT hr = CoGetInterfaceFromGlobal(dwCookie, riid, ppv);
    if( SUCCEEDED(hr) )
    {
        hr = CoRevokeInterfaceFromGlobal(dwCookie);
        if( FAILED(hr) )
        {
            char    sz[32];
            wsprintf(sz, "Revoking itf failed: 0x%x.\n", hr);
            OutputDebugString(sz);

            // Coerce hr to success
            hr = S_OK;
        }
    }

    return hr;
}
