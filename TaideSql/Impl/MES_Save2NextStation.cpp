#include "StdAfx.h"
#include "MES_Save2NextStation.h"
#include "ActionApp.h"
#include "../drv/MesData.h"
#include <assert.h>
std::string UnicodeToANSI(const std::wstring & wstr);

//
IMPLEMENT_RUNTIME_CLASS(CMES_Save2NextStation)
//////////////////////////////////////////////////////////////////////////
CMES_Save2NextStation::CMES_Save2NextStation(void)
{
}

CMES_Save2NextStation::~CMES_Save2NextStation(void)
{
}

BOOL CMES_Save2NextStation::LoadXMLConfig(void)
{
    return TRUE;
}

bool mytrace(TCHAR *format, ...)

{

	TCHAR buffer[1000];

	va_list argptr;

	va_start(argptr, format);

	wvsprintf(buffer, format, argptr);

	va_end(argptr);

	OutputDebugString(buffer);

	return true;

}

SPRESULT CMES_Save2NextStation::__PollAction(void)
{
	int iRet = EECT_ERROR ; 
	
	//*////
	PRINTF_FMT_MSG_MUST("SPRESULT CMES_Save2NextStation::__PollAction(void)") ;
	//*////
	extern CActionApp  myApp;

	//2019-9-24 by PengWei
	//***因调用MT1、老化、MT2接口会覆盖MES通讯接口的地址映射，因此在调用MES通讯接口前重新申请地址空间***//
	if (SP_OK != GetMesDriverFromShareMemory())
	{
		m_pMesDrv = new CMesDriver();
		CHKRESULT(SetMesDriverIntoShareMemory());
	}
	string_t strDLLPath = (string_t)myApp.GetAppPath() + _T("\\..\\MES\\Taide\\MES_ATE_SQLSERVER.dll");
	
	std::string strPath = UnicodeToANSI(myApp.GetAppPath()) ;
	strPath = UnicodeToANSI(strDLLPath) ;
	CHKRESULT_WITH_NOTIFY(m_pMesDrv->StartupA(strPath) ? SP_OK : SP_E_LOAD_LIBRARY, "Load MES_ATE_SQLSERVER.dll");

	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");
	/*End by PengWei*/


	SPRESULT sp_result = SP_OK;
	char szSN[64] = {0};
	//sp_result = MES_GetSN(szSN,sizeof(szSN));
	sp_result = GetShareMemory(ShareMemory_SN1_TD, (void* )szSN, sizeof(szSN));
	//*////
	PRINTF_FMT_MSG_MUST("CMES_Save2NextStation() SN:%s ", szSN) ;
	//*////
	if (SP_OK != sp_result)
	{
		return sp_result;
	}

	char szPathIni[512]={0},szStation[128]={0};
	string_t strIniFile = (string_t)myApp.GetAppPath() + _T("\\..\\MES\\Taide\\MES.ini");
	strcpy_s(szPathIni, _T2CA(strIniFile.data()));
	GetPrivateProfileStringA("MES", "NextStation", "", szStation, sizeof(szStation), szPathIni);

	//*////
	PRINTF_FMT_MSG_MUST("Station:%s szPathIni:%s", szStation, szPathIni) ;
	//*////

	SPTEST_RESULT_T simba_result;
	sp_result = GetShareMemory(InternalReservedShareMemory_ItemTestResult, (void* )&simba_result, sizeof(simba_result));
	if (SP_OK != sp_result)
	{   
		return sp_result;
	}

	if (simba_result.errCode ==SP_OK)
	{
		char szMsg[512]={0},szRet[512]={0};

		PRINTF_FMT_MSG_MUST("Save2NextStation() szStation:%s szMsg:%s szRet:%s" , szStation, szMsg, szRet) ;
		iRet = m_pMesDrv->Save2NextStation(szSN, szStation, szMsg) ;

		PRINTF_FMT_MSG_MUST("Save2NextStation() iRet:%d sSn:%s szStation:%s szMsg:%s szRet:%s" , iRet, szSN, szStation, szMsg, szRet) ;
		if (EECT_OK != iRet)
		{		
			NOTIFY("MES_Save2NextStation", LEVEL_ITEM, 1, 0, 1, NULL, -1, "Msg:%s Ret:%s",szMsg, szRet);
			return SP_E_FAIL;
		}
		NOTIFY("MES_Save2NextStation", LEVEL_ITEM, 1, 1, 1, NULL, -1, "Success");
	}

	return SP_OK;
}

