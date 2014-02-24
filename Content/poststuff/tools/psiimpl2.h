// psiimpl2.h: IPersistStreamInitImpl2
// Copyright (c) 1998, Chris Sells
// All rights reserved.
// NO WARRANTIES EXTENDED. Use at your own risk.
//
// IPersistStreamInitImpl2 provides an implementation of GetSizeMax
// using the ATL property map. This is useful when implementing
// Marshal-by-Value.
//
// History:
//  6/29/99:
//   -Forget to use var.punkVal on 2nd QI in VT_UNKNOWN/VT_DISPATCH case.
//    Thanks to dcarlino@columbus.rr.com for pointing this out.
//
// 10/31/98:
//   -Tested by Dharma Shukla (v-dharsh@microsoft.com). Thanks!
//
// 10/27/98:
//   -Initial release.
//   -Only tested on longs and BSTRs.

#pragma once

#ifndef PSIIMPL2_H
#define PSIIMPL2_H

template <class T>
class ATL_NO_VTABLE IPersistStreamInitImpl2 : public IPersistStreamInitImpl<T>
{
public:
    STDMETHOD(GetSizeMax)(ULARGE_INTEGER* pcbSize)
    {
        long    cbSize = 0;
        HRESULT hr = S_OK;
        T*      pT = static_cast<T*>(this);
        
        CComPtr<IUnknown>   pUnk = pT->GetUnknown();
        ATL_PROPMAP_ENTRY*  pMap = T::GetPropertyMap();
        ATLASSERT(pMap != NULL);
        
        CComPtr<IDispatch>  pDispatch;
        const IID*          piidOld = NULL;
        for (int i = 0; pMap[i].pclsidPropPage != NULL; i++)
        {
            if (pMap[i].szDesc == NULL)
                continue;
            
            // check if raw data entry
            if (pMap[i].dwSizeData != 0)
            {
                cbSize += pMap[i].dwSizeData;
                continue;
            }
            
            CComVariant var;
            if (pMap[i].piidDispatch != piidOld)
            {
                pDispatch.Release();
                if (FAILED(pUnk->QueryInterface(*pMap[i].piidDispatch, (void**)&pDispatch)))
                {
                    ATLTRACE2(atlTraceCOM, 0, _T("Failed to get a dispatch pointer for property #%i\n"), i);
                    hr = E_FAIL;
                    break;
                }
                piidOld = pMap[i].piidDispatch;
            }
            
            if (FAILED(CComDispatchDriver::GetProperty(pDispatch, pMap[i].dispid, &var)))
            {
                ATLTRACE2(atlTraceCOM, 0, _T("Invoked failed on DISPID %x\n"), pMap[i].dispid);
                hr = E_FAIL;
                break;
            }
            
            int cbWrite = 0;
            switch (var.vt)
            {
            case VT_UNKNOWN:
            case VT_DISPATCH:
            {
                CComQIPtr<IPersistStream> spStream = var.punkVal;
                if( !spStream && var.punkVal ) var.punkVal->QueryInterface(IID_IPersistStreamInit, (void**)&spStream);
                if( spStream )
                {
                    cbSize += sizeof(CLSID);
                    ULARGE_INTEGER  uiSize = { 0 };
                    hr = spStream->GetSizeMax(&uiSize);
                    if (FAILED(hr))
                        return hr;
                    cbSize += uiSize.LowPart;
                }
            }
            case VT_UI1:
            case VT_I1:
                cbWrite = sizeof(BYTE);
                break;
            case VT_I2:
            case VT_UI2:
            case VT_BOOL:
                cbWrite = sizeof(short);
                break;
            case VT_I4:
            case VT_UI4:
            case VT_R4:
            case VT_INT:
            case VT_UINT:
            case VT_ERROR:
                cbWrite = sizeof(long);
                break;
            case VT_R8:
            case VT_CY:
            case VT_DATE:
                cbWrite = sizeof(double);
                break;
            default:
                break;
            }
            
            if (cbWrite != 0)
            {
                cbSize += cbWrite;
                continue;
            }
            
            CComBSTR    bstrWrite;
            CComVariant varBSTR;
            if (var.vt != VT_BSTR)
            {
                hr = VariantChangeType(&varBSTR, &var, VARIANT_NOVALUEPROP, VT_BSTR);
                if (FAILED(hr))
                    return hr;
                bstrWrite = varBSTR.bstrVal;
            }
            else
                bstrWrite = var.bstrVal;
            
            cbSize += 4 + (static_cast<BSTR>(bstrWrite) ? SysStringByteLen(bstrWrite) : 0) + 2;
        }
        
        if( SUCCEEDED(hr) )
        {
            pcbSize->HighPart = 0;
            pcbSize->LowPart = cbSize;
        }
        
        return hr;
    }
};

#endif // PSIIMPL2_H
