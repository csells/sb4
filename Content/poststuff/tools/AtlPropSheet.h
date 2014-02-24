// AttilaPropSheet.h: CPropertySheetImpl and CPropertyPageImpl et al
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1999-2000, DevelopMentor.
// All rights reserved. No warranties extended.
/////////////////////////////////////////////////////////////////////////////
// History:
//  10/24/00:
//  -Define PSM_RECALCPAGESIZES if PSDK isn't used, so pages can resize under
//   IE5.
//
//  10/19/00:
//  -Allow modeless property sheets to kill activate.
//  -NOTE: Introduced a bug where OnKillActive is called twice when it
//         returns FALSE.
//
//  7/24/00:
//  -Stopped calling SetWizardButtons when the sheet isn't a wizard,
//   as it sets the default button to Cancel from OK.
//
//  7/7/00:
//  -Automatically enable/disable Back/Next button based on page position.
//  -Enable Close button for modeless property sheets.
//
//  3/2/00:
//  -Fake PSN_KILLACTIVE when faking PSN_APPLY for active page.
//
//  1/27/00:
//  -Fake PSN_APPLY when the Finish button is pressed. This makes it be really
//   important for the derived class to chain the message map to the base class.
//   This allows the page developer to unify development for both property sheet
//   and wizard property pages. If you want this to work, you must chain to the
//   base message map in the property page base class, and if you handle
//   WM_QUERYSIBLINGS, you must set bHandled = FALSE for unhandled queries.
//
//  -Fixed a nasty bug where PSN_APPLY wasn't sent to a modeless property sheet,
//   making it really hard to host COM property pages.
//
//  12/8/99:
//  -Updated to support building a replacement for OleCreatePropertyFrame.
/////////////////////////////////////////////////////////////////////////////
// Usage:
/*

class CFirstPage : public CPropertyPageImpl<CFirstPage>
{
public:
	BEGIN_MSG_MAP(CFirstPage)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
        CHAIN_MSG_MAP(CPropertyPageImpl<CFirstPage>)
	END_MSG_MAP()
	enum { IDD = IDD_FIRST_PAGE };

	LRESULT OnLButtonDown(UINT, WPARAM, LPARAM, BOOL&)
	{
		MessageBox("Ouch!");
		return 0;
	}
};

class CSecondPage : public CPropertyPageImpl<CSecondPage>
{
public:
	BEGIN_MSG_MAP(CSecondPage)
        CHAIN_MSG_MAP(CPropertyPageImpl<CSecondPage>)
	END_MSG_MAP()
	enum { IDD = IDD_SECOND_PAGE };
};

class CMyPropertySheet : public CPropertySheetImpl<CMyPropertySheet>
{
public:
	typedef CPropertySheetImpl<CMyPropertySheet> baseClass;
	CMyPropertySheet() : baseClass("My Properties") {}

    BEGIN_MSG_MAP(CMyPropertySheet)
        CHAIN_MSG_MAP(CPropertySheetImpl<CMyPropertySheet>)
    END_MSG_MAP()

	BEGIN_PROPPAGE_MAP(CMyPropertySheet)
	    PROPPAGE_ENTRY(m_page1)
	    PROPPAGE_ENTRY(m_page2)
	END_PROPPAGE_MAP()

private:
	CFirstPage  m_page1;
	CSecondPage m_page2;
};

LRESULT CMyFrameWindow::OnShowProperties()
{
	CMyPropertySheet sheet("My Properties");
	if( sheet.DoModal() == IDOK ) MessageBox("Ok!");
	return 0;
}

*/

#pragma once
#ifndef INC_ATLPROPSHEET
#define INC_ATLPROPSHEET

#include <commctrl.h>
#include <prsht.h>
#pragma comment(lib, "comctl32.lib")

#ifndef _ATL_NO_HOSTING
#include <atlhost.h>
#endif

#define BEGIN_PROPPAGE_MAP(_baseClass) \
	int GetPropertyPages(HPROPSHEETPAGE *hpp) { \
		int nPages = 0;

// TODO: How to manage errors, i.e. page creation failed?

#define PROPPAGE_ENTRY(_page) \
		if (hpp) hpp[nPages] = _page.CreatePropertySheetPage(); \
		nPages++;

#define END_PROPPAGE_MAP() \
		return nPages; \
	}

// Sent via PSM_QUERYSIBLINGS to indicate to all pages
// that the Finish button was pressed
#define PSN_APPLYFINISH PSN_LAST

#ifndef PSM_RECALCPAGESIZES
// From the PSDK headers in case we're being compiled against VC headers
#define PSM_RECALCPAGESIZES        (WM_USER + 136)
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetT - client side for a Windows property sheet

template <typename Base>
class CPropertySheetT : public Base
{
public:
// Constructors
	CPropertySheetT(HWND hWnd = NULL) : Base(hWnd) { }

	CPropertySheetT< Base >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	// Property Page Helpers
    bool IsWizard()
    {
		ATLASSERT(::IsWindow(m_hWnd));

        // Hueristically, if the tab control is disabled, this is a wizard
        // NOTE: Strangely enough, the tab control still exists for a 
        //       property sheet *and* is visible...
        CWindow wndTabControl = GetTabControl();
        return ((wndTabControl.GetStyle() & WS_DISABLED) != 0);
    }

	void AddPage(HPROPSHEETPAGE hpage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_ADDPAGE, 0, reinterpret_cast<LPARAM>(hpage));
	}

	template <typename TPage>
	void AddPage(TPage * page)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_ADDPAGE, 0, reinterpret_cast<LPARAM>(page->GetPropertySheetPage(true)));
        RecalcPageSizes();
	}

	void RemovePage(HPROPSHEETPAGE hpage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_REMOVEPAGE, 0, reinterpret_cast<LPARAM>(hpage));
	}

	void RemovePage(int index)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_REMOVEPAGE, index, 0);
	}

	template <typename TPage>
	void RemovePage(TPage * page)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_REMOVEPAGE, 0, reinterpret_cast<LPARAM>(page->GetPropertySheetPage()));
	}

	BOOL SetCurSel(int index)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, PSM_SETCURSEL, index, 0);
	}

	BOOL SetCurSel(HPROPSHEETPAGE hpage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, PSM_SETCURSEL, 0, reinterpret_cast<LPARAM>(hpage));
	}

	template <typename TPage>
	BOOL SetCurSelT(TPage * page)
	{
		return SetCurSel(page->GetPropertySheetPage());
	}

	BOOL SetCurSelId(UINT id)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (BOOL)::SendMessage(m_hWnd, PSM_SETCURSELID, 0, (LPARAM)id);
	}

	void Changed(HWND hwndPage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(::IsWindow(hwndPage));
		::SendMessage(m_hWnd, PSM_CHANGED, reinterpret_cast<WPARAM>(hwndPage), 0L);
	}

	void Unchanged(HWND hwndPage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(::IsWindow(hwndPage));
		::SendMessage(m_hWnd, PSM_UNCHANGED, reinterpret_cast<WPARAM>(hwndPage), 0L);
	}

	BOOL IsDialogMessage(const MSG *msg)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, PSM_ISDIALOGMESSAGE, 0, reinterpret_cast<LPARAM>(msg));
	}

	void RestartWindows()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_RESTARTWINDOWS, 0, 0L);
	}

	void RebootSystem()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_REBOOTSYSTEM, 0, 0L);
	}

	void CancelToClose()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::PostMessage(m_hWnd, PSM_CANCELTOCLOSE, 0, 0L);
	}

	int QuerySiblings(WPARAM wparam, LPARAM lparam)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, PSM_QUERYSIBLINGS, wparam, lparam);
	}

	BOOL Apply()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_APPLY, 0, 0);
	}

	void SetTitle(DWORD dwStyle, LPCTSTR pszTitle)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_SETTITLE, dwStyle, reinterpret_cast<LPARAM>(pszTitle));
	}

	void SetTitle(DWORD dwStyle, UINT nTitleID)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_SETTITLE, dwStyle, MAKEINTRESOURCE(nTitleID));
	}

	void SetWizButtons(DWORD dwFlags)
	{
		ATLASSERT(::IsWindow(m_hWnd));
        ATLASSERT(IsWizard());  // Sets Cancel to default button if called on a prop-sheet
		::PostMessage(m_hWnd, PSM_SETWIZBUTTONS, 0, (LPARAM)dwFlags);
	}

	void PressButton(int nButton)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::PostMessage(m_hWnd, PSM_PRESSBUTTON, (WPARAM)nButton, 0);
	}

	void SetFinishText(LPCTSTR pszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_SETFINISHTEXT, 0, reinterpret_cast<LPARAM>(pszText));
	}

	HWND GetTabControl()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return reinterpret_cast<HWND>(::SendMessage(m_hWnd, PSM_GETTABCONTROL, 0, 0));
	}

	HWND GetCurrentPageHwnd()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return reinterpret_cast<HWND>(::SendMessage(m_hWnd, PSM_GETCURRENTPAGEHWND, 0, 0));
	}

    // NOTE: This will only work under IE5
	BOOL RecalcPageSizes()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, PSM_RECALCPAGESIZES, 0, 0);
	}

#if (_WIN32_IE >= 0x0500)
	int GetResult()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_GETRESULT, 0, 0L);
	}

	int HwndToIndex(HWND hwndPage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(::IsWindow(hwndPage));
		return ::SendMessage(m_hWnd, PSM_HWNDTOINDEX, reinterpret_cast<WPARAM>(hwndPage), 0);
	}

	int IdToIndex(UINT id)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, PSM_IDTOINDEX, 0, (LPARAM)id);
	}

	int IndexToHwnd(int index)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		ATLASSERT(::IsWindow(hwndPage));
		return reinterpret_cast<HWND>(::SendMessage(m_hWnd, PSM_INDEXTOHWND, index, 0));
	}

	HPROPSHEETPAGE IndexToPage(int index)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return reinterpret_cast<HPROPSHEET>(::SendMessage(m_hWnd, PSM_INDEXTOPAGE, index, 0));
	}

	void InsertPage(int insertAfter, HPROPSHEETPAGE hpage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_INSERTPAGE, insertAfter, reinterpret_cast<LPARAM>(hpage));
	}

	template <typename TPage>
	void InsertPage(int insertAfter, TPage * page)
	{
		InsertPage(insertAfter, page->GetPropertySheetPage(true));
	}

	void InsertPage(HPROPSHEETPAGE insertAfter, HPROPSHEETPAGE hpage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_INSERTPAGE, reinterpret_cast<WPARAM>(hpage), reinterpret_cast<LPARAM>(hpage));
	}

	template <typename TPage1, typename TPage2>
	void InsertPage(TPage1 * insertAfter, TPage2 * page)
	{
		InsertPage(insertAfter->GetPropertySheetPage(), page->GetPropertySheetPage(true));
	}

	int PageToIndex(HPROPSHEETPAGE hpage)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, PSM_PAGETOINDEX, 0, reinterpret_cast<LPARAM>(hpage));
	}

	template <typename TPage>
	int PageToIndex(TPage * page)
	{
		return PageToIndex(page->GetPropertySheetPage());
	}

	void SetHeaderSubtitle(int index, LPCTSTR pszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, PSM_SETHEADERSUBTITLE, index, reinterpret_cast<LPARAM>(pszText));
	}

#endif // 0x0500
};

typedef CPropertySheetT<CWindow>     CPropertySheet;

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageImpl

template <typename TDeriving, typename TBase = CWindow>
class CPropertyPageImpl :
	public PROPSHEETPAGE,
	public CDialogImplBaseT<TBase>
{
public:
	CPropertyPageImpl()
	{
		ZeroMemory(this, sizeof(PROPSHEETPAGE));
		dwSize = sizeof(PROPSHEETPAGE);
		hInstance = _Module.GetModuleInstance();
		pszTemplate = MAKEINTRESOURCE(TDeriving::IDD);
		pfnDlgProc = reinterpret_cast<DLGPROC>(TDeriving::StartDialogProc);
		dwFlags = PSP_DEFAULT;
		m_hpsp = NULL;
	}

	BEGIN_MSG_MAP(CPropertyPageImpl)
        MESSAGE_HANDLER(PSM_QUERYSIBLINGS, OnQuerySiblings)
        NOTIFY_CODE_HANDLER(PSN_SETACTIVE, OnSetActive)
	END_MSG_MAP()

	static
	UINT CALLBACK PageCallback(HWND hwnd, UINT uMsg, PROPSHEETPAGE* ppsp)
	{
		TDeriving * pThis = reinterpret_cast<TDeriving*>(ppsp->lParam);
		if (uMsg == PSPCB_CREATE)
		{
			_Module.AddCreateWndData(&pThis->m_thunk.cd, pThis);
		}
		else if (uMsg == PSPCB_RELEASE)
		{
			pThis->OnPageReleased();
		}

		return 1;
	}

	static
    LRESULT CALLBACK StartDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        TDeriving*  pThis = reinterpret_cast<TDeriving*>(_Module.ExtractCreateWndData());
        ATLASSERT(pThis != NULL);
        pThis->m_hWnd = hWnd;
        pThis->m_thunk.Init(pThis->GetDialogProc(), pThis);
        WNDPROC pProc = (WNDPROC)&(pThis->m_thunk.thunk);
        WNDPROC pOldProc = (WNDPROC)::SetWindowLong(hWnd, DWL_DLGPROC, (LONG)pProc);
#ifdef _DEBUG
        // check if somebody has subclassed us already since we discard it
        if(pOldProc != StartDialogProc)
            ATLTRACE2(atlTraceWindowing, 0, _T("Subclassing through a hook discarded.\n"));
#else
        pOldProc;	// avoid unused warning
#endif
        // NOTE: This is the only new line from the base class's StartDialogProc,
        //       but we need to let the property pages know when their HWNDs have
        //       been created w/ requiring deriving classes to remember to chain
        //       to the base class. Even if they did remember, WM_CREATE has already
        //       happened and WM_INITDIALOG will likely be handled in the deriving
        //       class and setting bHandled = FALSE in WM_INITDIALOG is too much to ask.
        pThis->OnPageCreated();

        return pProc(hWnd, uMsg, wParam, lParam);
    }

	HPROPSHEETPAGE CreatePropertySheetPage()
	{
		ATLASSERT(m_hpsp == NULL);

		// Set up the page
		ATLASSERT((dwFlags & PSP_USECALLBACK) == 0);
		this->dwFlags |= PSP_USECALLBACK;
		this->pfnCallback = PageCallback;
		this->lParam = reinterpret_cast<LPARAM>(static_cast<TDeriving*>(this));

		m_hpsp = ::CreatePropertySheetPage(this);
		return m_hpsp;
	}

    HPROPSHEETPAGE GetPropertySheetPage(bool bCreateIfNecessary = false)
	{
		if (!m_hpsp && bCreateIfNecessary)
		{
			TDeriving * pThis = static_cast<TDeriving*>(this);
			return pThis->CreatePropertySheetPage();
		}

		return m_hpsp;
	}

    void OnPageCreated()
    {
    }

	void OnPageReleased()
	{
		m_hpsp = NULL;
		dwFlags &= ~PSP_USECALLBACK;
	}

    LRESULT OnQuerySiblings(UINT, WPARAM wp, LPARAM, BOOL& bHandled)
    {
        // The user pressed the Finish button, so fake a PSN_APPLY message
        if( wp == PSN_APPLYFINISH )
        {
            bool    bActivePage = CPropertySheet(GetParent()).GetCurrentPageHwnd() == m_hWnd;

            // If we're the active page, we have to fake a PSN_KILLACTIVE first
            if( bActivePage )
            {
                PSHNOTIFY   notify = { { GetParent(), 0, PSN_KILLACTIVE } };
                if( SendMessage(WM_NOTIFY, 0, (LPARAM)&notify) &&
                    (GetWindowLong(DWL_MSGRESULT) == TRUE) )
                {
                    // Tell the caller know that the active page would like to stay active
                    return -1;
                }
            }

            // Fake PSN_APPLY
            PSHNOTIFY   notify = { { GetParent(), 0, PSN_APPLY} };
            if( SendMessage(WM_NOTIFY, 0, (LPARAM)&notify) &&
                (GetWindowLong(DWL_MSGRESULT) == PSNRET_INVALID_NOCHANGEPAGE) )
            {
                // Tell the caller know that a page would like more input
                return (bActivePage ? -1 : (LRESULT)m_hpsp);
            }

        }

        // Let everyone play
        bHandled = FALSE;
        return FALSE;
    }

    LRESULT OnSetActive(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
    {
        // Enable/Disable Back/Next button
        CPropertySheet  sheet = GetParent();
        CWindow         wndTabControl = sheet.GetTabControl();
        int             nIndex = wndTabControl.SendMessage(TCM_GETCURSEL);
        int             nTotal = wndTabControl.SendMessage(TCM_GETITEMCOUNT);

        DWORD   buttons = 0;
        if( nIndex > 0 ) buttons |= PSWIZB_BACK;
        if( nIndex < nTotal - 1 ) buttons |= PSWIZB_NEXT;

        if( sheet.IsWizard() )
        {
            // If we set this w/o checking if we're a wizard, first
            // we make Cancel the default button -- weird.
            sheet.SetWizButtons(buttons);
        }

        // Let everyone play
        bHandled = FALSE;
        return 0;   // Accept the activation
    }

protected:
	HPROPSHEETPAGE m_hpsp;
};

#ifndef _ATL_NO_HOSTING

template <typename TDeriving, typename TBase = CWindow>
class CAxPropertyPageImpl : public CPropertyPageImpl<TDeriving, TBase>
{
public:
	CAxPropertyPageImpl()
	{
		m_hDlgRsrc = NULL;
		m_hInitData = NULL;
		m_hDlgCopy = NULL;
	}

    HPROPSHEETPAGE CreatePropertySheetPage()
	{
		ATLASSERT(m_hpsp == NULL);

		// Set up the page
		ATLASSERT((dwFlags & (PSP_USECALLBACK | PSP_DLGINDIRECT)) == 0);
		this->dwFlags |= PSP_USECALLBACK | PSP_DLGINDIRECT;
		this->pfnCallback = PageCallback;
		this->lParam = reinterpret_cast<LPARAM>(static_cast<TDeriving*>(this));

		AtlAxWinInit();

		HRSRC hDlg = ::FindResource(hInstance, pszTemplate, (LPTSTR)RT_DIALOG);
		if (hDlg)
		{
			HRSRC hDlgInit = ::FindResource(hInstance, pszTemplate, (LPTSTR)_ATL_RT_DLGINIT);
			m_hInitData = NULL;
			BYTE* pInitData = NULL;

			if (hDlgInit)
			{
				m_hInitData = ::LoadResource(hInstance, hDlgInit);
				pInitData = reinterpret_cast<BYTE*>(::LockResource(m_hInitData));
			}
			m_hDlgRsrc = LoadResource(hInstance, hDlg);
			DLGTEMPLATE* pDlg = reinterpret_cast<DLGTEMPLATE*>(::LockResource(m_hDlgRsrc));
			pResource = _DialogSplitHelper::SplitDialogTemplate(pDlg, pInitData);
			if (pResource != pDlg)
			{
				m_hDlgCopy = GlobalHandle(pResource);
			}

            m_hpsp = ::CreatePropertySheetPage(this);
			if (!m_hpsp)
			{
				OnPageReleased();
			}
		}
		return m_hpsp;
	}

	void OnPageReleased()
	{
		ATLASSERT(m_hDlgRsrc);
		UnlockResource(m_hDlgRsrc);
		FreeResource(m_hDlgRsrc);
		m_hDlgRsrc = NULL;

		if (m_hInitData)
		{
			UnlockResource(m_hInitData);
			FreeResource(m_hInitData);
			m_hInitData = NULL;
		}

		if (m_hDlgCopy)
		{
			GlobalFree(m_hDlgCopy);
			m_hDlgCopy = NULL;
		}

		dwFlags &= ~PSP_DLGINDIRECT;
		pszTemplate = MAKEINTRESOURCE(TDeriving::IDD);
		CPropertyPageImpl<TDeriving, TBase>::OnPageReleased();
	}

protected:
	HGLOBAL m_hDlgRsrc;
	HGLOBAL m_hInitData;
	HGLOBAL m_hDlgCopy;
};

#endif  // _ATL_NO_HOSTING

// Simple DLGTEMPLATE with no menu, no special class, no title and
// a font hard-coded to the default property sheet font.
#pragma pack(push, 2)   // WORD aligned
struct PAGETEMPLATE
{
	DLGTEMPLATE dlg;
    WORD        menu;
    WORD        windowClass;
    WORD        title;
	WORD        pointsize;
    WCHAR       typeface[32];

    PAGETEMPLATE()
    {
        ZeroMemory(this, sizeof(*this));

        // NOTE: This is a hack to make the dialog unit calculation work
        //       propertly, as shown empirically. It would be better to
        //       do this by getting the font information from the property
        //       sheet itself (as the window size is calculated by the sheet
        //       by calling MapDialogRect using the sheet's HWND on all of the
        //       pages sizes in Dialog Units), but that hasn't yet been
        //       implemented.
        // MS Shell Dlg is an entry in HKLM/SOFTWARE/Microsoft/Windows NT/CurrentVersion/FontSubstitutes.
        // Under both NT4 and W2K, it aliases to MS Sans Serif.
		lstrcpyW(typeface, L"MS Shell Dlg");
		pointsize = 8;

        dlg.style = DS_CONTROL | WS_CHILD | WS_DISABLED | WS_CAPTION | DS_SETFONT;
        dlg.dwExtendedStyle = WS_EX_CONTROLPARENT;
    }
};
#pragma pack(pop)

template <typename TDeriving, const CLSID* pclsid = &CLSID_NULL, typename TBase = CWindow>
class CPropertyPageSiteImpl :
    public CPropertyPageImpl<TDeriving, TBase>,
    public IPropertyPageSite
{
    typedef CPropertyPageImpl<TDeriving, TBase> _baseClass;

public:
    CPropertyPageSiteImpl()
    :   m_clsid(*pclsid), m_cObjects(0), m_rgspunkObjects(0), m_lcid(GetThreadLocale())
    {
    }

    ~CPropertyPageSiteImpl()
    {
        Destroy();
    }

    void SetPageInfo(
        REFCLSID            clsid,
        IUnknown*           punkContainer,
        ULONG               nObjects,
        IUnknown**          rgpunkObjects,
        LCID                lcid = GetThreadLocale())
    {
        // Can't set objects *after* page has been shown, but we can clear them
        ATLASSERT(!m_spPage || !nObjects);

        m_spunkContainer = punkContainer;
        m_clsid = clsid;
        m_lcid = lcid;

        // Delete existing objects
        delete[] m_rgspunkObjects;
        m_rgspunkObjects = 0;

        // Cache new objects
        m_cObjects = nObjects;
        if( m_cObjects )
        {
            m_rgspunkObjects = new CComPtr<IUnknown>[m_cObjects];
            if( m_rgspunkObjects )
            {
                for( ULONG i = 0; i != m_cObjects; ++i )
                {
                    m_rgspunkObjects[i] = rgpunkObjects[i];
                }
            }
        }
    }

	HPROPSHEETPAGE CreatePropertySheetPage()
	{
        USES_CONVERSION;
		ATLASSERT(m_hpsp == NULL);

        // Create COM property sheet
        HRESULT hr = m_spPage.CoCreateInstance(m_clsid);
        if( FAILED(hr) ) return 0;

        hr = m_spPage->SetPageSite(this);
        if( FAILED(hr) ) return 0;

        // NOTE: The reinterpret cast is only safe if CComPtr only holds
        //       an IUnknown*. If it does, however, the cast saves us copying the data.
        CTASSERT(sizeof(CComPtr<IUnknown>) == sizeof(IUnknown*));
        if( m_cObjects ) hr = m_spPage->SetObjects(m_cObjects, reinterpret_cast<IUnknown**>(m_rgspunkObjects));
        if( FAILED(hr) ) return 0;

        PROPPAGEINFO    ppi = { 0 };
        hr = m_spPage->GetPageInfo(&ppi);
        if( FAILED(hr) ) return 0;

		// Set up the page
		ATLASSERT((dwFlags & PSP_USECALLBACK) == 0);
		dwFlags |= PSP_USECALLBACK;
		pfnCallback = PageCallback;
		lParam = reinterpret_cast<LPARAM>(static_cast<TDeriving*>(this));

        // Calculate page size in dialog units
        this->pResource = reinterpret_cast<DLGTEMPLATE*>(&m_pageTemplate.dlg);
        m_pageTemplate.dlg.cx = 4;
        m_pageTemplate.dlg.cy = 8;
		SIZE sizeTemplate;
		AtlGetDialogSize(this->pResource, &sizeTemplate);

        // Convert property page size in pixels to dialog units
		m_pageTemplate.dlg.cx = short(MulDiv(ppi.size.cx, 4, sizeTemplate.cx));
		m_pageTemplate.dlg.cy = short(MulDiv(ppi.size.cy, 8, sizeTemplate.cy)); 

        ATLASSERT((dwFlags & (PSP_DLGINDIRECT | PSP_USETITLE)) == 0);
        this->dwFlags |= PSP_DLGINDIRECT | PSP_USETITLE;
        this->pszTitle = OLE2CT(ppi.pszTitle);

        m_hpsp = ::CreatePropertySheetPage(this);
        
        CoTaskMemFree(ppi.pszTitle);
        CoTaskMemFree(ppi.pszDocString);
        CoTaskMemFree(ppi.pszHelpFile);
        
        return m_hpsp;
	}

    void OnPageCreated()
    {
        // Activate page on dialog
        RECT    rect; GetClientRect(&rect);
        HRESULT hr = m_spPage->Activate(m_hWnd, &rect, TRUE);
        if( SUCCEEDED(hr) )
        {
            hr = m_spPage->Show(SW_SHOW);
        }
    }

	void OnPageReleased()
	{
        Destroy();
        this->dwFlags &= ~PSP_DLGINDIRECT & ~PSP_USETITLE;
		CPropertyPageImpl<TDeriving, TBase>::OnPageReleased();
	}

    enum { IDD = 0 };   // We're not using a dialog resource

    BEGIN_MSG_MAP(CPropertyPageSiteImpl)
        NOTIFY_CODE_HANDLER(PSN_APPLY, OnApply)
        CHAIN_MSG_MAP(_baseClass);
    END_MSG_MAP()

    LRESULT OnApply(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
    {
        // S_OK == page not dirty
        return (DoApply() == S_OK ? PSNRET_NOERROR : PSNRET_INVALID_NOCHANGEPAGE);
    }

// IUnknown
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        if( ppv ) return E_POINTER;
        *ppv = 0;

        if( riid == IID_IUnknown || riid == IID_IPropertyPageSite )
        {
            *ppv = static_cast<IPropertyPageSite*>(this);
        }

        if( !*ppv ) return E_NOINTERFACE;
        reinterpret_cast<IUnknown*>(*ppv)->AddRef();
        return S_OK;
    }

    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }

// IPropertyPageSite
public:
    STDMETHODIMP OnStatusChange(DWORD dwFlags)
    {
    	ATLASSERT(::IsWindow(GetParent()));
    	ATLASSERT(::IsWindow(m_hWnd));
        CPropertySheet  sheet(GetParent());

        if( (dwFlags & PROPPAGESTATUS_DIRTY) || (dwFlags & PROPPAGESTATUS_VALIDATE) )
        {
            sheet.Changed(m_hWnd);
        }
        else
        {
            sheet.Unchanged(m_hWnd);
        }

        return S_OK;
    }

    STDMETHODIMP GetLocaleID(LCID* pLocaleID)
    {
        if( !pLocaleID ) return E_POINTER;
    	*pLocaleID = m_lcid;
        return S_OK;
    }

    STDMETHODIMP GetPageContainer(IUnknown** ppunk)
    {
        return m_spunkContainer.CopyTo(ppunk);
    }

    STDMETHODIMP TranslateAccelerator(MSG* pMsg)
    {
        // TODO
        ATLASSERT(FALSE && "Who's calling this function?!?");
        return S_OK;
    }

protected:
    void Destroy()
    {
        if( m_spPage )
        {
            // If we're not here, neither is any child property page
            if( m_hWnd ) m_spPage->Show(SW_HIDE);

            m_spPage->Deactivate();
            m_spPage.Release();
        }

        m_clsid = CLSID_NULL;
        m_cObjects = 0;
        delete[] m_rgspunkObjects;
        m_rgspunkObjects = 0;
        m_spunkContainer.Release();
    }

protected:
    CLSID                       m_clsid;
    CComPtr<IPropertyPage>      m_spPage;
    ULONG                       m_cObjects;
    CComPtr<IUnknown>*          m_rgspunkObjects;
    PAGETEMPLATE                m_pageTemplate;
    CComPtr<IUnknown>           m_spunkContainer;
    LCID                        m_lcid;

protected:
    HRESULT DoApply()
    {
        HRESULT hr = S_OK;
        if( m_spPage->IsPageDirty() == S_OK )
        {
            hr = m_spPage->Apply();
            if( hr != S_OK )
            {
                // Check if page is still dirty
                if( m_spPage->IsPageDirty() != S_OK ) hr = S_OK;
            }
        }

        return hr;
    }

private:
    // Not allowed
    CPropertyPageSiteImpl(const CPropertyPageSiteImpl&);
    CPropertyPageSiteImpl& operator=(const CPropertyPageSiteImpl&);
};

template <typename TDeriving, typename TBase = CWindow>
class CPropertySheetImpl :
	public PROPSHEETHEADER,
	public CWindowImplBaseT< CPropertySheetT<TBase> >
{
public:
	CPropertySheetImpl(LPCTSTR pszCaption, UINT _nStartPage = 0)
	{
		ZeroMemory(this, sizeof(PROPSHEETHEADER));
		dwSize = sizeof(PROPSHEETHEADER);
		hInstance = _Module.GetModuleInstance();
		dwFlags = PSH_DEFAULT | PSH_NOAPPLYNOW;
		m_bstrCaption = pszCaption;
		nStartPage = _nStartPage;
	}

	CPropertySheetImpl(UINT nCaptionId, UINT _nStartPage = 0)
	{
		ZeroMemory(this, sizeof(PROPSHEETHEADER));
		dwSize = sizeof(PROPSHEETHEADER);
		hInstance = _Module.GetModuleInstance();
		dwFlags = PSH_DEFAULT | PSH_NOAPPLYNOW;
		m_bstrCaption.LoadString(nCaptionId);
		nStartPage = _nStartPage;
	}

	int DoModal(HWND parent = ::GetActiveWindow())
	{
		dwFlags &= ~PSH_MODELESS;

		// Create and run the wizard
		int ret = PropertySheet(parent);
		return (ret == -1) ? IDABORT : m_nModalResult;
	}

	HWND Create(HWND parent = ::GetActiveWindow())
	{
		dwFlags |= PSH_MODELESS;

		// Create and run the wizard
		int ret = PropertySheet(parent);
		return (ret == -1) ? 0 : (HWND)ret;
	}

	BOOL Translate(const MSG& msg, HACCEL hAccel = 0)
	{
        // Let active page have a crack and than try the sheet itself
        HWND    hwndPage = GetCurrentPageHwnd();
        if( ::IsWindow(hwndPage) &&
            ::SendMessage(hwndPage, WM_FORWARDMSG, 0, (LPARAM)&msg) )
        {
                return TRUE;
        }
        
		return CPropertySheetT<TBase>::IsDialogMessage(&msg);
	}

	BEGIN_MSG_MAP(CPropertySheetImpl)
		COMMAND_ID_HANDLER(IDOK, OnButton)
		COMMAND_ID_HANDLER(IDCANCEL, OnButton)
		COMMAND_ID_HANDLER(IDFINISH, OnButton)
        MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
	END_MSG_MAP()

	LRESULT OnButton(WORD, UINT nID, HWND, BOOL& bHandled)
	{
        // Fake a notification message for IDFINISH
        // NOTE: PSN_APPLY is sent to all pages when a modal IDOK or IDAPPLY
        //       is pressed. However, when IDFINISH or modeless IDOK is pressed,
        //       there is no all-page notification, which is a pain for Property
        //       Page Sites, which needs a notification to let them know when to call
        //       IPropertyPage::Apply().
        // NOTE: If this is a modeless property sheet, and the fake OnKillActive
        //       returns FALSE, it will be called twice, once by the fake and
        //       once by the DefWindowProc below.
        if( (nID == IDFINISH) ||
            ((nID == IDOK) && (dwFlags & PSH_MODELESS)) )
        {
            HPROPSHEETPAGE  hpsp = (HPROPSHEETPAGE)QuerySiblings(PSN_APPLYFINISH, 0);
            if( hpsp )
            {
                // A page is unhappy with its results
                // NOTE: -1 means that the current page would like to stay active.
                //       This is an important distinction, because if we SetCurSel
                //       on the currently active page, it will receive PSN_KILLACTIVE,
                //       PSN_SETACTIVE messages again.
                if( hpsp != (HPROPSHEETPAGE)-1 ) SetCurSel(hpsp);
                bHandled = TRUE;
                return TRUE;
            }
        }

		bHandled = FALSE;
		if( dwFlags & PSH_MODELESS )
		{
            // Let the propery sheet get this message first, so that it can
            // notify all the pages, e.g. PSN_APPLY, before we destroy the window
			LRESULT res = DefWindowProc();

            // We need to enable the parent window before destroying this window
            // or the parent will be sent to the background.
			::EnableWindow(GetParent(), TRUE);

			DestroyWindow();
		}

        m_nModalResult = (nID == IDFINISH ? IDOK : nID);

		return 0;
	}

    LRESULT OnSysCommand(UINT, WPARAM wp, LPARAM, BOOL& bHandled)
    {
        // Neither SC_CLOSE nor PressButton seems to work in the modeless case,
        // so force it.
        if( (dwFlags & PSH_MODELESS) && (wp == SC_CLOSE) )
        {
            //PressButton(PSBTN_CANCEL);
            PostMessage(WM_COMMAND, MAKEWPARAM(IDCANCEL, BN_CLICKED), LPARAM(GetDlgItem(IDCANCEL)));
            bHandled = TRUE;
            return 0;
        }

        bHandled = FALSE;
        return 1;
    }

protected:
	enum { IDFINISH = 0x3025 };

	struct SheetData
	{
		HHOOK                hhook;
		CPropertySheetImpl*  pThis;
	};

	static
	LRESULT CALLBACK SheetHook(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode == HCBT_CREATEWND)
		{
			HWND            hwnd = reinterpret_cast<HWND>(wParam);
			CBT_CREATEWND*  pcs = reinterpret_cast<CBT_CREATEWND*>(lParam);
			SheetData*      pData = reinterpret_cast<SheetData*>(_Module.ExtractCreateWndData());
			UnhookWindowsHookEx(pData->hhook);
			pData->hhook = 0;
			pData->pThis->SubclassWindow(hwnd);
		}

		return 0;
	}

	int PropertySheet(HWND parent)
	{
		USES_CONVERSION;

		// Get the pages set up by the page map
		TDeriving * pThis = static_cast<TDeriving*>(this);

		// Set up the header
		dwFlags &= ~PSH_PROPSHEETPAGE;
		hwndParent = parent;
		pszCaption = OLE2CT(m_bstrCaption);

		// Count the pages
		nPages = pThis->GetPropertyPages(NULL);

		// Create the pages
		phpage = new HPROPSHEETPAGE[nPages];
		pThis->GetPropertyPages(phpage);

		// Provide an opportunity to subclass
		SheetData data = { 0, pThis };
		_Module.AddCreateWndData(&m_thunk.cd, &data);

		// Set the hook
		// NOTE: The hook is needed so that we can catch messages on the sheet sooner.
		//       The PSCB_INITIALIZED callback only happens *after* WM_CREATE.
		data.hhook = SetWindowsHookEx(WH_CBT, SheetHook, _Module.GetModuleInstance(), GetCurrentThreadId());

		// Create and run the property sheet
		int ret = ::PropertySheet(this);
		delete[] phpage;
		ppsp = 0;

		// Unhook our hooks on failure to create property sheet
		if( (ret == -1) && (data.hhook) )
		{
			ATLTRACE("PropertySheet failure: 0x%x\n", GetLastError());
			_Module.ExtractCreateWndData();
			UnhookWindowsHookEx(data.hhook);
		}

		return ret;
	}

protected:
	int         m_nModalResult;
	CComBSTR    m_bstrCaption;
};

#endif  // INC_ATLPROPSHEET
