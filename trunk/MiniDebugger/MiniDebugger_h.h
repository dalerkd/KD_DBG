

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0595 */
/* at Sat Jun 10 12:29:52 2017
 */
/* Compiler settings for MiniDebugger.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0595 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __MiniDebugger_h_h__
#define __MiniDebugger_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMiniDebugger_FWD_DEFINED__
#define __IMiniDebugger_FWD_DEFINED__
typedef interface IMiniDebugger IMiniDebugger;

#endif 	/* __IMiniDebugger_FWD_DEFINED__ */


#ifndef __MiniDebugger_FWD_DEFINED__
#define __MiniDebugger_FWD_DEFINED__

#ifdef __cplusplus
typedef class MiniDebugger MiniDebugger;
#else
typedef struct MiniDebugger MiniDebugger;
#endif /* __cplusplus */

#endif 	/* __MiniDebugger_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __MiniDebugger_LIBRARY_DEFINED__
#define __MiniDebugger_LIBRARY_DEFINED__

/* library MiniDebugger */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_MiniDebugger;

#ifndef __IMiniDebugger_DISPINTERFACE_DEFINED__
#define __IMiniDebugger_DISPINTERFACE_DEFINED__

/* dispinterface IMiniDebugger */
/* [uuid] */ 


EXTERN_C const IID DIID_IMiniDebugger;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("C52201EB-DF98-488E-A102-3DBDD26CCE56")
    IMiniDebugger : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IMiniDebuggerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMiniDebugger * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMiniDebugger * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMiniDebugger * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IMiniDebugger * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IMiniDebugger * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IMiniDebugger * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IMiniDebugger * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IMiniDebuggerVtbl;

    interface IMiniDebugger
    {
        CONST_VTBL struct IMiniDebuggerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMiniDebugger_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMiniDebugger_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMiniDebugger_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMiniDebugger_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IMiniDebugger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IMiniDebugger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IMiniDebugger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IMiniDebugger_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_MiniDebugger;

#ifdef __cplusplus

class DECLSPEC_UUID("5B6EF2E1-EAE5-41CE-9BC7-8F2AD2E42F5D")
MiniDebugger;
#endif
#endif /* __MiniDebugger_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


