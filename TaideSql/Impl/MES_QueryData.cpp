#include "StdAfx.h"
#include "MES_QueryData.h"
#include "ActionApp.h"
#include "../drv/MesData.h"
//
IMPLEMENT_RUNTIME_CLASS(CMES_QueryData)
//////////////////////////////////////////////////////////////////////////
CMES_QueryData::CMES_QueryData(void)
{
}

CMES_QueryData::~CMES_QueryData(void)
{
}

BOOL CMES_QueryData::LoadXMLConfig(void)
{
    return TRUE;
}

SPRESULT CMES_QueryData::__PollAction(void)
{
	int iRet = EECT_ERROR ; 

	//*////
	PRINTF_FMT_MSG_MUST("SPRESULT CMES_QueryData::__PollAction(void)") ;
	//*////

	extern CActionApp  myApp;

	CHKRESULT_WITH_NOTIFY(GetMesDriverFromShareMemory(), "GetMesDriver");

	SPRESULT sp_result = SP_OK;
	char szSN[64] = {0};
	//sp_result = MES_GetSN(szSN,sizeof(szSN));
	sp_result = GetShareMemory(ShareMemory_SN1_TD, (void* )szSN, sizeof(szSN));
	if (SP_OK != sp_result)
	{
		return sp_result;
	}

	ZeroMemory(m_InputSN,sizeof(m_InputSN));
	sp_result = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN));
	if (SP_OK != sp_result)
	{
		NOTIFY("MES_BindData", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_My_UserInputSN");
		return SP_E_FAIL;
	}

	char szIMEI1[128]={0},szBT[128]={0},szWIFI[128]={0},szSN2[128]={0}, szMsg[128]={0}, szRet[128]={0};
	PRINTF_FMT_MSG_MUST("QueryData() SN1 " ) ;
	iRet = m_pMesDrv->QueryData(szSN, "seq", szIMEI1, szMsg) ;
	PRINTF_FMT_MSG_MUST("QueryData() iRet:%d SN1:%s IMEI:%s" , iRet, szSN, szIMEI1) ;
	if (EECT_OK != iRet)
	{
		NOTIFY("MES_QueryData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SN1:%s Msg:%s", szSN, szMsg);
		return SP_E_FAIL;
	}
	if (strcmp(m_InputSN[BC_IMEI1].szCode,szIMEI1)!=0)
	{
		NOTIFY("MES_QueryData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SCAN IMEI1:%s,MES IMEI1:%s",m_InputSN[BC_IMEI1].szCode,szIMEI1);
	}
	else
	{
		NOTIFY("MES_QueryData", LEVEL_ITEM, 1, 1, 1, NULL, -1, "SCAN IMEI1:%s,MES IMEI1:%s",m_InputSN[BC_IMEI1].szCode,szIMEI1);
	}

	memset(szBT, 0x00, sizeof(szBT)) ;
	memset(szMsg, 0x00, sizeof(szMsg)) ;
	PRINTF_FMT_MSG_MUST("QueryData() BT " ) ;
	iRet = m_pMesDrv->QueryData(szSN, "BTmac", szBT, szMsg) ;
	PRINTF_FMT_MSG_MUST("QueryData() iRet:%d SN1:%s BT_MAC:%s" , iRet, szSN, szBT) ;
	if (EECT_OK != iRet)
	{
		NOTIFY("MES_QueryData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SN1:%s Msg:%s",szSN, szMsg);
		return SP_E_FAIL;
	}
	if (strcmp(m_InputSN[BC_BT].szCode,szBT)!=0)
	{
		NOTIFY("MES_QueryData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SCAN BT:%s,MES BT:%s",m_InputSN[BC_BT].szCode,szBT);
	}
	else
	{
		NOTIFY("MES_QueryData", LEVEL_ITEM, 1, 1, 1, NULL, -1, "SCAN BT:%s,MES BT:%s",m_InputSN[BC_BT].szCode,szBT);
	}

	memset(szWIFI, 0x00, sizeof(szWIFI)) ;
	memset(szMsg, 0x00, sizeof(szMsg)) ;
	PRINTF_FMT_MSG_MUST("QueryData() WIFI " ) ;
	iRet = m_pMesDrv->QueryData(szSN,"wifimac", szWIFI, szMsg) ;
	PRINTF_FMT_MSG_MUST("QueryData() iRet:%d SN1:%s WIFI_MAC:%s" , iRet, szSN, szWIFI) ;
	if (EECT_OK != iRet)
	{
		NOTIFY("MES_QueryData(WIFI)", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SN1:%s Msg:%s",szSN, szMsg);
		return SP_E_FAIL;
	}
	if (strcmp(m_InputSN[BC_WIFI].szCode,szWIFI)!=0)
	{
		NOTIFY("MES_QueryData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SCAN WIFI:%s,MES WIFI:%s",m_InputSN[BC_WIFI].szCode,szWIFI);
	}
	else
	{
		NOTIFY("MES_QueryData", LEVEL_ITEM, 1, 1, 1, NULL, -1, "SCAN WIFI:%s,MES WIFI:%s",m_InputSN[BC_WIFI].szCode,szWIFI);
	}

	#if 1 
	PRINTF_FMT_MSG_MUST("QueryData() SN2 " ) ;
	iRet = m_pMesDrv->QueryData(szSN, "ksn", szSN2, szMsg) ;
	PRINTF_FMT_MSG_MUST("QueryData() iRet:%d SN1:%s szMsg:%s szRet:%s" , iRet, szSN2, szMsg, szRet) ;
	if (EECT_OK != iRet)
	{
		NOTIFY("MES_QueryData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SN1:%s",szSN2);
		return SP_E_FAIL;
	}
	if (strcmp(m_InputSN[BC_SN1].szCode,szSN2)!=0)
	{
		NOTIFY("MES_QueryData", LEVEL_ITEM, 1, 0, 1, NULL, -1, "SCAN SN2:%s,MES SN2:%s",m_InputSN[BC_SN1].szCode,szSN2);
	}
	else
	{
		NOTIFY("MES_QueryData", LEVEL_ITEM, 1, 1, 1, NULL, -1, "SCAN SN2:%s,MES SN2:%s",m_InputSN[BC_SN1].szCode,szSN2);
	}
	#endif // 

	return SP_OK;
}

