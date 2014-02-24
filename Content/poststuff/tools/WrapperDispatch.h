// WrapperDispatch.h
// Copyright (c) 1999, Chris Sells
// All rights reserved. No warrenties extended.
/////////////////////////////////////////////////////////////////////////////
// The ATL Object Wizard generated an HTML Control that sets it's external
// dispatch with a simple static_cast:
//
//  hr = wnd.SetExternalDispatch(static_cast<IMyHtmlControlUI*>(this));
//
// This gives a non-default IDispatch to IE to use as the window.external
// objref. Under IE4, this works fine because IE4 never QIs this objref.
// However, IE5 *does* QI(IDispatch) on window.external, yielding the
// control's default IDispatch, i.e. IMyHtmlControl. The CWrapperDispatch
// class defined in this file provides a new COM identity to wrap around
// a non-default IDispatch so that QI will not yield another interface on the
// main object.
/////////////////////////////////////////////////////////////////////////////
// Known Issues:
// The WrapperDispatch object only implements IDispatch. It does so by
// forwarding calls to the non-default IDispatch that is provided at
// creation type. However, if you'd like to QI the window.external for any
// other interface that the control implements, you can't. The best way
// around this problem is to provide [propget] methods on the IMyHtmlControlUI
// interface that exposes other interfaces on the actual control. Or even
// provide a general purpose Object property for this purpose.
/////////////////////////////////////////////////////////////////////////////
// Usage:
/*
LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    HRESULT     hr = S_OK;
    CAxWindow   wnd(m_hWnd);

    // Create the control
    hr = wnd.CreateControl(<%= IDH_HTMLUIID %>);
    if( FAILED(hr) ) return -1;

    // Create a wrapper about the external dispatch interface
    CComObject<CWrapperDispatch>*   spdispWrapper = 0;
    hr = CComObject<CWrapperDispatch>::CreateInstance(&spdispWrapper);
    if( FAILED(hr) ) return -1;
    
    // Dummy for refcount management
    CComPtr<IUnknown> spUnk = spdispWrapper;

    // Set the external dispatch interface
    CComQIPtr<<%= InterfaceName %>UI> spdispExternal(GetUnknown());
    if( !spdispExternal ) return -1;
    spdispWrapper->SetDispatch(spdispExternal);

    hr = wnd.SetExternalDispatch(spdispWrapper);
    if( FAILED(hr) ) return -1;

    hr = wnd.QueryControl(IID_IWebBrowser2, (void**)&m_spBrowser);
    if( FAILED(hr) ) return -1;
    
    return 0;
}
*/

#pragma once
#ifndef __WRAPPERDISPATCH_H__
#define __WRAPPERDISPATCH_H__

template <typename Deriving>
class CWrapperDispatchT :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CWrapperDispatchT>,
    public IDispatch
{
public:
    static
    HRESULT CreateDispatchWrapper(IDispatch* pdispToWrap, IDispatch** ppdispWrapper)
    {
        if( !ppdispWrapper ) return E_POINTER;
        *ppdispWrapper = 0;

        CComObject<Deriving>*   pdispWrapper;
        HRESULT hr = CComObject<Deriving>::CreateInstance(&pdispWrapper);
        if( FAILED(hr) ) return hr;
        
        pdispWrapper->SetDispatch(pdispToWrap);
        (*ppdispWrapper = pdispWrapper)->AddRef();
        return S_OK;
    }

public:
    void SetDispatch(IDispatch* pdisp)
    {
        m_spdisp = pdisp;   // NOTE: No QI
    }

DECLARE_NO_REGISTRY()
BEGIN_COM_MAP(CWrapperDispatchT)
    COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IDispatch
    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo)
    {
        if( !m_spdisp ) return E_UNEXPECTED;
        return m_spdisp->GetTypeInfoCount(pctinfo);
    }

	STDMETHODIMP GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
    {
        if( !pptinfo ) return E_POINTER;
        *pptinfo = 0;
        if( !m_spdisp ) return E_UNEXPECTED;
        return m_spdisp->GetTypeInfo(itinfo, lcid, pptinfo);
    }

	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid)
    {
        if( !m_spdisp ) return E_UNEXPECTED;
        return m_spdisp->GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);
    }

	STDMETHODIMP Invoke(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, EXCEPINFO* pexcepinfo, UINT* puArgErr)
    {
        if( !m_spdisp ) return E_UNEXPECTED;
        return m_spdisp->Invoke(dispidMember, riid, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);
    }

protected:
    CComPtr<IDispatch>  m_spdisp;
};

class CWrapperDispatch : public CWrapperDispatchT<CWrapperDispatch> {};

#endif  // __WRAPPERDISPATCH_H__
