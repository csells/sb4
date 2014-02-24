// atlcompose.h: ATL composition support
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 19971-1998, 1998 Chris Sells
// All rights reserved.
//
// 8/14/98
// -Fixed ref-count bug pointed out by Valery Pryamikov.
//  Unfortunately, this means that the interface map composing
//  macros can no longer be used as the first entry in the map.
// -Put _Compose function back so ATL will catch it if you
//  try to make one of the COMPOSE macros first in your map.
//
// 4/15/98
// -Added support for Dual composed interfaces via
//  BEGIN_COMPOSED_CLASS_DUAL and BEGIN_COMPOSED_CLASS_DUAL_EX.
//
// 2/18/98
// -Fixed a bug where the composition macros couldn't
//  be used at the first entry in a COM_MAP
//  (now they can be).
// -Fixed a bug that prevented the This() function
//  from being used to access protected and private
//  members of the composing class.
// -Thanks to Russ for pointing these two bugs out to me.
//
// 2/13/98
// -Replaced _Compose function w/ _ATL_SIMPLEMAPENTRY
//
// NO WARRANTIES ARE EXTENDED. USE AT YOUR OWN RISK.
//
// To contact the author with suggestions or comments, use csells@sellsbrothers.com.
/////////////////////////////////////////////////////////////////////////////
// This set of macros is for using nested compostion in ATL.
// The benefit of composition is that it is easy to implement
// multiple interfaces with methods with the same name
// but that require different behavior, e.g.
//
// interface IArtist : IUnknown {
//   HRESULT Draw();
// }
//
// interface ICowboy : IUnknown {
//   HRESULT Draw();
// }
//
// The benefit of this particular implementation is that it 
// has no object size overhead. Interfaces implemented using
// nested composition have no greater space requirements than
// using multiple-inheritance and no speed hit.
//
// COM_INTERFACE_ENTRY_COMPOSED(itf, classInner)
//  Macro for putting composed interfaces into the interface map.
//
// COM_INTERFACE_ENTRY_COMPOSED_IID(iid, classInner)
//  Macro for putting composed interfaces with an interface different from
//  the name of the interface into the interface map.
//
// BEGIN_COMPOSED_CLASS(classOuter, itf, classInner)
//  Macro for starting the declaration of a composed class.
//
// BEGIN_COMPOSED_CLASS_DUAL(classOuter, itf, libid, classInner)
//  Macro for starting the declaration of a composed class
//  that implements a dual interface.
//
// BEGIN_COMPOSED_CLASS_DUAL_EX(classOuter, itf, libid, major, minor, classInner)
//  Macro for starting the declaration of a composed class
//  that implements a dual interface of some version other than 1.0.
//
// END_COMPOSED_CLASS(classInner)
//  Macro for ending the declaration of a composed class.
//
// This()
//  Method for accessing composing class inside of a composed class method.
//  Returns a pointer to a classOuter type.
//
// Intended usage:
/*
class CAcePowell : ... // code remove for clarity
    public IArtist // ICowboy not in list of interfaces
{
public:
BEGIN_COM_MAP(CAcePowell)
	COM_INTERFACE_ENTRY(IArtist)
    COM_INTERFACE_ENTRY_COMPOSED(ICowboy, XCowboy)
END_COM_MAP()

// IArtist
    STDMETHODIMP Draw()
    {
        MessageBox(0, "Say 'Cheese.'", "IArtist::Draw", MB_SETFOREGROUND);
        return S_OK;
    }

// ICowboy
BEGIN_COMPOSED_CLASS(CAcePowell, ICowboy, XCowboy)

    STDMETHODIMP Draw()
    {
        MessageBox(0, "Reach for the sky.", "ICowboy::Draw", MB_SETFOREGROUND);
        return S_OK;
    }

END_COMPOSED_CLASS(XCowboy)
};
*/
//
// When implementing methods outside of the class declaration, use the classInner
// name as part of the scope resolution, e.g.
//
/*
// AcePowell.h

class CAcePowell : ... // code remove for clarity
{
    ... // more code remove for clarity

// ICowboy
BEGIN_COMPOSED_CLASS(CAcePowell, ICowboy, XCowboy)
    STDMETHODIMP Draw();    // Code not part of the class declaration
END_COMPOSED_CLASS(XCowboy)
};

// AcePowell.cpp

// classInner used as part of the scope resolution
STDMETHODIMP CAcePowell::XCowboy::Draw()
{
    MessageBox(0, "Reach for the sky.", "ICowboy::Draw", MB_SETFOREGROUND);
    return S_OK;
}
*/
//
// When exposing base interfaces from a composed interface, make sure is
// one BEGIN_COMPOSED_CLASS macro for each IID and that the BEGIN_COMPOSED_CLASS
// macro has the name of the most derived interface, e.g.
//
/*
// AcePowell.idl

interface ICowboyEx : ICowboy
{
    HRESULT QuickDraw();
};

// AcePowell.h

class CAcePowell : ... // code remove for clarity
{
public:
BEGIN_COM_MAP(CAcePowell)
	COM_INTERFACE_ENTRY(IArtist)
    COM_INTERFACE_ENTRY_COMPOSED(ICowboy, XCowboy)   // Both ICowboy and ICowboyEx
    COM_INTERFACE_ENTRY_COMPOSED(ICowboyEx, XCowboy) // exposed from XCowboy
END_COM_MAP()

    ... // more code remove for clarity

// ICowboy
BEGIN_COMPOSED_CLASS(CAcePowell, ICowboyEx, XCowboy)
    STDMETHODIMP Draw();
    STDMETHODIMP QuickDraw();
END_COMPOSED_CLASS(XCowboy)
};
*/
//
// BTW, Ace Powell was an artist-cowboy who lived at the turn of the century.
// I'd like to thank Tim Ewald for this example (which I've since beaten to death).
//

#pragma once
#include <stddef.h> // offsetof

inline
HRESULT WINAPI _Compose(void* pvThis, REFIID riid, LPVOID* ppv, DWORD dw)
{
    *ppv = (BYTE*)pvThis + dw;
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

#define COM_INTERFACE_ENTRY_COMPOSED(itf, classInner) \
    {&IID_##itf, offsetof(_ComMapClass, m_##classInner), _Compose},

#define COM_INTERFACE_ENTRY_COMPOSED_IID(iid, classInner) \
    {&iid, offsetof(_ComMapClass, m_##classInner), _Compose},

#define BEGIN_COMPOSED_CLASS(classOuter, itf, classInner) \
    class classInner; \
    friend class classInner; \
    class classInner : public itf { \
    private: \
        classOuter* This() \
        { return (classOuter*)((BYTE*)this - offsetof(classOuter, m_##classInner)); } \
    public: \
        STDMETHODIMP QueryInterface(REFIID riid, void** ppv) \
        { return This()->_InternalQueryInterface(riid, ppv); } \
        STDMETHODIMP_(ULONG) AddRef() \
        { return This()->GetUnknown()->AddRef(); } \
        STDMETHODIMP_(ULONG) Release() \
        { return This()->GetUnknown()->Release(); }

#define BEGIN_COMPOSED_CLASS_DUAL(classOuter, itf, libid, classInner) \
    class classInner; \
    friend class classInner; \
    class classInner : public IDispatchImpl<itf, &IID_##itf, &libid> { \
    private: \
        classOuter* This() \
        { return (classOuter*)((BYTE*)this - offsetof(classOuter, m_##classInner)); } \
    public: \
        STDMETHODIMP QueryInterface(REFIID riid, void** ppv) \
        { return This()->_InternalQueryInterface(riid, ppv); } \
        STDMETHODIMP_(ULONG) AddRef() \
        { return This()->GetUnknown()->AddRef(); } \
        STDMETHODIMP_(ULONG) Release() \
        { return This()->GetUnknown()->Release(); }

#define BEGIN_COMPOSED_CLASS_DUAL_EX(classOuter, itf, libid, major, minor, classInner) \
    class classInner; \
    friend class classInner; \
    class classInner : public IDispatchImpl<itf, &IID_##itf, &libid, major, minor> { \
    private: \
        classOuter* This() \
        { return (classOuter*)((BYTE*)this - offsetof(classOuter, m_##classInner)); } \
    public: \
        STDMETHODIMP QueryInterface(REFIID riid, void** ppv) \
        { return This()->_InternalQueryInterface(riid, ppv); } \
        STDMETHODIMP_(ULONG) AddRef() \
        { return This()->GetUnknown()->AddRef(); } \
        STDMETHODIMP_(ULONG) Release() \
        { return This()->GetUnknown()->Release(); }

#define END_COMPOSED_CLASS(classInner) } m_##classInner;
