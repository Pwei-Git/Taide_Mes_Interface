#pragma once
#include "IItemDataObserver.h"
#include "../../../../../../Common/thread/Thread.h"
#include "LockQueue.h"
typedef CLockQueue<IItemDataObserver::ITEMDATA_T> CDataLockQueue;

typedef struct _STRUCT_MES_LOG_HEAD
{
	int iSlot;
	int iTestResult;
	int iCsvLogUploadLevel;
	double dElapsedTime;
	char szSN[64];
	char szBatch[64];
	char szStation[64];
	char szPcName[64];
	char szPcIpAdd[64];
	char szPcMacAdd[64];
	char szPcOS[64];
	char szToolName[64];
	char szToolVersion[64];
	char szSeqName[64];
	char szSeqVersion[64];
	char szRfDeviceID[256];
	char szPwr1ID[256];
	char szPwr2ID[256];
	char szFixtureID[128];
	char szLoss[128];
	char szErrorCode[32];
	char szErrorType[32];
	char szErrorMsg[256];
	_STRUCT_MES_LOG_HEAD()
	{
		ZeroMemory(this, sizeof(*this));
	}
}_MES_LOG_HEAD,*_pMES_LOG_HEAD;

//////////////////////////////////////////////////////////////////////////
class CMesData : public IItemDataObserver
              , public CThread
{
public:
    CMesData();
    virtual ~CMesData(void);

    BOOL StartUpload(BOOL bStart);

    // Interface of <IItemDataObserver>
    virtual void Release(void) { delete this; };
    virtual void Clear(void);
    virtual void PushData(CONST ITEMDATA_T& item);

    // Interface of <CThread>
    virtual void run(void);

public:
	BOOL MES_Log_Path_Init(int iSlot,char*pszDirectory,char*pszStation);
	BOOL MES_Log_Path_New(int iResult,int iSlot,char*pszSN,char*pszDirectory,char*pszStation);
	BOOL MES_Log_Path_Rename(char*pszPath,int iSize);

	BOOL MES_Log_Head(_pMES_LOG_HEAD p_data);
	BOOL MES_Log_Get_New_Name(char*pszName,int iSize);

	BOOL  Creat_Dir(char*pszDir);

	BOOL	File_Open();

	int m_iCsvLogUploadLevel;
	char m_szInitFileName[512];
	char m_szNewFileName[512];

	FILE*m_pFileData;

    CDataLockQueue m_LockQ;
    volatile BOOL  m_bStopUpload;  // Stop to upload
};
