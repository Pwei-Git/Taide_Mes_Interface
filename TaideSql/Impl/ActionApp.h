#pragma once

#include "SpatLibExport.h"
#include "Tr.h"
#include "ISpatAction.h"
#include <map>
#include <string>
#include "CSimpleLock.h"
using namespace std;
typedef map<SP_HANDLE, pISpatAction> CActionMap;

//////////////////////////////////////////////////////////////////////////
/// 
class CActionApp
{
public:
    CActionApp(void);
    virtual ~CActionApp(void);

    SPRESULT     Startup(HMODULE hApp, SPAT_DATA** lppData);
    void         Cleanup(void);

    SP_HANDLE    CreateAction(LPCWSTR lpFuncName, LPCWSTR lpClassName, LPVOID pLogUtil, SPRESULT& resCode);
    void         FreeAction (SP_HANDLE hAction);

    void         SetCallBack(SP_HANDLE hAction, LPCSPCALLBACK_PARAM lpCallBack);
    SPRESULT     SetDriver  (SP_HANDLE hAction, INT nDriv, LPVOID pDriv, LPVOID pParam);

    SPRESULT     InitAction (SP_HANDLE hAction, LPCSPAT_INIT_PARAM pParam);
    void         EnterAction(SP_HANDLE hAction);
    SPRESULT     PollAction (SP_HANDLE hAction);
    void         LeaveAction(SP_HANDLE hAction);
    SPRESULT     FinalAction(SP_HANDLE hAction);
    
    SPRESULT     SetProperty(SP_HANDLE hAction, INT nFlags, INT nOption, LPCVOID lpValue);
    SPRESULT     GetProperty(SP_HANDLE hAction, INT nFlags, INT nOption, LPVOID  lpValue);

    // 
    CRITICAL_SECTION& GetLock(void) { return m_Lock;      };
    LPCTSTR GetAppPath(void)const   { return m_szAppPath; };
    LPCTSTR GetSysPath(void)const   { return m_szSysPath; };

private:
    void         DeleteAppData(LPSPAT_DATA& lpData);
    LPSPAT_DATA  CreateAppData(HMODULE hApp); 
    pISpatAction GetSpatAction(SP_HANDLE hAction);
    void         InitSysPath(HMODULE hApp);

private:
    LPSPAT_DATA  m_lpAppData;
    CActionMap   m_hActionMap;
    CSimpleLock  m_Lock;
    std::wstring m_strAppVer;
    std::wstring m_strVendor;
    TCHAR        m_szAppPath[MAX_PATH];
    TCHAR        m_szSysPath[MAX_PATH];
};
