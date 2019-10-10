#pragma once
#include "SpatBase.h"
#include "../drv/MesDriver.h"
#include "SharedDefine.h"
#include "BarCodeUtility.h"
#include <shlwapi.h>
using namespace std;
#pragma comment(lib,"shlwapi.lib")
//////////////////////////////////////////////////////////////////////////
class CImpBase : public CSpatBase
{
public:
    CImpBase(void);
    virtual ~CImpBase(void);
	SPRESULT GetMesDriverFromShareMemory(void);
	SPRESULT SetMesDriverIntoShareMemory(void);

	SPRESULT MES_GetSN(char*pszSN,int iSize);

	INPUT_CODES_T m_InputSN[BC_MAX_NUM];
protected:
	CMesDriver*m_pMesDrv;
};
