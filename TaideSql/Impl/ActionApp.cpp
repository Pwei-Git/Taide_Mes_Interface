#include "StdAfx.h"
#include "ActionApp.h"
#include "SpatBase.h"
#include "SpatRuntimeClass.h"
#include "CLocks.h"
#include "AppVer.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////////
#define ACTION(FnTag, FnSubName, ClassName, ActionName, rcID, rcName, Description)        FnTag
static LPCWSTR FNTAG_LIST[] =
{
#include "ActionList.h"
};
#undef ACTION

#define ACTION(FnTag, FnSubName, ClassName, ActionName, rcID, rcName, Description)       ClassName
static LPCWSTR CLASS_LIST[] =
{
#include "ActionList.h"
};
#undef ACTION

#define ACTION(FnTag, FnSubName, ClassName, ActionName, rcID, rcName, Description)       ActionName
static LPCWSTR ACTNAME_LIST[] =
{
#include "ActionList.h"
};
#undef ACTION

#define ACTION(FnTag, FnSubName, ClassName, ActionName, rcID, rcName, Description)       rcID
static UINT32 RCID_LIST[] =
{
#include "ActionList.h"
};
#undef ACTION

#define ACTION(FnTag, FnSubName, ClassName, ActionName, rcID, rcName, Description)       rcName
static LPCWSTR RCNAME_LIST[] =
{
#include "ActionList.h"
};
#undef ACTION

#define ACTION(FnTag, FnSubName, ClassName, ActionName, rcID, rcName, Description)       Description
static LPCWSTR DESCIPION_LIST[] =
{
#include "ActionList.h"
};
#undef ACTION

#define ACTION(FnTag, FnSubName, ClassName, ActionName, rcID, rcName, Description)       FnSubName
static LPCWSTR SUBNAME_LIST[] =
{
#include "ActionList.h"
};
#undef ACTION


//////////////////////////////////////////////////////////////////////////
static LPBYTE WINAPI LoadXML(HMODULE hApp, WORD wID, UINT32& dwSize)
{
    HRSRC hr = ::FindResource(hApp, MAKEINTRESOURCE(wID), _T("XML"));
    if (NULL == hr)
    {  
        return NULL;
    }

    dwSize = ::SizeofResource(hApp, hr);
    if (0 == dwSize)
    {
        return NULL;
    }

    HGLOBAL hg = ::LoadResource(hApp, hr);
    if (NULL == hg)
    {
        return NULL;
    }

    return (LPBYTE)::LockResource(hg);
}


//////////////////////////////////////////////////////////////////////////
CActionApp::CActionApp(void)
: m_lpAppData(NULL)
{
}

CActionApp::~CActionApp(void)
{
}


void CActionApp::DeleteAppData(LPSPAT_DATA& lpData)
{
    if (NULL == lpData)
    {
        return ;
    }

    if (NULL != lpData->lpFuncList)
    {
        for (UINT32 i=0; i<lpData->u32Count; i++)
        {
            if (NULL !=  lpData->lpFuncList[i].lpActionList)
            {
                delete []lpData->lpFuncList[i].lpActionList;
                lpData->lpFuncList[i].lpActionList = NULL;
            }
        }

        delete []lpData->lpFuncList;
        lpData->lpFuncList = NULL;
    }

    delete lpData;
    lpData = NULL;
}

LPSPAT_DATA CActionApp::CreateAppData(HMODULE hApp)
{
    LPSPAT_DATA lpData = NULL;
    try
    {
        lpData  = new SPAT_DATA;

        size_t i = 0; 
        size_t j = 0;
        const size_t MAX_COUNT = ARRAY_SIZE(FNTAG_LIST);

        size_t nFnCount = 0;
        size_t nFnIndex[MAX_COUNT] = {0};
        for (i=0; i<MAX_COUNT; i++)
        {
            for (j=0; j<nFnCount; j++)
            {
                if (!_wcsicmp(FNTAG_LIST[i], FNTAG_LIST[nFnIndex[j]])) 
                {
                    break;
                }
            }

            if (nFnCount == j || 0 == i)
            {
                nFnIndex[nFnCount] = i;
                nFnCount++;
            }
        }

        lpData->u32Count = static_cast<UINT32>(nFnCount);
        lpData->lpFuncList = new SPAT_FUNCTION[nFnCount];

        for (i=0; i<nFnCount; i++)
        {
            lpData->lpFuncList[i].lpFnName = const_cast<LPWSTR>(FNTAG_LIST[nFnIndex[i]]);
        
            size_t nActionCount = 0;
            size_t nActionIndex[MAX_COUNT] = {0};

            for (j=0; j<MAX_COUNT; j++)
            {
                if (!_wcsicmp(FNTAG_LIST[j], FNTAG_LIST[nFnIndex[i]])) 
                {	
                    nActionIndex[nActionCount] = j;
                    nActionCount++;
                }
            }
            lpData->lpFuncList[i].u32Count = static_cast<UINT32>(nActionCount);
            lpData->lpFuncList[i].lpActionList = new SPAT_ACTION[nActionCount];

            LPSPAT_ACTION lpActionList = lpData->lpFuncList[i].lpActionList;
            for (j=0; j<nActionCount; j++)
            {
                size_t k = nActionIndex[j];
                lpActionList[j].u32Version    = 0x0101;
                lpActionList[j].lpClassName   = const_cast<LPWSTR>(CLASS_LIST[k]);
                lpActionList[j].lpActionName  = const_cast<LPWSTR>(ACTNAME_LIST[k]);
                lpActionList[j].lpDescription = const_cast<LPWSTR>(DESCIPION_LIST[k]);
                lpActionList[j].lpSubFnName   = const_cast<LPWSTR>(SUBNAME_LIST[k]);
                if (0 == RCID_LIST[k])
                {
                    lpActionList[j].rc.u32ID      =   IDR_XML_DUMMY;
                    lpActionList[j].rc.lpName     = L"IDR_XML_DUMMY";
                }
                else
                {
                    lpActionList[j].rc.u32ID      = RCID_LIST[k];
                    lpActionList[j].rc.lpName     = const_cast<LPWSTR>(RCNAME_LIST[k]);
                }
                lpActionList[j].rc.lpData     = LoadXML(hApp, static_cast<WORD>(lpActionList[j].rc.u32ID), lpActionList[j].rc.u32Size);
                if (NULL == lpActionList[j].rc.lpData || 0 == lpActionList[j].rc.u32Size)
                {
                    DeleteAppData(lpData);
                    assert(0);
                    return NULL;
                }
            }
        }
    }
    catch (const std::bad_alloc& /*e*/)
    {
        DeleteAppData(lpData);
        return NULL;
    }

    return lpData;
}

SPRESULT CActionApp::Startup(HMODULE hApp, SPAT_DATA** lppData)
{ 
    if (NULL == lppData)
    {
        assert(0);
        return SP_E_SPAT_INVALID_PARAMETER;
    }

    if (NULL == m_lpAppData)
    {   
        InitSysPath(hApp);

        CLocks Lock(m_Lock);
        m_hActionMap.clear();

        m_lpAppData = CreateAppData(hApp);
        if (NULL == m_lpAppData)
        {
            assert(0);
            return SP_E_SPAT_STARTUP;
        }

        CAppVer theVer;
        theVer.Init(hApp);
        m_strAppVer = theVer.GetVersion();
        m_strVendor = theVer.GetVendor();
        m_lpAppData->lpFileVersion = (LPWSTR)m_strAppVer.c_str();
        m_lpAppData->lpVendorInfo  = (LPWSTR)m_strVendor.c_str();
    }
    
    *lppData = m_lpAppData;

    return SP_OK;
}

void CActionApp::Cleanup(void)
{
    CLocks Lock(m_Lock);

    if (m_hActionMap.size() > 0)
    {
        CActionMap::iterator it = m_hActionMap.begin();
        for (; it != m_hActionMap.end(); ++it)
        {
            pISpatAction lpAction = it->second;
            if (NULL != lpAction)
            {
                lpAction->Release();
                lpAction = NULL;
            }
        }
   
        m_hActionMap.clear();
    }

    DeleteAppData(m_lpAppData);
}

SP_HANDLE CActionApp::CreateAction(
   LPCWSTR lpFuncName, LPCWSTR lpClassName, LPVOID pLogUtil, SPRESULT& resCode
   )
{
#define SAFE_LOG_PTR  if (NULL != pLogUtil)  ((ISpLog* )pLogUtil)
   
    resCode = SP_OK;

    if (NULL == lpFuncName || NULL == lpClassName)
    {
        resCode = SP_E_SPAT_INVALID_PARAMETER;
        SAFE_LOG_PTR->LogFmtStrA(SPLOGLV_ERROR, "%s: Invalid parameter!", __FUNCTION__);
        return INVALID_NPI_HANDLE;
    }
    
    BOOL bFound = FALSE;
    for (UINT32 i=0; i<m_lpAppData->u32Count; i++)
    {
       // if (!_wcsicmp(m_lpAppData->lpFuncList[i].lpFnName, lpFuncName))
        {
            for (UINT32 j=0; j<m_lpAppData->lpFuncList[i].u32Count; j++)
            {
                if (!_wcsicmp(m_lpAppData->lpFuncList[i].lpActionList[j].lpClassName, lpClassName))
                {
                    bFound = TRUE;
                    break;
                }
            }
        }
    }

    if (!bFound)
    {
        resCode = SP_E_SPAT_ACTION_NOT_EXIST;
        SAFE_LOG_PTR->LogFmtStrW(SPLOGLV_ERROR, L"%s - %s not exist!", lpFuncName, lpClassName);
        return INVALID_NPI_HANDLE;
    }

    CSpatRuntimeClass*  lpClassObject = SPAT_RUNTIME_CLASS(CSpatBase);
    assert(lpClassObject);

    SP_HANDLE hAction = lpClassObject->CreateObject(lpClassName);
    if (INVALID_NPI_HANDLE == hAction)
    {
        resCode = SP_E_SPAT_CREATE_OBJECT;
        SAFE_LOG_PTR->LogFmtStrW(SPLOGLV_ERROR, L"CreateObject(%s) fails!", lpClassName);
        return INVALID_NPI_HANDLE;    
    }
    else
    {
        ((CSpatBase* )hAction)->InitLog(pLogUtil, SPLOGLV_INFO);

        CLocks Lock(m_Lock);
        m_hActionMap.insert(make_pair(hAction, (pISpatAction)hAction));
        return hAction;
    }
}

pISpatAction CActionApp::GetSpatAction(SP_HANDLE hAction)
{
    if (INVALID_NPI_HANDLE != hAction)
    {
        CActionMap::iterator it = m_hActionMap.find(hAction);
        if (it != m_hActionMap.end())
        {
            return it->second;
        }
    }

    return NULL;
}

void CActionApp::FreeAction(SP_HANDLE hAction)
{
    if (INVALID_NPI_HANDLE == hAction)
    {
        return ;
    }

    CLocks Lock(m_Lock);
    CActionMap::iterator it = m_hActionMap.find(hAction);
    if (it != m_hActionMap.end())
    {
        pISpatAction lpAction = it->second;
        if (NULL !=  lpAction)
        {
            lpAction->Release();
        }

        /// remove from list
        m_hActionMap.erase(it);
    }
}

void CActionApp::SetCallBack(SP_HANDLE hAction, LPCSPCALLBACK_PARAM lpCallBack)
{
    pISpatAction lpAction = GetSpatAction(hAction);
    if (NULL != lpAction)
    {
        lpAction->SetCallBack(lpCallBack);
    }
}

SPRESULT CActionApp::SetDriver(SP_HANDLE hAction, INT nDriv, LPVOID pDriv, LPVOID pParam)
{
    pISpatAction lpAction = GetSpatAction(hAction);
    if (NULL  == lpAction)
    {
        return SP_E_SPAT_INVALID_HANDLE;
    }
    else
    {
        return lpAction->SetDriver(nDriv, pDriv, pParam);
    }
}

SPRESULT CActionApp::InitAction(SP_HANDLE hAction, LPCSPAT_INIT_PARAM pParam)
{
    pISpatAction lpAction = GetSpatAction(hAction);
    if (NULL  == lpAction)
    {
        return SP_E_SPAT_INVALID_HANDLE;
    }
    else
    {
        return lpAction->InitAction(pParam);
    }
}

void CActionApp::EnterAction(SP_HANDLE hAction)
{
    pISpatAction lpAction = GetSpatAction(hAction);
    if (NULL  != lpAction)
    {
        lpAction->EnterAction();
    }
}

SPRESULT CActionApp::PollAction(SP_HANDLE hAction)
{
    pISpatAction lpAction = GetSpatAction(hAction);
    if (NULL  == lpAction)
    {
        return SP_E_SPAT_INVALID_HANDLE;
    }
    else
    {
        return lpAction->PollAction();
    }
}

void CActionApp::LeaveAction(SP_HANDLE hAction)
{
    pISpatAction lpAction = GetSpatAction(hAction);
    if (NULL  != lpAction)
    {
        lpAction->LeaveAction();
    }
}

SPRESULT CActionApp::FinalAction(SP_HANDLE hAction)
{
    pISpatAction lpAction = GetSpatAction(hAction);
    if (NULL  == lpAction)
    {
        return SP_E_SPAT_INVALID_HANDLE;
    }
    else
    {
        return lpAction->FinalAction();
    }
}

SPRESULT CActionApp::SetProperty(SP_HANDLE hAction, INT nFlags, INT nOption, LPCVOID lpValue)
{
    pISpatAction lpAction = GetSpatAction(hAction);
    if (NULL  == lpAction)
    {
        return SP_E_SPAT_INVALID_HANDLE;
    }
    else
    {
        return lpAction->SetProperty(nFlags, nOption, lpValue);
    }
}

SPRESULT CActionApp::GetProperty(SP_HANDLE hAction, INT nFlags, INT nOption, LPVOID lpValue)
{
    pISpatAction lpAction = GetSpatAction(hAction);
    if (NULL  == lpAction)
    {
        return SP_E_SPAT_INVALID_HANDLE;
    }
    else
    {
        return lpAction->GetProperty(nFlags, nOption, lpValue);
    }
}

void CActionApp::InitSysPath(HMODULE hApp)
{
    /* The path of SpatLib.dll */
    GetModuleFileName(hApp,  m_szAppPath, MAX_PATH);
    LPTSTR lpChar = _tcsrchr(m_szAppPath, _T('\\'));
    if (NULL != lpChar)
    {
        *lpChar = _T('\0');
    }

    /* The path of *.exe path */
    GetModuleFileName(NULL,  m_szSysPath, MAX_PATH);
    lpChar = _tcsrchr(m_szSysPath, _T('\\'));
    if (NULL != lpChar)
    {
        *lpChar = _T('\0');
    }
    _tcscat_s(m_szSysPath, L"\\sys");

    if (::CreateDirectory(m_szSysPath, NULL))
    {
        SetFileAttributes(m_szSysPath, FILE_ATTRIBUTE_HIDDEN);
    }
}