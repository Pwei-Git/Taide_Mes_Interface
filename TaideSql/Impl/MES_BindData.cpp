#include "StdAfx.h"
#include "MES_BindData.h"
#include "ActionApp.h"
#include "../drv/MesData.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_BindData)
//////////////////////////////////////////////////////////////////////////
CMES_BindData::CMES_BindData(void)
{
}

CMES_BindData::~CMES_BindData(void)
{
}

BOOL CMES_BindData::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_BindData::__PollAction(void)
{
	int iRet = EECT_ERROR ; 
	
	extern CActionApp  myApp;

	//*////
	PRINTF_FMT_MSG_MUST("SPRESULT CMES_BindData::__PollAction(void)") ;
	//*////

	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");

	SPRESULT sp_result = SP_OK;
	ZeroMemory(m_InputSN,sizeof(m_InputSN));
	sp_result = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN));
	if (SP_OK != sp_result)
	{
		NOTIFY("MES_BindData", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_My_UserInputSN");
		return SP_E_FAIL;
	}

	char szSN[64] = {0};
	//sp_result = MES_GetSN(szSN,sizeof(szSN));
	sp_result = GetShareMemory(ShareMemory_SN1_TD, (void* )szSN, sizeof(szSN));
	//*////
	PRINTF_FMT_MSG_MUST("sp_result:%d szSN:%s", sp_result, szSN) ;
	//*////
	if (SP_OK != sp_result)
	{
		return sp_result;
	}

	char szMsg[512]={0},szRet[512]={0};
	//*////
	PRINTF_FMT_MSG_MUST("BC_IMEI1:%s", m_InputSN[BC_IMEI1].szCode) ;
	//*////
	if (15== strlen(m_InputSN[BC_IMEI1].szCode))
	{
		iRet = m_pMesDrv->BindData(szSN, "seq", m_InputSN[BC_IMEI1].szCode, szMsg) ;
		//*////
		PRINTF_FMT_MSG_MUST("BindData: iRet: %d IMEI : ", iRet, m_InputSN[BC_IMEI1].szCode) ;
		//*////
		if (EMRT_OK != iRet)
		{
			NOTIFY("MES_BindData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "IMEI1:%s Msg:%s", m_InputSN[BC_IMEI1].szCode, szMsg);
			return SP_E_FAIL;
		}
		NOTIFY("MES_BindData", LEVEL_ITEM, 1, 1, 1, NULL, -1, "IMEI1:%s",m_InputSN[BC_IMEI1].szCode);
	}
	
	#if 1
	//*////
	PRINTF_FMT_MSG_MUST("BT:%s", m_InputSN[BC_BT].szCode) ;
	//*////
	if (12== strlen(m_InputSN[BC_BT].szCode))
	{
		iRet = m_pMesDrv->BindData(szSN, "BTmac", m_InputSN[BC_BT].szCode, szMsg) ;
		//*////
		PRINTF_FMT_MSG_MUST("BindData: iRet: %d BT_MAC: %s", iRet, m_InputSN[BC_BT].szCode) ;
		//*////
		if (EMRT_OK != iRet)
		{
			NOTIFY("MES_BindData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "BT:%s Msg:%s", m_InputSN[BC_BT].szCode, szMsg);
			return SP_E_FAIL;
		}
		NOTIFY("MES_BindData", LEVEL_ITEM, 1, 1, 1, NULL, -1, "BT:%s",m_InputSN[BC_BT].szCode);
	}
	#endif // 
	
	//*////
	PRINTF_FMT_MSG_MUST("WIFI:%s", m_InputSN[BC_WIFI].szCode) ;
	//*////
	if (12== strlen(m_InputSN[BC_WIFI].szCode))
	{
		iRet = m_pMesDrv->BindData(szSN, "wifimac", m_InputSN[BC_WIFI].szCode, szMsg) ;
		//*////
		PRINTF_FMT_MSG_MUST("BindData: iRet: %d WIFI_MAC: %s", iRet, m_InputSN[BC_WIFI].szCode) ;
		//*////
		if (EMRT_OK != iRet)
		{
			NOTIFY("MES_BindData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "WIFI:%s Msg:%s", m_InputSN[BC_WIFI].szCode, szMsg);
			return SP_E_FAIL;
		}
		NOTIFY("MES_BindData", LEVEL_ITEM, 1, 1, 1, NULL, -1, "WIFI:%s",m_InputSN[BC_WIFI].szCode);
	}

	#if 0 
	//*////
	PRINTF_FMT_MSG_MUST("SN2:%s", m_InputSN[BC_SN2].szCode) ;
	//*////
	if (strlen(m_InputSN[BC_SN2].szCode)>0)
	{
		iRet = m_pMesDrv->BindData(szSN, "SN2", m_InputSN[BC_SN2].szCode, szMsg, szRet) ;
		//*////
		PRINTF_FMT_MSG_MUST("BindData: iRet: %d szMsg: %s szRet:%s", iRet, szMsg, szRet) ;
		//*////
		if (EMRT_OK != iRet)
		{
			NOTIFY("MES_BindData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SN2:%s,Ret:%s",m_InputSN[BC_SN2].szCode,szRet);
			return SP_E_FAIL;
		}
		NOTIFY("MES_BindData", LEVEL_ITEM, 1, 1, 1, NULL, -1, "SN2:%s",m_InputSN[BC_SN2].szCode);
	}
	#endif // 

	//*////
	PRINTF_FMT_MSG_MUST("BC_SN2:%s", m_InputSN[BC_SN1].szCode) ;
	//*////
	if (strlen(m_InputSN[BC_SN1].szCode)>0)
	{
		iRet = m_pMesDrv->BindData(szSN, "ksn", m_InputSN[BC_SN1].szCode, szMsg) ;
		//*////
		PRINTF_FMT_MSG_MUST("BindData: iRet: %d szMsg: %s ", iRet, szMsg) ;
		//*////
		if (EMRT_OK != iRet)
		{
			NOTIFY("MES_BindData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SN2:%s",m_InputSN[BC_SN1].szCode);
			return SP_E_FAIL;
		}
		NOTIFY("MES_BindData", LEVEL_ITEM, 1, 1, 1, NULL, -1, "SN2:%s",m_InputSN[BC_SN1].szCode);
	}

	return SP_OK;
}

