// HtmlDialogImpl.h
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000, Chris Sells
// All rights reserved.
// 2/2/00 - Initial release.
//
// NO WARRANTIES ARE EXTENDED. USE AT YOUR OWN RISK.
//
// To contact the author with suggestions or comments, use csells@sellsbrothers.com.
/////////////////////////////////////////////////////////////////////////////
// This header file contains two classes, CHtmlDialogImpl and CSimpleHtmlDialog.
// CHtmlDialogImpl is meant as a base class for modal HTML-based dialogs.
//
// These classes are just thunks to call the ShowHTMLDialog function exported
// from mshtml.dll:
//
//  HRESULT ShowHTMLDialog(HWND hwndParent, LPCOLESTR pszUrl, VARIANT* pvarArgIn, LPCOLESTR pszOptions, VARIANT* pvarArgOut);
//
// This function is what is called by the showModalDialog method of the window object
// in the HTML DOM. Check out the docs at http://msdn.microsoft.com/workshop/Author/dhtml/reference/methods/showModalDialog.asp.
//
// The hwndParent is gleaned from the parameter to DoModal. The trailing options are
// cached from the CHtmlDialogImpl constructor. The URL can come from a resource ID
// in the derived class, like so:
//
/*
    class CAboutDlg : public CHtmlDialogImpl<CAboutDlg>
    {
    public:
        enum { IDD = IDD_ABOUT };
    };
*/
//
// The URL can come dynamically from the derived class implementing GetUrl, like so:
//
/*
    class CAboutDlg : public CHtmlDialogImpl<CAboutDlg>
    {
    public:
        HRESULT GetUrl(BSTR* pbstrUrl)
        {
            *pbstrUrl = SysAllocString(OLESTR("http://www.sellsbrothers.com"));
            return S_OK;
        }
    };
*/
//
// Or, using CSimpleHtmlDialog, the URL can come from a template parameter, like so:
//
/*
typedef CSimpleHtmlDialog<IDD_ABOUT> CAboutDlg;
*/
//
// A simple HTML resource for use as a dialog is as follows:
/*
<html style="HEIGHT: 246pt; WIDTH: 328pt">
<body bgcolor=BUTTONFACE>
<table height=100% width=100%><tr><td align=center><font size=5 face="impact">
<center>
Copyright (c) Chris Sells, 1998<br>
All rights reserved.<br>
<a href="mailto:csells@sellsbrothers.com?subject=DHTML Rules!">csells@sellsbrothers.com</a>
<center>
</font></td></tr></table>
</body>
</html>
*/

#pragma once
#ifndef INC_HTMLDIALOGIMPL
#define INC_HTMLDIALOGIMPL

template <typename Deriving>
class CHtmlDialogImpl
{
public:
    CHtmlDialogImpl(VARIANT* pvarArgIn = 0, LPCOLESTR pszOptions = 0, VARIANT* pvarArgOut = 0)
        :   m_bstrOptions(pszOptions),
            m_pvarArgOut(pvarArgOut)
    {
        if( pvarArgIn ) m_varArgIn = *pvarArgIn;
    }

    int DoModal(HWND hwndParent = ::GetActiveWindow())
    {
        HRESULT     hr = S_OK;

        CComBSTR    bstrUrl;
        hr = GetStaticOrDynamicUrl(&bstrUrl);
        if( FAILED(hr) ) return IDABORT;

        hr = ShowHTMLDialog(hwndParent, bstrUrl, &m_varArgIn, m_bstrOptions, m_pvarArgOut);
        if( FAILED(hr) ) return IDABORT;

        return (hr == S_OK ? IDOK : IDCANCEL);
    }

public:
    static
    HRESULT ShowHTMLDialog(HWND hwndParent, LPCOLESTR pszUrl, VARIANT* pvarArgIn = 0, LPCOLESTR pszOptions = 0, VARIANT* pvarArgOut = 0)
    {
        // SHOWHTMLDIALOGFN is typedef'd incorrectly
        typedef HRESULT (STDAPICALLTYPE *PFNSHOWHTMLDIALOG)(HWND, IMoniker*, VARIANT*, LPCOLESTR, VARIANT*);
        PFNSHOWHTMLDIALOG   pfnShowHTMLDialog = 0;
        
        // So long as I'm loading mshtml.dll, might as well load urlmon.dll, too
        typedef HRESULT (STDAPICALLTYPE *PFNMKPARSEDISPLAYNAMEEX)(IBindCtx*, LPCOLESTR, ULONG*, IMoniker**);
        PFNMKPARSEDISPLAYNAMEEX pfnMkParseDisplayNameEx = 0;
        
        HINSTANCE   hinstMshtml = 0;
        HINSTANCE   hinstUrlmon = 0;
        IBindCtx*   pbc = 0;
        IMoniker*   pmk = 0;
        ULONG       cch = 0;
        HRESULT     hr = S_OK;
        
        // Load the libraries and get the two functions I need
        if( !(hinstMshtml = LoadLibrary(__T("mshtml.dll"))) ||
            !(pfnShowHTMLDialog = (PFNSHOWHTMLDIALOG)GetProcAddress(hinstMshtml, "ShowHTMLDialog")) ||
            !(hinstUrlmon = LoadLibrary(__T("urlmon.dll"))) ||
            !(pfnMkParseDisplayNameEx = (PFNMKPARSEDISPLAYNAMEEX)GetProcAddress(hinstUrlmon, "MkParseDisplayNameEx")))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
        
        // Do the moniker stuff and show the dialog
        if( SUCCEEDED(hr) &&
            SUCCEEDED(hr = CreateBindCtx(0, &pbc)) &&
            SUCCEEDED(hr = pfnMkParseDisplayNameEx(pbc, pszUrl, &cch, &pmk)) )
        {
            hr = pfnShowHTMLDialog(hwndParent, pmk, pvarArgIn, pszOptions, pvarArgOut);
        }
        
        // Clean up, clean up, everybody do their share...
        if( hinstMshtml ) FreeLibrary(hinstMshtml);
        if( hinstUrlmon ) FreeLibrary(hinstUrlmon);
        if( pbc ) pbc->Release();
        if( pmk ) pmk->Release();
        
        return hr;
    }

    static
    HRESULT UrlFromResourceID(UINT id, BSTR* pbstrUrl)
    {
        ATLASSERT(pbstrUrl);

        TCHAR szModule[_MAX_PATH];
        GetModuleFileName(_Module.GetModuleInstance(), szModule, _MAX_PATH);
    
        CComBSTR bstrUrl(OLESTR("res://"));
        bstrUrl.Append(szModule);
        bstrUrl.Append(OLESTR("/"));
        TCHAR szResID[11];
        wsprintf(szResID, _T("%0d"), id);
        bstrUrl.Append(szResID);

        *pbstrUrl = bstrUrl.Detach();
        return S_OK;
    }

protected:
    enum { IDD = 0 };

    HRESULT GetStaticOrDynamicUrl(BSTR* pbstrUrl)
    {
        ATLASSERT(pbstrUrl);

        DWORD   dwResID = This()->IDD;
        if( dwResID ) return UrlFromResourceID(dwResID, pbstrUrl);

        return This()->GetUrl(pbstrUrl);
    }

    HRESULT GetUrl(BSTR* pbstrUrl)
    {
        // Default implementation
        ATLASSERT(pbstrUrl);
        *pbstrUrl = 0;
        return E_NOTIMPL;
    }

    Deriving* This()
    {
        return static_cast<Deriving*>(this);
    }

protected:
    CComVariant m_varArgIn;
    CComBSTR    m_bstrOptions;
    VARIANT*    m_pvarArgOut;
};

template <UINT nDlgTemplateID>
class CSimpleHtmlDialog : public CHtmlDialogImpl<CSimpleHtmlDialog>
{
public:
    HRESULT GetUrl(BSTR* pbstrUrl)
    {
        return UrlFromResourceID(nDlgTemplateID, pbstrUrl);
    }
};

#endif  // INC_HTMLDIALOGIMPL
