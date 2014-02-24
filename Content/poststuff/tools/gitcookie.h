// gitcookie.h
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1998-1999, Chris Sells
// All rights reserved.
// NO WARRANTIES ARE EXTENDED. USE AT YOUR OWN RISK.
//
// To contact the author with suggestions or comments, use csells@sellsbrothers.com.
/////////////////////////////////////////////////////////////////////////////
// 9/10/99: Removed all the global functions and updated CGitCookie to fit
//          into a single (header) file.
// 5/21/99: Added CGitCookie.
// 3/17/98: 1st release.
/////////////////////////////////////////////////////////////////////////////
// The file provides one class for use with the COM Global Interface Table.
//
// CGitCookie: Wraps the GIT cookie for an interface. It supports automatic
//             placement into the GIT, retrieval from the GIT and removal from the GIT.
//             When used with a smart point, e.g. CComPtr, provides everything
//             you need to use the GIT.
/////////////////////////////////////////////////////////////////////////////
// Usage:
/*
class CHighLander : public CComObjectRootEx...
{
public:
    HRESULT FinalConstruct() {
      CComPtr<ISword>   spSword;
      spSword.CoCreateInstance(CLSID_BigSword);

      // Get apt-neutral interface cookie
      m_gitSword = spSword;
      return S_OK;
    }

    STDMETHODIMP CHighLander::SwordFight() {
      CComPtr<ISword>   spSword;

      // Marshal interface to this apartment
      m_gitSword.GetInterface(&spSword);

      spSword->Parry();
      spSword->Thrust();
      return S_OK;
    }

private:
    CGitCookie  m_gitSword; // Cookie revoked in dtor
};
*/

#pragma once
#ifndef INC_GITCOOKIE
#define INC_GITCOOKIE

class CGitCookie
{
public:
    CGitCookie(IUnknown* punk = 0) : m_dwCookie(0)
    {
        RegisterInterface(punk);
    }

    ~CGitCookie()
    {
        RevokeInterface();
    }

    CGitCookie& operator=(IUnknown* punk)
    {
        RegisterInterface(punk);
        return *this;
    }
    
    HRESULT RegisterInterface(IUnknown* punk)
    {
        RevokeInterface();

        HRESULT hr = S_OK;
        if( punk )
        {
            hr = GetGit()->RegisterInterfaceInGlobal(punk, IID_IUnknown, &m_dwCookie);
        }

        return hr;
    }

    HRESULT GetInterface(REFIID riid, void** ppv)
    {
        if( !ppv ) return E_POINTER;
        *ppv = 0;
        if( !m_dwCookie ) return E_UNEXPECTED;

        // NOTE: Two steps to work around NT4, SP3 bug
        CComPtr<IUnknown>   spunk;
        HRESULT hr = GetGit()->GetInterfaceFromGlobal(m_dwCookie, IID_IUnknown, (void**)&spunk);
        if( SUCCEEDED(hr) ) hr = spunk->QueryInterface(riid, ppv);
        return hr;
    }

    template <typename Q>
    HRESULT GetInterface(Q** pp)
    {
        return GetInterface(__uuidof(Q), (void**)pp);
    }

    HRESULT RevokeInterface()
    {
        HRESULT hr = S_OK;
        if( m_dwCookie )
        {
            hr = GetGit()->RevokeInterfaceFromGlobal(m_dwCookie);
            m_dwCookie = 0;
        }

        return hr;
    }

    DWORD Detach()
    {
        DWORD   dwCookie = m_dwCookie;
        m_dwCookie = 0;
        return dwCookie;
    }

    void Attach(DWORD dwCookie)
    {
        RevokeInterface();
        m_dwCookie = dwCookie;
    }

    operator DWORD()
    {
        return m_dwCookie;
    }

    bool operator!()
    {
        return m_dwCookie ? true : false;
    }

public:
    static
    IGlobalInterfaceTable* GetGit()
    {
        if( !s_pgit )
        {
            HRESULT hr = CoCreateInstance(CLSID_StdGlobalInterfaceTable,
                                          0,
                                          CLSCTX_INPROC_SERVER,
                                          IID_IGlobalInterfaceTable,
                                          (void**)&s_pgit);
            _ASSERTE(hr == S_OK);
        }

        return s_pgit;
    }

private:
    DWORD   m_dwCookie;
    static  IGlobalInterfaceTable*  s_pgit;
};

// Leak the interface because the GIT is process-global
__declspec(selectany) IGlobalInterfaceTable*  CGitCookie::s_pgit = 0;

#endif  // INC_GITCOOKIE
