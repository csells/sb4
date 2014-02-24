// CycleDetect.h: A set of helpers for detecting ref count cycles in COM.
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1999, Chris Sells.
// All rights reserved. No warranties extended. Use at your own risk.
// Comments and bug reports to csells@sellsbrothers.com.
/////////////////////////////////////////////////////////////////////////////
// History:
// -8/31/99:
//  -Initial release. Still in the experimental stage.
/////////////////////////////////////////////////////////////////////////////
// This file defines a set of types for detecting reference count cycles,
// specifically for use in building hierarchical object models. The goal
// was to allow a client the option of manual or automatic "Close" of an
// object model with a minimum of impact on the code of the object model
// itself. This is all done in the face of expected reference cycles, i.e.
// child holds onto one or more parent interfaces.
//
// The general philosophy is that a child object in an object model cannot
// exist without its parent. Also, the child needs to be able to respond
// to requests for it's parent (as well as the topmost object in the model,
// often known as the "application" object). The child can keep it's parent
// around for its (the child's) life as well as cache the parent's interface
// by keeping a COM reference on one of the parent's interfaces. However, as
// the parent is also likely to keep a reference on its child, what results
// is known as a reference count cycle. Practically, this means that in the
// absence of some protocol to elminate the cycle that we are going to leak
// objects that the client no longer references, but are keeping each other
// alive.
//
// This file attempts to solve this problem by introducing a protocol between
// the parent and the child such that the child notifies the parent when it
// (the child) is holding a reference on the parent. This protocol allows
// the parent to keep track of the number of cyclic references as well as
// the number of total references. When the number of total references on
// the parent object drops to the number of cycle references, the parent
// has detected a cycle, i.e. the client no longer holds any references
// and auto-closing of the object model can begin.
//
// The way the protocol is modeled is via a COM interface called
// ICycleDetectParent:
/*
[ uuid(6F510BA0-5E74-11D3-94C2-00500428E7C0), object ]
interface ICycleDetectParent : public IUnknown
{
    HRESULT CycleAddRef();
    HRESULT CycleRelease();
};
*/
// Whenever the child acquires an interface on a parent, it will QI for
// ICycleDetectParent and call CycleAddRef(). CycleRelease is called when
// the child releases the cyclic reference on the parent. For convenience,
// a CComCyclePtr has been provided that acts just like CComPtr, but does
// the extra steps required by the ICycleDetectParent protocol.
//
// The parent's implementation of ICycleDetectParent has merely to manage
// another reference (canonically called m_dwCycleRef). Cycles themselves
// should only be checked for during IUnknown::Release. Cycles can appear
// to manifest themselves at other times, but are logically impossible until
// IUnknown::Release is called. An implementation of ICycleDetectParent
// (called ICycleDetectParentImpl) is provided as a convenience. This
// implementation will call the following function when a cycle is detected:
//
// void OnCycleDetected();
//
// The typical response is for the object to call whatever function the
// client would have called, had it been doing shutdown manually. The
// canonical name for this function is "Close".
//
// If you are using ATL to implement your COM classes, you need to hook
// into the implementation of Release to check for cycles. You can do
// this by implementing InternalRelease in your C++ class and calling
// ICycleDetectParentImpl::CheckCycle after calling the CComObjectRootEx
// base class's implementation of InternalRelease. A macro,
// DECLARE_DETECT_CYCLE has been provided that does this.
//
// NOTE: Only the objects in your object model hierarchy that need to
// perform shutdown automatically need to implement cycle detection, i.e.
// if your object has no "Close" function, there's no need to use cycle
// detection. Practically speaking, this means that child objects that are
// themselves parents do not need to do cycle detection, but the top-level
// application object does.
/////////////////////////////////////////////////////////////////////////////
// Usage:
// In the application object:
//  1. Derive from ICycleDetectParentImpl.
//  2. Add ICycleDetectParent to the COM map.
//  3. Put 'DECLARE_CYCLE_DETECT()' somewhere in your class.
//  4. Implement 'void OnCycleDetected()' to call your application's "Close"
//     method.
//  5. The "Close" method should let all child objects know that they've
//     been closed so that they can release their parent's interface and
//     refuse further calls.
/*
class ATL_NO_VTABLE CApplication : 
	...
    public ICycleDetectParentImpl<CApplication> // 1.
{
BEGIN_COM_MAP(CApplication)
	...
	COM_INTERFACE_ENTRY(ICycleDetectParent) // 2.
END_COM_MAP()

DECLARE_CYCLE_DETECT(CApplication) // 3.

    void OnCycleDetected()  // 4.
    {
        Close();
    }

    STDMETHODIMP Close()    // 5.
    {
        CComPtr<IPrivateClose>  spClose;
        m_spDocumentCollection->QueryInterface(&spClose);
        spClose->Close();
        return S_OK;    
    }
    ...
private:
    CComPtr<IDocumentCollection> m_spDocumentCollection;
};
*/
// In the child object:
//  1. Use CComCyclePtr to cache the parent's interface.
//  2. Release the parent's interface during Close.
//  3. Refuse further calls (since you've been closed).
/*
class ATL_NO_VTABLE CDocumentCollection : 
	...
{
...
    HRESULT Init(IApplication* pParent)
    {
        m_spParent = pParent;   // 1.
        return S_OK;
    }

    HRESULT Close()
    {
        // Closing child objects elided...

        m_spParent.Release();   // 2.
        return S_OK;
    }

    STDMETHODIMP DoSomething()
    {
        if( !m_spParent ) return E_UNEXPECTED;  // 3.
        ...
    }

private:
    CComCyclePtr<IApplication>  m_spParent; // 1.
};
*/
/////////////////////////////////////////////////////////////////////////////
// Known Issues:
//  -This has not been tested for multiple objects at different levels of a
//   hierarchy that want cycles tested independently of a higher level cycle.
//   It should work (meaning I want it to...).
//  -This has not been tested across context boundaries, i.e. in the presence
//   of a proxy/stub between the parent and the child. There are issues
//   related to the proxy manager's caching of AddRef/Release calls.
//  -The interface is defined in this header file and not in IDL. This makes
//   usage from C++ convenient and from any other language impossible.
//  -There are too many lines of integration code. I'd prefer fewer to reduce
//   the chance for error.
//  -The testing has been pretty rudimentary.
//  -Should OnCycleDetected return an HRESULT?
//  -Is the theory sound? There are cases where a child object may decide to
//   use cycle detection, but is it ever the case that it *must*?
//  -What bad things happen when a non-child gets ahold of ICycleDetectParent?
/////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef INC_CYCLEDETECT
#define INC_CYCLEDETECT

MIDL_INTERFACE("6F510BA0-5E74-11D3-94C2-00500428E7C0")
ICycleDetectParent : public IUnknown
{
public:
    STDMETHOD(CycleAddRef)() =0;
    STDMETHOD(CycleRelease)() =0;
};

#define IID_ICycleDetectParent __uuidof(ICycleDetectParent)

template <typename Deriving>
class ICycleDetectParentImpl : public ICycleDetectParent
{
public:
    ICycleDetectParentImpl() : m_dwCycleRef(0)
    {}

    // ICycleDetectParent
    STDMETHODIMP CycleAddRef()
    {
        ULONG   l = Deriving::_ThreadModel::Increment(&m_dwCycleRef);
        //ATLTRACE("++m_dwCycleRef= %d\n", m_dwCycleRef);
        return S_OK;
    }

    STDMETHODIMP CycleRelease()
    {
        ULONG   l = Deriving::_ThreadModel::Decrement(&m_dwCycleRef);
        //ATLTRACE("--m_dwCycleRef= %d\n", m_dwCycleRef);
        return S_OK;
    }

protected:
    // If CheckCycle returns true, a cycle was detected and object
    // may very well have killed itself. This means no checking of
    // member variables if CheckCycle returns true.
    bool CheckCycle()
    {
        if( m_dwCycleRef && (m_dwCycleRef == This()->m_dwRef) )
        {
            // Keep object alive during this method call
            CComPtr<IUnknown>   punk(This()->GetUnknown());

            // Let object know a cycle was detected --
            // it's likely response is to kill itself.
            This()->OnCycleDetected();
            return true;
        }

        return false;
    }

private:
    Deriving* This()
    { return static_cast<Deriving*>(this); }

private:
    long    m_dwCycleRef;
};


#define DECLARE_CYCLE_DETECT(_class) \
/*	ULONG InternalAddRef() \
	{ \
        ULONG   l = CComObjectRootEx<_ThreadModel>::InternalAddRef(); \
        ATLTRACE("++m_dwRef= %d\n", m_dwRef); \
        return l; \
	} \
\
*/	ULONG InternalRelease() \
    { \
        ULONG   l = CComObjectRootEx<_ThreadModel>::InternalRelease(); \
        /*ATLTRACE("--m_dwRef= %d\n", m_dwRef);*/ \
        ICycleDetectParentImpl<_class>::CheckCycle(); \
        return l; \
    }

template <typename T>
class CComCyclePtr : public CComPtr<T>
{
public:
    CComCyclePtr(T* _p = 0) : CComPtr<T>(_p)
	{
        CycleAddRef();
	}

    CComCyclePtr(const CComCyclePtr<T>& _p) : CComPtr<T>(_p)
	{
        CycleAddRef();
	}

	~CComCyclePtr()
	{
		CycleRelease();
	}

	void Release()
	{
        CycleRelease();
        CComPtr<T>::Release();
	}

	CComCyclePtr<T>& operator=(T* _p)
	{
        return Assign(_p);
	}

	CComCyclePtr<T>& operator=(const CComPtr<T>& sp)
	{
        return Assign(sp.p);
	}

	void Attach(T* _p)
	{
        CycleRelease();
        CComPtr<T>::Attach(_p);
        CycleAddRef();
        return *this;
	}

	T* Detach()
	{
        CycleRelease();
        return CComPtr<T>::Detach();
	}

private:
    CComCyclePtr<T>& Assign(T* _p)
    {
        CycleRelease();
        CComPtr<T>::operator=(_p);
        CycleAddRef();
        return *this;
    }

    void CycleAddRef()
    {
        CComQIPtr<ICycleDetectParent>   sp = p;
        if( sp ) sp->CycleAddRef();
    }

    void CycleRelease()
    {
        CComQIPtr<ICycleDetectParent>   sp = p;
        if( sp ) sp->CycleRelease();
    }
};

#endif // INC_CYCLEDETECT
