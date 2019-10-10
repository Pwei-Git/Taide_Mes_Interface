#include "StdAfx.h"
#include "MesDriver.h"
#include <assert.h>



#ifdef _DEBUG
//#define _DUMMY_TINNO_MES_     
#endif

#ifdef _DUMMY_MES_
#pragma warning(disable:4100)
#endif
//////////////////////////////////////////////////////////////////////////
CMesDriver::CMesDriver(void)
    : m_hDLL(NULL)
{
}

CMesDriver::~CMesDriver(void)
{
}

void CMesDriver::Cleanup(void)
{
    if (NULL != m_hDLL)
    {
        FreeLibrary(m_hDLL);
        m_hDLL = NULL;
        m_drv.Reset();
    }
}

BOOL CMesDriver::Startup(std::wstring& strDLL)
{
	BOOL bRet = FALSE ; 
	
	//*////
	PRINTF_FMT_MSG_MUST("BOOL CMesDriver::Startup() str:%s", strDLL.c_str()) ;
	//*////

	
	m_hDLL = LoadLibraryExW(strDLL.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (NULL == m_hDLL)
	{
		return (bRet) ;
	}


	m_drv.CheckRoute = (pCheckRoute)::GetProcAddress(m_hDLL, "CheckRoute");
	m_drv.Save2NextStation = (pSave2NextStation)::GetProcAddress(m_hDLL, "Save2NextStation");
	m_drv.BindData = (pBindData)::GetProcAddress(m_hDLL, "BindData");
	m_drv.QueryData = (pQueryData)::GetProcAddress(m_hDLL, "QueryData");


	//*////
	PRINTF_FMT_MSG_MUST("CheckRoute:0x%08x" , m_drv.CheckRoute) ;
	PRINTF_FMT_MSG_MUST("Save2NextStation:0x%08x" , m_drv.Save2NextStation) ;
	PRINTF_FMT_MSG_MUST(" BindData:0x%08x" , m_drv.BindData) ;
	PRINTF_FMT_MSG_MUST("QueryData:0x%08x" , m_drv.QueryData) ;
	//*////

	if (!m_drv.IsValid())
	{
		Cleanup();
		return (bRet) ;
	}

	bRet = TRUE ;
	return (bRet) ;
}



BOOL CMesDriver::StartupA(std::string& strDLL)
{
	BOOL bRet = FALSE ; 
	
	//*////
	PRINTF_FMT_MSG_MUST("BOOL CMesDriver::StartupA() str:%s", strDLL.c_str()) ;
	//*////

	
   	 m_hDLL = LoadLibraryA(strDLL.c_str());  
	if (NULL == m_hDLL)
	{
		return (bRet) ;
	}


	m_drv.CheckRoute = (pCheckRoute)::GetProcAddress(m_hDLL, "CheckRoute");
	m_drv.Save2NextStation = (pSave2NextStation)::GetProcAddress(m_hDLL, "Save2NextStation");
	m_drv.BindData = (pBindData)::GetProcAddress(m_hDLL, "BindData");
	m_drv.QueryData = (pQueryData)::GetProcAddress(m_hDLL, "QueryData");

	//*////
	PRINTF_FMT_MSG_MUST("CheckRoute:0x%08x" , m_drv.CheckRoute) ;
	PRINTF_FMT_MSG_MUST("Save2NextStation:0x%08x" , m_drv.Save2NextStation) ;
	PRINTF_FMT_MSG_MUST(" BindData:0x%08x" , m_drv.BindData) ;
	PRINTF_FMT_MSG_MUST("QueryData:0x%08x" , m_drv.QueryData) ;
	//*////

	if (!m_drv.IsValid())
	{
		Cleanup();
		return (bRet) ;
	}

	bRet = TRUE ;
	return (bRet) ;
}

//bool mytrace(TCHAR *format, ...)
//
//{
//
//	TCHAR buffer[1000];
//
//	va_list argptr;
//
//	va_start(argptr, format);
//
//	wvsprintf(buffer, format, argptr);
//
//	va_end(argptr);
//
//	OutputDebugString(buffer);
//
//	return true;
//
//}
//
int CMesDriver::CheckRoute(char* Sn, char* Station, char* Msg)
{
	int iRet = EECT_ERROR ; 
	//mytrace(L"########CheckRoute");

	//mytrace(L"##int CMesDriver::CheckRoute() m_drv.CheckRoute:0x%08x " , m_drv.CheckRoute) ;
	//mytrace(L"##int CMesDriver::CheckRoute() Sn:%s " , Sn) ;
	//mytrace(L"##int CMesDriver::CheckRoute() Station:%s " , Station) ;
	//mytrace(L"##int CMesDriver::CheckRoute() Msg:%s " , Msg) ;

 	PRINTF_FMT_MSG_MUST("int CMesDriver::CheckRoute() m_drv.CheckRoute:0x%08x " , m_drv.CheckRoute) ;
 	PRINTF_FMT_MSG_MUST("int CMesDriver::CheckRoute() Sn:%s " , Sn) ;
 	PRINTF_FMT_MSG_MUST("int CMesDriver::CheckRoute() Station:%s " , Station) ;
 	PRINTF_FMT_MSG_MUST("int CMesDriver::CheckRoute() Msg:%s " , Msg) ;


//	mytrace(L"int CMesDriver::CheckRoute() Msg:%s " , Msg) ;
	if (NULL != m_drv.CheckRoute)
	{
			//mytrace(L"int CMesDriver::CheckRoute() Msg:%s " , Msg) ;
#ifdef _DUMMY_MES_
		mytrace(L"CMesDriver::CheckRoute() Msg:%s " , Msg) ;
		iRet = EECT_OK ;
#else
		
		iRet = m_drv.CheckRoute(Sn,Station, Msg);
		//mytrace(L"int CMesDriver::CheckRoute() Msg:%s,iRet = %d " , Msg,iRet) ;
#endif
	}
	else
//	mytrace(L"int CMesDriver::CheckRoute() Msg:%s " , Msg) ;

	return (iRet) ;
}
int CMesDriver::Save2NextStation(char* Sn, char* Station, char* Msg)
{
	int iRet = EECT_ERROR ; 

	PRINTF_FMT_MSG_MUST("int CMesDriver::Save2NextStation() m_drv.Save2NextStation:0x%08x " , m_drv.Save2NextStation) ;

	if (NULL != m_drv.Save2NextStation)
	{
#ifdef _DUMMY_MES_
		iRet = EECT_OK ;
#else
		iRet = m_drv.Save2NextStation(Sn, Station, Msg);
#endif
	}
	
	return (iRet) ;
}


int CMesDriver::BindData(char* Sn, char* FieldName, char* FieldValue, char* Msg)
{
	int iRet = EECT_ERROR ; 

	PRINTF_FMT_MSG_MUST("int CMesDriver::BindData() m_drv.BindData:0x%08x " , m_drv.BindData) ;
	
	if (NULL != m_drv.BindData)
	{
#ifdef _DUMMY_MES_
		iRet = EECT_OK ;
#else
		iRet = m_drv.BindData(Sn, FieldName, FieldValue, Msg);
#endif
	}
	
	return (iRet) ;
}


int CMesDriver::QueryData(char* Sn, char* FieldName, char* FieldValue, char* Msg)
{
	int iRet = EECT_ERROR ; 

	PRINTF_FMT_MSG_MUST("int CMesDriver::QueryData() m_drv.QueryData:0x%08x " , m_drv.QueryData) ;
	
	if (NULL != m_drv.QueryData)
	{
#ifdef _DUMMY_MES_
		iRet = EECT_OK ;
#else
		iRet = m_drv.QueryData(Sn, FieldName, FieldValue, Msg);
#endif
	}

	return (iRet) ;
}




