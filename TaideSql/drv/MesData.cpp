#include "StdAfx.h"
#include "MesData.h"
#include "callback_def.h"
#include "ISpLogExport.h"
#include <algorithm>
#include <regex>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

//////////////////////////////////////////////////////////////////////////
CMesData::CMesData()
   :m_bStopUpload(FALSE) // Should be FALSE to enable push data,
   ,m_pFileData(NULL)
   ,m_iCsvLogUploadLevel(0)
{
	ZeroMemory(m_szInitFileName,sizeof(m_szInitFileName));
	ZeroMemory(m_szNewFileName,sizeof(m_szNewFileName));
}

CMesData::~CMesData(void)
{
	ZeroMemory(m_szInitFileName,sizeof(m_szInitFileName));
	ZeroMemory(m_szNewFileName,sizeof(m_szNewFileName));
}

void CMesData::Clear(void)
{
    m_LockQ.clear();
}

void CMesData::PushData(CONST ITEMDATA_T& item)
{
    if (!m_bStopUpload && IS_BIT_SET(item.nLv, LEVEL_REPORT))
    {
        m_LockQ.push_back(item);
    }
}

void CMesData::run(void)
{
	std::deque<ITEMDATA_T> dqe;
	while (m_LockQ.size() > 0)   
	{ 	
		INT nCount = m_LockQ.pop_all(dqe, INFINITE);

		if (NULL != m_pFileData)
		{
			char szLogData[1024]={0};

			sprintf_s(szLogData,sizeof(szLogData),"ItemCount,%d\n",nCount);
			fputs(szLogData,m_pFileData);

			sprintf_s(szLogData,sizeof(szLogData),"Code,Item,SubItem,Condition,TestInfo,Level,Result,TestValue,LSL,USL,Unit,ElapsedTime\n");
			fputs(szLogData,m_pFileData);

			for (INT nIndex=0;nIndex<nCount;nIndex++)
			{
				int iLogLevel = (((dqe[nIndex].nLv)<<(LEVEL_MAX_BITS-LEVEL_HOST_OFFSET)) >> (LEVEL_MAX_BITS-LEVEL_HOST_OFFSET));
				if ((m_iCsvLogUploadLevel&iLogLevel)==iLogLevel || 9999 == m_iCsvLogUploadLevel)
				{
					char szSubItem[512]={0};
					if (NULL!=dqe[nIndex].szBand[0])
					{			
						if (dqe[nIndex].nChannel>= 0)
						{

							sprintf_s(szSubItem,sizeof(szSubItem),"%s_%s_CH%d",dqe[nIndex].szItemName,dqe[nIndex].szBand,dqe[nIndex].nChannel);
						}
						else
						{
							sprintf_s(szSubItem,sizeof(szSubItem),"%s_%s",dqe[nIndex].szItemName,dqe[nIndex].szBand);
						}							
					}
					else
					{
						sprintf_s(szSubItem,sizeof(szSubItem),"%s",dqe[nIndex].szItemName);
					}

					char szCondition[512]={0};
					if (NULL != dqe[nIndex].szCond)
					{
						if (dqe[nIndex].szCond[0]=='\'')
						{
							sprintf_s(szCondition,sizeof(szCondition),"%s",dqe[nIndex].szCond+1);
						}
						else
						{
							sprintf_s(szCondition,sizeof(szCondition),"%s",dqe[nIndex].szCond);
						}		
					}
					sprintf_s(szLogData,sizeof(szLogData),"%s,%s,%s,%s,%s,%d,%s,%.2f,%.2f,%.2f,%s,%.2f\n",
						"U0001",
						dqe[nIndex].szActionName,
						szSubItem,
						szCondition,
						"",
						iLogLevel,
						(dqe[nIndex].dValue<dqe[nIndex].dLower || dqe[nIndex].dValue>dqe[nIndex].dUpper)?"FAIL":"PASS",
						dqe[nIndex].dValue,
						dqe[nIndex].dLower,
						dqe[nIndex].dUpper,
						"-",
						0.0);
					fputs(szLogData,m_pFileData);
				}
			}
			fclose(m_pFileData);
			m_pFileData = NULL;
		}
	}   
	m_LockQ.clear();
}

BOOL CMesData::StartUpload(BOOL bStart)
{
    if (bStart)
    {
        m_bStopUpload = FALSE;
        return CThread::start();
    }
    else
    {
        m_bStopUpload = TRUE;
        m_LockQ.wakeup();
        CThread::join(INFINITE);
        return TRUE;
    }
}


BOOL CMesData::MES_Log_Path_Init(int iSlot,char*pszDirectory,char*pszStation)
{
	if(!Creat_Dir(pszDirectory))     
	{       
		return FALSE;              
	}

	SYSTEMTIME sTime;
	GetLocalTime(&sTime);
	char szTime[100]={0};
	sprintf_s(szTime,sizeof(szTime),"%04d%02d%02d%02d%02d%02d%03d",sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond,sTime.wMilliseconds);
	sprintf_s(m_szInitFileName,sizeof(m_szInitFileName),"%s\\PrintDataLog_%s_%d_%s.csv",pszDirectory,pszStation,iSlot,szTime);
	return TRUE;
}

BOOL CMesData::MES_Log_Path_New(int iResult,int iSlot,char*pszSN,char*pszDirectory,char*pszStation)
{
	if (NULL != m_pFileData)
	{
		fclose(m_pFileData);
		m_pFileData = NULL;
	}
	if (NULL == m_szNewFileName[0])
	{
		string strPath(pszDirectory);

		if(!Creat_Dir(pszDirectory))     
		{       
			return FALSE;              
		}

		SYSTEMTIME sTime;
		GetLocalTime(&sTime);

		char szTime[100]={0};
		char szNewName[100]={0};

		sprintf_s(szTime,sizeof(szTime),"%04d%02d%02d%02d%02d%02d",sTime.wYear,sTime.wMonth,sTime.wDay,sTime.wHour,sTime.wMinute,sTime.wSecond);

		sprintf_s(szNewName,sizeof(szNewName),"%s_%s_%d_%s_%s.csv",pszSN,pszStation,iSlot,iResult==1?"PASS":"FAIL",szTime);
		sprintf_s(m_szNewFileName,sizeof(m_szNewFileName),"%s\\%s",strPath.data(),szNewName);
	}
	return TRUE;
}
BOOL CMesData::MES_Log_Get_New_Name(char*pszName,int iSize)
{
	if (NULL==m_szNewFileName[0])
	{
		return FALSE;
	}
	strcpy_s(pszName,iSize,m_szNewFileName);
	return TRUE;
}

BOOL CMesData::MES_Log_Path_Rename(char*pszPath,int iSize)
{
	if (NULL==m_szNewFileName[0] || NULL==m_szInitFileName[0])
	{
		return FALSE;
	}

	if (!PathFileExistsA(m_szInitFileName))
	{
		return FALSE;
	}

	CopyFileA(m_szInitFileName,m_szNewFileName,TRUE);
	DeleteFileA(m_szInitFileName);

	strcpy_s(pszPath,iSize,m_szNewFileName);

	ZeroMemory(m_szInitFileName,sizeof(m_szInitFileName));
	ZeroMemory(m_szNewFileName,sizeof(m_szNewFileName));
	
	return TRUE;
}

BOOL CMesData::MES_Log_Head(_pMES_LOG_HEAD p_data)
{
	if (!File_Open())
	{
		return FALSE;
	}

	m_iCsvLogUploadLevel = p_data->iCsvLogUploadLevel;

	char szLogData[1024]={0};

	sprintf_s(szLogData,"SN,%s\n",NULL==p_data->szSN[0]?"NONE":p_data->szSN);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"Batch,%s\n",NULL==p_data->szBatch[0]?"NONE":p_data->szBatch);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"Station,%s\n",NULL==p_data->szStation[0]?"NONE":p_data->szStation);
	fputs(szLogData,m_pFileData);
	
	sprintf_s(szLogData,"Slot,%d\n",p_data->iSlot);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"PcName,%s\n",NULL==p_data->szPcName[0]?"NONE":p_data->szPcName);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"PcIpAdd,%s\n",NULL==p_data->szPcIpAdd[0]?"NONE":p_data->szPcIpAdd);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"PcMacAdd,%s\n",NULL==p_data->szPcMacAdd[0]?"NONE":p_data->szPcMacAdd);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"OS,%s\n",NULL==p_data->szPcOS[0]?"NONE":p_data->szPcOS);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"ToolName,%s\n",NULL==p_data->szToolName[0]?"NONE":p_data->szToolName);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"ToolVersion,%s\n",NULL==p_data->szToolVersion[0]?"NONE":p_data->szToolVersion);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"SeqName,%s\n",NULL==p_data->szSeqName[0]?"NONE":p_data->szSeqName);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"SeqVersion,%s\n",NULL==p_data->szSeqVersion[0]?"NONE":p_data->szSeqVersion);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"RfDeviceId,%s\n",NULL==p_data->szRfDeviceID[0]?"NONE":p_data->szRfDeviceID);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"Pwr1DeviceId,%s\n",NULL==p_data->szPwr1ID[0]?"NONE":p_data->szPwr1ID);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"Pwr2DeviceId,%s\n",NULL==p_data->szPwr2ID[0]?"NONE":p_data->szPwr2ID);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"FixtureId,%s\n",NULL==p_data->szFixtureID[0]?"NONE":p_data->szFixtureID);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"Loss,%s\n",NULL==p_data->szLoss[0]?"NONE":p_data->szLoss);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"ElapsedTime,%.2f\n",p_data->dElapsedTime<0.0?0.0:p_data->dElapsedTime);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"TesrResult,%s\n",p_data->iTestResult==1?"PASS":"FAIL");
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"ErrorCode,%s\n",p_data->szErrorCode);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"ErrorType,%s\n",p_data->szErrorType);
	fputs(szLogData,m_pFileData);

	sprintf_s(szLogData,"ErrorMsg,%s\n",p_data->szErrorMsg);
	fputs(szLogData,m_pFileData);

	return TRUE;
}
BOOL CMesData::Creat_Dir(char*pszDir)
{
	string strDir(pszDir),strPath="";
	const string strFname("\\");
	if(PathIsDirectoryA(strPath.data()))     
	{  
		return TRUE;
	}

#pragma warning(disable:4127)
	while(TRUE)
#pragma warning(default:4127)
	{
		string::size_type posS = strDir.find(strFname);
		if (std::string::npos != posS)
		{
			strPath += strDir.substr(0, posS);
			strPath += strFname;
			strDir = strDir.substr(posS + strFname.length(),strDir.length()-(posS + strFname.length()));  
			if(!PathIsDirectoryA(strPath.data()))     
			{   
				if (!CreateDirectoryA(strPath.data(),NULL))
				{
					return FALSE;
				}		
			} 
		}
		else
		{
			strPath +=  strDir;
			if(!PathIsDirectoryA(strPath.data()))     
			{   
				if (!CreateDirectoryA(strPath.data(),NULL))
				{
					return FALSE;
				}		
			} 
			break;
		}
	}
	return TRUE;
}

BOOL CMesData::File_Open()
{
	int iTry=0;
	for (iTry=0;iTry<5;iTry++)
	{
		m_pFileData = NULL;
		fopen_s(&m_pFileData,m_szInitFileName,"at+");
		if(NULL != m_pFileData)
		{
			break;
		}
	}
	if (5==iTry)
	{
		return FALSE;
	}
	fseek(m_pFileData,0L,2);
	return TRUE;
}