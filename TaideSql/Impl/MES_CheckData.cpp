#include "StdAfx.h"
#include "MES_CheckData.h"
#include "ActionApp.h"
#include "../drv/MesData.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_CheckData)
//////////////////////////////////////////////////////////////////////////
CMES_CheckData::CMES_CheckData(void)
{
}

CMES_CheckData::~CMES_CheckData(void)
{
}

BOOL CMES_CheckData::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_CheckData::__PollAction(void)
{
	int iRet = EECT_ERROR ; 

	//*////
	PRINTF_FMT_MSG_MUST("SPRESULT CMES_CheckData::__PollAction(void)") ;
	//*////

	extern CActionApp  myApp;

	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");

	SPRESULT sp_result = SP_OK;
	ZeroMemory(m_InputSN,sizeof(m_InputSN));
	sp_result = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN));
	if (SP_OK != sp_result)
	{
		NOTIFY("MES_BindData", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_My_UserInputSN");
		return SP_E_FAIL;
	}

	char szSN1[128]={0};
	#if 0
	if (TRUE == m_pMesDrv->CMES_CheckData("IMEI1",m_InputSN[BC_IMEI1].szCode,"SN1",szSN1))
	#else 
	iRet = m_pMesDrv->QueryData("IMEI1",m_InputSN[BC_IMEI1].szCode,"SN1",szSN1) ;
	PRINTF_FMT_MSG_MUST("CMES_CheckData() iRet:%d IMEI1:%s SN1:%s " , iRet, m_InputSN[BC_IMEI1].szCode, szSN1) ;
	if (EECT_OK != iRet)
	#endif //  
	{
		NOTIFY("MES_CheckData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SN1:%s",szSN1);
		return SP_E_FAIL;
	}

	if (TRUE == m_pMesDrv->QueryData("BT",m_InputSN[BC_BT].szCode,"SN1",szSN1))
	{
		NOTIFY("MES_CheckData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SN1:%s",szSN1);
		return SP_E_FAIL;
	}

	if (TRUE == m_pMesDrv->QueryData("WIFI",m_InputSN[BC_WIFI].szCode,"SN1",szSN1))
	{
		NOTIFY("MES_CheckData(WIFI)", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SN1:%s",szSN1);
		return SP_E_FAIL;
	}

	if (TRUE == m_pMesDrv->QueryData("SN2",m_InputSN[BC_SN2].szCode,"SN1",szSN1))
	{
		NOTIFY("MES_CheckData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SN1:%s",szSN1);
		return SP_E_FAIL;
	}

	return SP_OK;
}

