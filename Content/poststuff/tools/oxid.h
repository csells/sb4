// oxid.h: A set of types to help find and crack oxids.
// This code is taken from a header file development by DevelopMentor instructors.
// Copyright (c) 1999, DeveloMentor.
// All rights reserved. No warranties extended.

#pragma once
#ifndef INC_OXID
#define INC_OXID

#include <assert.h>

struct Dummy :  IUnknown
{
    STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
    {
        if (riid == IID_IUnknown)
            return (*ppv = static_cast<IUnknown*>(this)), S_OK;
        else
            return (*ppv = 0), E_NOINTERFACE;
    }
    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }
};

const unsigned long OBJREF_SIGNATURE	= 0x574f454d;
const unsigned long OBJREF_STANDARD		= 0x1;  
const unsigned long OBJREF_HANDLER		= 0x2;  
const unsigned long OBJREF_CUSTOM		= 0x4;  
const unsigned long SORF_NOPING			= 0x1000;

typedef __int64 OXID;
typedef __int64 OID;
typedef GUID	IPID;

typedef struct tagDUALSTRINGARRAY    {
    unsigned short wNumEntries;     // Number of entries in array.
    unsigned short wSecurityOffset; // Offset of security info.
    unsigned short aStringArray[1];
} DUALSTRINGARRAY;

typedef struct tagSTDOBJREF    {
    unsigned long  flags;              
    unsigned long  cPublicRefs;        
    OXID           oxid;               
    OID            oid;                
    IPID           ipid;               
} STDOBJREF;

typedef struct tagOBJREF    {
    unsigned long signature;           
    unsigned long flags;               
    GUID          iid;                 
    union        {
        struct            {
            STDOBJREF       std;       
            DUALSTRINGARRAY saResAddr; 
        } u_standard;    
        struct            {
            STDOBJREF       std;       
            CLSID           clsid;     
            DUALSTRINGARRAY saResAddr; 
        } u_handler;            
        struct            {
            CLSID           clsid;     
            unsigned long   cbExtension;
            unsigned long   size;      
            byte *pData; 
        } u_custom;        
    } u_objref;    
} OBJREF;

inline HRESULT GetCurrentOXID(OXID* poxid)
{
    CComPtr<IStream>    spStm;
    HRESULT hr = CreateStreamOnHGlobal(0, FALSE, &spStm);
    if( FAILED(hr) ) return hr;

    Dummy   dummy; // Implements IUnknown only
    hr = CoMarshalInterface(spStm, IID_IUnknown, &dummy, MSHCTX_INPROC, 0, MSHLFLAGS_NORMAL);
    if( FAILED(hr) ) return hr;

    HGLOBAL hglobal = 0;
    hr = GetHGlobalFromStream(spStm, &hglobal);
    if( SUCCEEDED(hr) )
    {
        OBJREF* pobjref = (OBJREF*)GlobalLock(hglobal);
        assert(pobjref->signature == OBJREF_SIGNATURE);

        if (pobjref->flags & OBJREF_STANDARD)
            *poxid = pobjref->u_objref.u_standard.std.oxid;
        else if (pobjref->flags & OBJREF_HANDLER)
            *poxid = pobjref->u_objref.u_handler.std.oxid;
        else
            hr = E_UNEXPECTED;

        GlobalUnlock(hglobal);
    }

    LARGE_INTEGER   li = { 0 };
    spStm->Seek(li, STREAM_SEEK_SET, 0);
    CoReleaseMarshalData(spStm);
    return hr;
}

// Handy for cracking OXIDs
typedef __int64 QWORD;
inline DWORD LODWORD(const QWORD & qw) { return DWORD(qw); }
inline DWORD HIDWORD(const QWORD & qw) { return ((DWORD)((qw >> 32) & 0xFFFFFFFF)); }

enum { CCH_MAX_FOR_OXID = 22 }; // buffer size needed to format and null-terminate
enum { CCH_MAX_FOR_GUID = 39 }; // buffer size needed to format GUID (with curlies), and null-terminate

inline int StringFromOXID2(const OXID& roxid, WCHAR *pwsz, int cchMax)
{
	if (cchMax < CCH_MAX_FOR_OXID)
		return 0;
	wsprintfW(pwsz, L"{%04X-%04X-%04X-%04X}", 
			  int(HIWORD(HIDWORD(roxid))), 
			  int(LOWORD(HIDWORD(roxid))), 
			  int(HIWORD(LODWORD(roxid))), 
			  int(LOWORD(LODWORD(roxid)))); 
	return CCH_MAX_FOR_OXID;
}

#endif  // INC_OXID
