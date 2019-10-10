#include "StdAfx.h"
#include "ImpBase.h"
#include "ActionApp.h"
//////////////////////////////////////////////////////////////////////////
CImpBase::CImpBase(void)
{
}

CImpBase::~CImpBase(void)
{
}
SPRESULT CImpBase::GetMesDriverFromShareMemory(void)
{
	SPRESULT res = GetShareMemory(ShareMemory_MesDriver, (void* )&m_pMesDrv, sizeof(m_pMesDrv));
	if (SP_OK == res)
	{
		if (NULL == m_pMesDrv)
		{
			res = SP_E_POINTER;
		}
	}
	return res;
}

SPRESULT CImpBase::SetMesDriverIntoShareMemory(void)
{
	return SetShareMemory(ShareMemory_MesDriver, (const void* )&m_pMesDrv, sizeof(m_pMesDrv),IContainer::System);
}

SPRESULT CImpBase::MES_GetSN(char*pszSN,int iSize)
{
	SPRESULT sp_result = SP_OK;
	ZeroMemory(m_InputSN,sizeof(m_InputSN));
	char szSN[64] = {0};
	//获取SN1（手机中读取的SN1）
	sp_result = GetShareMemory(ShareMemory_SN1, (void* )szSN, sizeof(szSN));
	if (SP_OK != sp_result || 0 == strlen(szSN))
	{
		//获取SN1（InputCodes扫描获取）
		sp_result = GetShareMemory(ShareMemory_My_UserInputSN, (void* )&m_InputSN, sizeof(m_InputSN));
		if (SP_OK != sp_result)
		{
			NOTIFY("GetShareMemory", LEVEL_ITEM, 1, 0, 1, NULL, -1, NULL, "ShareMemory_My_UserInputSN");
			return sp_result;
		}
		if (0 == strlen(m_InputSN[BC_SN1].szCode))
		{
			LogFmtStrW(SPLOGLV_ERROR, L"GetShareMemory(SN1 Length == 0)");
			NOTIFY("GetShareMemory", LEVEL_UI, 1, 0, 1, NULL, -1, NULL, "SN1 Length == 0");
			return SP_E_FAIL;
		}
		strcpy_s(szSN,sizeof(szSN),m_InputSN[BC_SN1].szCode);
	}
	strcpy_s(pszSN,iSize,szSN);
	return SP_OK;
}
