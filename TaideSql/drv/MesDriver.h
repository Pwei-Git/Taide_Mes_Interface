#pragma once
#include <Windows.h>
#include <string>
#include "drvdef.h"

// Lisiqiang 20190328 +++ 
#include "Includes.h"
// Lisiqiang 20190328 ---


#define ShareMemory_MesDriver    L"MesDriverObject"



//////////////////////////////////////////////////////////////////////////
class CMesDriver sealed
{
public:
    CMesDriver(void);
   ~CMesDriver(void);

    // Load or free CustomerSolutionMes.dll
    BOOL Startup(std::wstring& strDLL);
    BOOL StartupA(std::string& strDLL);
    void Cleanup(void);

    // Invoke interfaces CustomerSolutionMes.dll
	// Lisiqiang 20190328 +++ 

	int TDAPI CheckRoute(char* Sn, char* Station, char* Msg) ;
	int TDAPI Save2NextStation(char* Sn, char* Station, char* Msg) ;
	int TDAPI BindData(char* Sn, char* FieldName, char* FieldValue, char* Msg) ;
	int TDAPI QueryData(char* Sn, char* FieldName, char* FieldValue, char* Msg) ;

	// Lisiqiang 20190328 ---


	MES_DRIVER_T&GetDrvObject(void) { return m_drv; };
private:
    MES_DRIVER_T m_drv;
    HMODULE m_hDLL;
    CHAR m_szErrMsg[512];
		
};


