// atlsip.h
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1998, Chris Sells
// All rights reserved.
// 2/13/98: 1st release.
// 3/5/98:  Added CComQIPtr specialization for IUnknown and removed SIPUNK.
//
// NO WARRANTIES ARE EXTENDED. USE AT YOUR OWN RISK.
//
// To contact the author with suggestions or comments, use csells@sellsbrothers.com.
/////////////////////////////////////////////////////////////////////////////
// This file defines a macro to save typing when using CComQIPtr objects.
// It also provides a template specialization for CComQIPtr so that
// IUnknown works, too.
//
// Usage:
/*
void foo()
{
    // Replace CComPtr<IUnknown> (or CComQIPtr<IUnknown, &IID_IUnknown>)
    SIP(IUnknown)   spunk;
    CoCreateInstance(CLSID_Mouse, 0, CLSCTX_ALL, IID_IUnknown, (void**)&spunk);
    
    if( spunk )
    {
        printf("Mouse created.\n");
        
        // Replace CComQIPtr<IMouse, &IID_IMouse>
        SIP(IMouse) spm = spunk;
        if( spm )
        {
            printf("IMouse acquired.\n");
        }
    }
}
*/

#ifndef ATL_NO_NAMESPACE
namespace ATL
{
#endif

#if _MSC_VER>1020
template <>
#endif
class CComQIPtr<IUnknown, &IID_IUnknown>
{
public:
	typedef IUnknown _PtrClass;
	CComQIPtr() {p=NULL;}
	CComQIPtr(IUnknown* lp)
	{
		if ((p = lp) != NULL)
			p->AddRef();
	}
	CComQIPtr(const CComQIPtr<IUnknown,&IID_IUnknown>& lp)
	{
		if ((p = lp.p) != NULL)
			p->AddRef();
	}
	~CComQIPtr() {if (p) p->Release();}
	void Release() {if (p) p->Release(); p=NULL;}
	operator T*() {return p;}
	IUnknown& operator*() {_ASSERTE(p!=NULL); return *p; }
	//The assert on operator& usually indicates a bug.  If this is really
	//what is needed, however, take the address of the p member explicitly.
	IUnknown** operator&() { _ASSERTE(p==NULL); return &p; }
	IUnknown* operator->() {_ASSERTE(p!=NULL); return p; }
	IUnknown* operator=(IUnknown* lp){return (IUnknown*)AtlComPtrAssign((IUnknown**)&p, lp);}
	IUnknown* operator=(const CComQIPtr<IUnknown,&IID_IUnknown>& lp)
	{
		return (IUnknown*)AtlComPtrAssign((IUnknown**)&p, lp.p);
	}
#if _MSC_VER>1020
	bool operator!(){return (p == NULL);}
#else
	BOOL operator!(){return (p == NULL) ? TRUE : FALSE;}
#endif
	T* p;
};

#ifndef ATL_NO_NAMESPACE
}; //namespace ATL
using namespace ATL;
#endif

#define SIP(itf) CComQIPtr<itf, &IID_##itf>
