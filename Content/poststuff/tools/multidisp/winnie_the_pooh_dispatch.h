#ifndef __BxComExt
#define __BxComExt
//***********************************************
// Author:  Kjell Tangen (kat@computas.no)
// Filename: BxComExt.h
// Part of: BRIX COM Frameworks
// Copyright (C) Kjell Tangen, 1998
//-----------------------------------------------
// Description:
// This file contains implementation classes for COM
// CComTypeInfoHolderEx - Extends ATL::CComTypeInfoHolder
// IDispatchImplEx      - Replaces ATL::IDispatchImpl
// CComClonableBase     - Base class for clonable COM objects
//
// The implementation uses ATL (2.1) and STL.
// This file should be included right after the ATL-includes
// (normally in stdafx.h)
//-----------------------------------------------
// Created: 1998-03-06
// 1.01.01  1998-05-07
//***********************************************

#pragma warning( disable : 4786) // identifier truncated to 255 chars

#include <utility>
#include <vector>
#include 

// Required by STL:
bool operator <(const GUID g1, const GUID g2); #ifndef _stdDispIdBase // Override this to have another default #define _stdDispIdBase 0x10F0 #endif //************************************************************************** * // Class CComTypeInfoHolderEx // Kjell Tangen, 1998 (kat@computas.no) // Computas, Norway // // CComTypeInfoHolderEx is used by the IDispatchImplEx template class to access // the object's type information in order to support dispatch clients. // This class uses type info to dispatch to any dual or dispatchable // interface on the class. If you have an ATL-based implementation, // use the IDispatchImplEx class instead of the standard ATL::IDispatchImpl // class if your COM-class has multiple dual interfaces and you want your // class to be scriptable. CComTypeInfoHolderEx will search through the type // info on each of the interfaces on the class and try to bind to the // dispatch member requested by the client. // Even if your implementation is not based on ATL, this class can be used by // your dual interface implementation, just look in the IDispatchImplEx // class for details on how to use it. // // Intented usage: // If you have an ATL-based COM implementation, you will not use // CComTypeInfoHolderEx directly. Instead you will just implement your dual interfaces // by deriving your class from IDispatchImplEx instead of IDispatchImpl: // class ATL_NO_VTABLE CMyClass : // public IDispatchImplEx<IMyInterface, &IID_IMyInterface, &CLSID_MyClass, &LIBID_MyLib>,
//
//**************************************************************************
*
/* Example:

IDL:
----
   interface IA : IDispatch
   {
      HRESULT MethodOnIA();
   }:
   interface IB : IDispatch
   {
      HRESULT MethodOnIB();
   };
   coclass NaPMView
 {
  [default] interface IA;
      interface IB;
   };
C++ implementation:
---------------------
   class ATL_NO_VTABLE CMyClass :
    public IDispatchImplEx<IA, &IID_IA, &CLSID_MyClass, &LIBID_MyLib>,
    public IDispatchImplEx<IB, &IID_IB, &CLSID_MyClass, &LIBID_MyLib>,
    public CComCoClass<CMyClass, &CLSID_MyClass>,
    public ISupportErrorInfo,
      public CComObjectRoot
   {
   ..standard ATL implementation

VB script client:
-----------------

  Dim instanceOfMyClass
  ... got instanceOfMyClass from somewhere
  ' Regardless of which interface I got hold of,
  ' I can script towards all methods on the class:
  instanceOfMyClass.MethodOnIA
  instanceOfMyClass.MethodOnIB
*/

template<const IID* piid, const CLSID* pclsid, const GUID* plibid, long DispIdBase="_stdDispIdBase">
class CComTypeInfoHolderEx
: public CComTypeInfoHolder
{
   typedef std::pair<long, IID> _dispidEntry;
   typedef std::vector<_dispidEntry> _idispidContainer;
   typedef std::map<_dispidEntry, long> _cdispidContainer;
   typedef _cdispidContainer::value_type  _cdispValue;

   typedef _idispidContainer::value_type  _idispValue;

   long m_nextIdx;
   _idispidContainer m_idispCont; // Contains Interface-specific dispids
   _cdispidContainer m_cdispCont; // Maps pairs of IID+dispid to
class-specific dispids
   const GUID* m_pclsid;
public:
   CComTypeInfoHolderEx(const IID* pIID = piid, const CLSID* pCLSID =
pclsid, const GUID* pLIBID = plibid)
   : m_idispCont()
   {
      m_nextIdx = 0;
      m_pguid = pIID;
      m_pclsid = pCLSID;
      m_plibid = pLIBID;
      m_wMajor = 1;
      m_wMinor = 0;
      m_pInfo = NULL;
      m_dwRef = 0;
   }
   long GetCDIDFromIdx(long idx) {return (DispIdBase + idx);}
   long GetIdxFromCDID(long cdid){return (cdid - DispIdBase);}
   long AddEntry(long lDID, IID);
   long GetDIDFromIdx(long idx){return m_idispCont[idx].first;}
   IID& GetIIDFromIdx(long idx) {return m_idispCont[idx].second;}
 HRESULT GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
  LCID lcid, DISPID* rgdispid);
 HRESULT Invoke(IDispatch* p, DISPID dispidMember, REFIID riid,
  LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
  EXCEPINFO* pexcepinfo, UINT* puArgErr);
};

template<const IID* piid, const CLSID* pclsid, const GUID* plibid, long DispIdBase="_stdDispIdBase">
inline long
CComTypeInfoHolderEx<piid,pclsid,plibid,DispIdBase>::AddEntry(long lDID, IID
riid)
{
   long lNewId;
   _dispidEntry e(lDID,riid);
   lNewId = m_nextIdx;
   // Check if the entry exist
   _cdispidContainer::iterator iter = m_cdispCont.find(e);
   if(iter != m_cdispCont.end())
   {
      lNewId = (*iter).second;
   }
   else
   {
      m_idispCont.insert(m_idispCont.end(), e);
      m_cdispCont.insert(_cdispValue(e,lNewId));
      InterlockedIncrement(&m_nextIdx);
   }
   return lNewId;
}

template<const IID* piid, const CLSID* pclsid, const GUID* plibid, long DispIdBase="_stdDispIdBase">
inline HRESULT
CComTypeInfoHolderEx<piid,pclsid,plibid,DispIdBase>::GetIDsOfNames(REFIID
riid, LPOLESTR* rgszNames,
 UINT cNames, LCID lcid, DISPID* rgdispid)
{
   HRESULT hRes;
   hRes = CComTypeInfoHolder::GetIDsOfNames(riid,rgszNames,cNames,
                                             lcid,rgdispid);
   if(SUCCEEDED(hRes))
      return hRes;
   else if(hRes != DISP_E_UNKNOWNNAME)
      return hRes;
   LPOLESTR psName = rgszNames[0];
 ITypeInfo* pInfo;
 hRes = GetTI(lcid, &pInfo);
 if (pInfo != NULL)
 {
      unsigned int idx;
      ITypeLib* pifTypeLib = NULL;
      hRes = pInfo->GetContainingTypeLib(&pifTypeLib, &idx);
      if(SUCCEEDED(hRes) && pifTypeLib != NULL)
      {
         ITypeInfo* pClassTI = NULL;
         hRes = pifTypeLib->GetTypeInfoOfGuid(*pclsid,&pClassTI);
         if(SUCCEEDED(hRes) && pClassTI != NULL)
         {
            ITypeComp* pifTypeComp = NULL;
            hRes = pInfo->GetTypeComp(&pifTypeComp);
            if(pifTypeComp != NULL)
            {
               ITypeInfo* pifTypeInfo2 = NULL;
               DESCKIND dc;
               BINDPTR bp;
               long lHashId = LHashValOfName(lcid,psName);
               hRes = pifTypeComp->Bind(psName,lHashId, 0,
                                       &pifTypeInfo2, &dc, &bp);
               if(SUCCEEDED(hRes) && pifTypeInfo2 != NULL)
               {
                  long lDispId;
              hRes = pifTypeInfo2->GetIDsOfNames(rgszNames, cNames,
&lDispId);
                  if(SUCCEEDED(hRes))
                  {
                     TYPEATTR* pTAttr = NULL;
                     hRes = pifTypeInfo2->GetTypeAttr(&pTAttr);
                     if(SUCCEEDED(hRes) && pTAttr != NULL)
                     {
                        long idx;
                        EnterCriticalSection(&_Module.m_csTypeInfoHolder);
                        idx = AddEntry(lDispId, (IID&)pTAttr->guid);
                        *rgdispid = GetCDIDFromIdx(idx);
                        LeaveCriticalSection(&_Module.m_csTypeInfoHolder);
                        pifTypeInfo2->ReleaseTypeAttr(pTAttr);
                     }
                  }
                  pifTypeInfo2->Release();
               }
               else if(SUCCEEDED(hRes))
               {
                  // Search through the class type info
                  TYPEATTR* pTAttr = NULL;
                  hRes = pClassTI->GetTypeAttr(&pTAttr);
                  if(SUCCEEDED(hRes) && pTAttr != NULL)
                  {
                     HREFTYPE hRefType;
                     unsigned int i;
                     bool bComplete = false;
                     for(i = 0; !bComplete && i <pTAttr->cImplTypes; ++i)
                     {
                        hRes = pClassTI->GetRefTypeOfImplType(i,&hRefType);
                        if(SUCCEEDED(hRes))
                        {
                           hRes = pClassTI->GetRefTypeInfo(hRefType,
&pifTypeInfo2);
                           if(SUCCEEDED(hRes) && pifTypeInfo2 != NULL)
                           {
                              long lDispId;
                          hRes = pifTypeInfo2->GetIDsOfNames(rgszNames,
cNames, &lDispId);
                              if(SUCCEEDED(hRes))
                              {
                                 TYPEATTR* pTAttr2 = NULL;
                                 hRes = pifTypeInfo2->GetTypeAttr(&pTAttr2);
                                 if(SUCCEEDED(hRes) && pTAttr2 != NULL)
                                 {
                                    bComplete = true;

EnterCriticalSection(&_Module.m_csTypeInfoHolder);
                                    long idx = AddEntry(lDispId,
(IID&)pTAttr2->guid);
                                    *rgdispid = GetCDIDFromIdx(idx);

LeaveCriticalSection(&_Module.m_csTypeInfoHolder);
                                    pifTypeInfo2->ReleaseTypeAttr(pTAttr2);
                                 }
                              }
                              pifTypeInfo2->Release();
                           }
                        }
                     }
                     pClassTI->ReleaseTypeAttr(pTAttr);
                  }
               }
               pifTypeComp->Release();
            }
            pClassTI->Release();
         }
         pifTypeLib->Release();
      }

  pInfo->Release();
 }
 return hRes;
}

template<const IID* piid, const CLSID* pclsid, const GUID* plibid, long DispIdBase="_stdDispIdBase">
inline HRESULT
CComTypeInfoHolderEx<piid,pclsid,plibid,DispIdBase>::Invoke(IDispatch* p,
DISPID dispidMember, REFIID riid,
 LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
 EXCEPINFO* pexcepinfo, UINT* puArgErr)
{
   HRESULT hRes;
   hRes = CComTypeInfoHolder::Invoke(p, dispidMember, riid, lcid, wFlags,
                              pdispparams, pvarResult,pexcepinfo, puArgErr);
   if(SUCCEEDED(hRes))
      return hRes;
   else if(hRes != DISP_E_MEMBERNOTFOUND)
      return hRes;

 SetErrorInfo(0, NULL);
 ITypeInfo* pInfo;
 hRes = GetTI(lcid, &pInfo);
 if (pInfo != NULL)
 {
      long ldid;
      unsigned int uTypeIndex;
      try
      {
         EnterCriticalSection(&_Module.m_csTypeInfoHolder);
         long idx = GetIdxFromCDID(dispidMember);
         _dispidEntry e = m_idispCont.at(idx);
         ldid = m_idispCont.at(idx).first;
         IID riid2 = m_idispCont.at(idx).second;
         LeaveCriticalSection(&_Module.m_csTypeInfoHolder);

         ITypeLib* pifTypeLib = NULL;
         hRes = pInfo->GetContainingTypeLib(&pifTypeLib, &uTypeIndex);
         if(pifTypeLib != NULL)
         {
            ITypeInfo* pifTypeInfo2 = NULL;
            hRes = pifTypeLib->GetTypeInfoOfGuid(riid2,&pifTypeInfo2);
            if(SUCCEEDED(hRes) && pifTypeInfo2 != NULL)
            {
               IDispatch* p2 = NULL;
               hRes = p->QueryInterface(riid2,(void**)&p2);
               if(p2 != NULL)
               {
              hRes = pifTypeInfo2->Invoke(p2, ldid, wFlags, pdispparams,
pvarResult, pexcepinfo, puArgErr);
                  p2->Release();
               }
               pifTypeInfo2->Release();
            }
            pifTypeLib->Release();
         }
      }
      catch(std::out_of_range)
      {
         return DISP_E_MEMBERNOTFOUND;
      }
  pInfo->Release();
 }
 return hRes;
}

template <class T, const IID* piid, const CLSID* pclsid, const GUID* plibid, WORD wMajor="1," WORD wMinor="0," class tihclass="CComTypeInfoHolderEx<piid,pclsid,plibid"> >
class ATL_NO_VTABLE IDispatchImplEx : public T
{
public:
 typedef tihclass _tihclass;
   IDispatchImplEx() {_tih.AddRef();}
 ~IDispatchImplEx() {_tih.Release();}

 STDMETHOD(GetTypeInfoCount)(UINT* pctinfo)
 {*pctinfo = 1; return S_OK;}

 STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo** pptinfo)
 {return _tih.GetTypeInfo(itinfo, lcid, pptinfo);}

 STDMETHOD(GetIDsOfNames)(REFIID riid, LPOLESTR* rgszNames, UINT cNames,
  LCID lcid, DISPID* rgdispid)
 {return _tih.GetIDsOfNames(riid, rgszNames, cNames, lcid, rgdispid);}

 STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid,
  LCID lcid, WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult,
  EXCEPINFO* pexcepinfo, UINT* puArgErr)
 {return _tih.Invoke((IDispatch*)this, dispidMember, riid, lcid,
  wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr);}
protected:
 static _tihclass _tih;
 static HRESULT GetTI(LCID lcid, ITypeInfo** ppInfo)
 {return _tih.GetTI(lcid, ppInfo);}
};

template <class T, const IID* piid, const CLSID* pclsid, const GUID* plibid, WORD wMajor="1," WORD wMinor="0," class tihclass="CComTypeInfoHolderEx<piid,pclsid,plibid"> >
IDispatchImplEx<T, piid, pclsid, plibid, wMajor, wMinor, tihclass>::_tihclass
IDispatchImplEx<T, piid, pclsid, plibid, wMajor, wMinor, tihclass>::_tih;

// Required by STL
inline bool operator <(const GUID g1, const GUID g2) { union { GUID g; struct { ULONG l1; ULONG l2; ULONG l3; ULONG l4; } l; } u1, u2; u1.g="g1;" u2.g="g2;" if(g1="=" g2) return false; if(u1.l.l1> u2.l.l1)
      return false;
   else if(u1.l.l1 <u2.l.l1) return true; else { if(u1.l.l2> u2.l.l2)
         return false;
      else if(u1.l.l2 <u2.l.l2) return true; else { if(u1.l.l3> u2.l.l3)
            return false;
         else if(u1.l.l3 <u2.l.l3) return true; else { if(u1.l.l4 < u2.l.l4) return true; else return false; } } } } //************************************************************************** * // Class CComClonableBase // Kjell Tangen, 1998 (kat@computas.no) // Computas, Norway // // Implements the IClassFactory interface for cloning. The class in particular // targets cloning of aggregates. Note that this class assumes that your object // is implemented using ATL. // // Intented usage: // 1. A clonable class (CMyClass) should inherit from CComClonableBase<CMyClass>
// 2. Declare and implement HRESULT Copy(CMyClass& source) in CMyClass
// 3. Add COM_INTERFACE_ENTRY(IClassFactory) to your interface map.
//    Important notice: This interface entry MUST BE PRIOR TO ANY AGGREGATE
//    DECLARATIONS where the aggregated object is also clonable.
//**************************************************************************
*
/*
Example:
class ATL_NO_VTABLE COuter :
 public CComCoClass<COuter, &CLSID_Outer>,
 public IMyObject,
   public CComClonableBase<COuter>,
 public CComObjectRootEx<CComSingleThreadModel>
{
   CComBSTR    m_bsExpression;
public:
   IUnknown*   m_pifInner; // Aggregated object

****Boiler-plate ATL-code removed for clarity****

BEGIN_COM_MAP(COuter)
 COM_INTERFACE_ENTRY(IMyObject)
   COM_INTERFACE_ENTRY(IClassFactory)
 COM_INTERFACE_ENTRY_AGGREGATE_BLIND( m_pifInner)
END_COM_MAP()

};

// Implementation:
STDMETHODIMP COuter::Copy(COuter& source)
{
   // Copy class members
   m_bsExpression = source.m_bsExpression;

   // Clone the aggregated object
   return = _CopyAggObject(source.m_pifInner, m_pifInner);
}
*/

template <class T>
class ATL_NO_VTABLE CComClonableBase :
   public IClassFactory
{
public:
   // IClassFactory methods:
   STDMETHOD(CreateInstance)(IUnknown* pifOuterUnk,
                                         REFIID riid,
                                         void** ppvObj)
   {
      HRESULT hr = E_UNEXPECTED;
      *ppvObj = NULL;
      if(pifOuterUnk)


         IUnknown* pifInnerUnk = NULL;
         // Clone should be part of an aggregate
         hr = CComCreator<CComAggObject<T> >::CreateInstance(pifOuterUnk,
                                                              riid,

(void**)&pifInnerUnk);
         if(pifInnerUnk)
         {
            CComAggObject<T>* pObject =
static_cast<CComAggObject<T>*>(pifInnerUnk);
            if(pObject)
            {
               hr = pObject->m_contained.Copy(static_cast<T&>(*this));
               *ppvObj = (void*)pifInnerUnk;
            }
         }
      }
      else
      {
         CComObject<T>* pObject = NULL;
         hr = CComObject<T>::CreateInstance(&pObject);
         if(pObject)
         {
            pObject->AddRef();
            hr = pObject->Copy(static_cast<T&>(*this));
            hr = pObject->QueryInterface(riid, ppvObj);
            pObject->Release();
         }
      }

      return hr;
   }
   STDMETHOD(LockServer)(BOOL fLock){return E_FAIL;}
protected:
   HRESULT _CopyAggObject(IUnknown* pifSourceUnk, IUnknown*& pifTargetUnk)
   {
      // This method is a utility to aggregate a clone of an object
      CComQIPtr<IClassFactory, &IID_IClassFactory> ifElementClonable;
      HRESULT hr;
      ifElementClonable = pifSourceUnk;
      // Release the target unknown first
      if(pifTargetUnk)
         pifTargetUnk->Release();
      hr =
ifElementClonable->CreateInstance(static_cast<T&>(*this).GetControllingUnkno
wn(),
                                             IID_IUnknown,
                                             (void**)&pifTargetUnk);
      return hr;
   }
   virtual HRESULT Copy(T& source) = 0;
};

#endif