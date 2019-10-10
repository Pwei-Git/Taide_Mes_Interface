#include "StdAfx.h"
#include "MES_CheckRoute.h"
#include "ActionApp.h"
#include "../drv/MesData.h"

#include "../drv/MesDriver.h"


std::string UnicodeToANSI(const std::wstring & wstr)
{
    std::string ret;
    std::mbstate_t state = {0};
    const wchar_t *src = wstr.data();
    size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
    if (static_cast<size_t>(-1) != len) {
        std::unique_ptr< char [] > buff(new char[len + 1]);
        len = std::wcsrtombs(buff.get(), &src, len, &state);
        if (static_cast<size_t>(-1) != len) {
            ret.assign(buff.get(), len);
        }
    }
    return ret;
}

std::wstring ANSIToUnicode(const std::string & str)
{
    std::wstring ret;
    std::mbstate_t state = {0};
    const char *src = str.data();
    size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
    if (static_cast<size_t>(-1) != len) {
        std::unique_ptr< wchar_t [] > buff(new wchar_t[len + 1]);
        len = std::mbsrtowcs(buff.get(), &src, len, &state);
        if (static_cast<size_t>(-1) != len) {
            ret.assign(buff.get(), len);
        }
    }
    return ret;
}


//
IMPLEMENT_RUNTIME_CLASS(CMES_CheckRoute)
//////////////////////////////////////////////////////////////////////////
CMES_CheckRoute::CMES_CheckRoute(void)
:  m_eUpload(SYNC)
{
}

CMES_CheckRoute::~CMES_CheckRoute(void)
{
}

BOOL CMES_CheckRoute::LoadXMLConfig(void)
{
    std::wstring strUpload = GetConfigValue(L"Option:Upload", L"Sync");
    if (strUpload == L"Async")
    {
        m_eUpload = ASYNC;
    }
    else if (strUpload == L"Sync")
    {
        m_eUpload = SYNC;
    }
    else
    {
        m_eUpload = NONE;
    }
    return TRUE;
}


SPRESULT CMES_CheckRoute::__PollAction(void)
{
	int iRet = EECT_ERROR ; 

	
	//*////
	PRINTF_FMT_MSG_MUST("SPRESULT CMES_CheckRoute::__PollAction(void)") ;
	//*////
	
	extern CActionApp  myApp;
	if (SP_OK != GetMesDriverFromShareMemory())
	{
		m_pMesDrv = new CMesDriver();
		CHKRESULT(SetMesDriverIntoShareMemory());
	}

//	string_t strDrvPath = (string_t)myApp.GetAppPath() + _T("\\..\\MES\\Taide\\MES_ATE_SOAP.dll");
	string_t strDrvPath = (string_t)myApp.GetAppPath() + _T("\\..\\MES\\Taide\\MES_ATE_SQLSERVER.dll");

	//*////
	std::string strPath = UnicodeToANSI(myApp.GetAppPath()) ;
	PRINTF_FMT_MSG_MUST("DllPath:%s", strPath.c_str()) ;
	strPath = UnicodeToANSI(strDrvPath) ;
	PRINTF_FMT_MSG_MUST("strDrvPath:%s", strPath.c_str()) ;
	PRINTF_FMT_MSG_MUST("m_pMesDrv:0x%08x", m_pMesDrv) ;
	//*////
	
	//CHKRESULT_WITH_NOTIFY(m_pMesDrv->Startup(strDrvPath) ? SP_OK : SP_E_LOAD_LIBRARY, "Load MES_ATE_SOAP_HTTP.dll");
	CHKRESULT_WITH_NOTIFY(m_pMesDrv->StartupA(strPath) ? SP_OK : SP_E_LOAD_LIBRARY, "Load MES_ATE_SQLSERVER.dll");

	char szPathIni[512]={0},szStation[128]={0};
	string_t strIniFile = (string_t)myApp.GetAppPath() + _T("\\..\\MES\\Taide\\MES.ini");
	strcpy_s(szPathIni, _T2CA(strIniFile.data()));
	GetPrivateProfileStringA("MES", "Station", "", szStation, sizeof(szStation), szPathIni);


	SPRESULT sp_result = SP_OK;
	char szSN[64] = {0};
	sp_result = MES_GetSN(szSN,sizeof(szSN));
	//*////
	PRINTF_FMT_MSG_MUST("Station:%s szPathIni:%s" , szStation, szPathIni) ;
	PRINTF_FMT_MSG_MUST("MES_GetSN() result:%d szSN:%s" , sp_result, szSN) ;
	//*////
	if (SP_OK != sp_result)
	{
		return sp_result;
	}
	
{
	SetShareMemory(ShareMemory_SN1_TD, (const void* )szSN, sizeof(szSN),IContainer::System);
}

	char szMsg[512]={0},szRet[512]={0};

	memset(szMsg, 0x00, sizeof(szMsg)) ; 
	memset(szRet, 0x00, sizeof(szRet)) ; 
	
	PRINTF_FMT_MSG_MUST("CheckRoute(0) szStation:%s szMsg:%s szRet:%s m_pMesDrv:0x%08x " , szStation, szMsg, szRet, m_pMesDrv) ;
	iRet = m_pMesDrv->CheckRoute(szSN, szStation, szMsg) ;
	
	PRINTF_FMT_MSG_MUST("CheckRoute(1) iRet:%d szStation:%s szMsg:%s" , iRet, szStation, szMsg) ;
	if (EECT_OK != iRet)
	{
		
		NOTIFY("MES_CheckRoute", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "Msg:%s",szMsg);
		return SP_E_SPDB_SERVER_CONNECT;
	}
	NOTIFY("MES_CheckRoute", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	//以下代码用于存储Simba界面显示的内部，具体存在在ShareMemoryItemDataObserver
    if (NONE != m_eUpload)
    {
		CMesData* pMesData = NULL;
		if ((SP_OK == GetShareMemory(ShareMemoryItemDataObserver, (void* )&pMesData, sizeof(pMesData))) && (NULL != pMesData))
		{
			delete pMesData;
			pMesData = NULL;
		}
		pMesData = new CMesData();
        if (ASYNC == m_eUpload)
        {
            CHKRESULT_WITH_NOTIFY(pMesData->StartUpload(TRUE) ? SP_OK : SP_E_SPDB_START_TEST, "StartUploading");
        }
        IItemDataObserver* pObs = (IItemDataObserver* )pMesData;
        CHKRESULT(SetShareMemory(ShareMemoryItemDataObserver, (const void* )&pObs, sizeof(pObs), IContainer::System));
    }
    return SP_OK;
}


