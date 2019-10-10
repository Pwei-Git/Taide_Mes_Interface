#include "stdafx.h"
#include "main.h"
#include <sstream>
#include <string>
#include <direct.h>
#include <afx.h>
#include <string>
#include <atlstr.h>

using namespace std;

string ConnectString();
string GetLogName(char* Sn);
string GetLogFolder();
/*****��¼��־*****/
int write_log(FILE* pFile, const char *format, ...); 
_variant_t SqlCommand_Check_Route(char * pcbSeq, char * prodNo, char * stationNo, char * retest);
_variant_t SqlCommand_BindData(char * prodNo, char * pcbSeq, char * fieldName, char * fieldValue);
_variant_t SqlCommand_QueryData(char * prodNo, char * pcbSeq, char * fieldName);
_variant_t SqlCommand_Save2NextStation(char * pcbSeq, char * prodNo, char * stationNo);


MESAPI int CheckRoute(char* Sn, char* Station, char* Msg)
{
	string logName;
	logName = GetLogName(Sn);
	//TRACE("#####TestTime = %s", logName.c_str());
	char LOGName[256] = { 0 };
	strcpy(LOGName, logName.c_str());
	char mkdir[128] = "md ";
	strcat(mkdir, GetLogFolder().c_str());
	//system("md f:\\mydir");
	system(mkdir);

	char logname[128] = { 0 };
	char mkdir1[2] = "\\";
	strcpy(logname, GetLogFolder().c_str());
	strcat(logname, mkdir1);
	strcat(logname, LOGName);

	FILE *pFile = fopen(logname, "a+");
	write_log(pFile, "------------------------------------------------------------------------------------------------------------------\n");
 	//FILE *pFile = fopen(LOGName, "a+");
	write_log(pFile, "��ǰ������ˮ�ţ� %s\n", Sn);

	_variant_t msgId, msgStr;
	int Result = 1;

	/*******��ʼ������*******/
	CoInitialize(NULL);  //��ʼ��com
	_ConnectionPtr m_pConnect = NULL;
	HRESULT hr = m_pConnect.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))
		return 1;
	_RecordsetPtr pRecordset;
	/*******��ʼ���������*******/

	/*******��ʼ����SQLServer���ݿ�*******/
	//��ȡ�����ַ���
	//_bstr_t strConnect = "Provider=SQLOLEDB;Data Source=192.168.5.14;User ID=tdmes;Password=Tdmes755473ABC;Initial Catalog=SmartMES";
	string str = ConnectString();
	_bstr_t strConnect = (str.c_str());
	write_log(pFile, "SQLServer���ݿ������ִ� = %s\n", str.c_str());

	char path[256] = { 0 }, prodNo[256] = { 0 }, reTest[2] = { 0 };
	_getcwd(path, 256);
	char mes[16] = "\\MESConnect.ini";
	strcat_s(path, mes);

	GetPrivateProfileStringA("SqlConnect", "prodNo", "", prodNo, sizeof(prodNo), path);     //��ini�ļ���ȡ�����
	GetPrivateProfileStringA("SqlConnect", "reTest", "", reTest, sizeof(reTest), path);     //reTest==�Ƿ������ز� 1�����ظ����ԣ�0������

	/*******SQL���ݿ�ָ��*******/
	//_variant_t strSQL = "execute checkRoute 'PestpcbSeq','PestprodNo','PeststationNo',0";
	_variant_t strSQL = SqlCommand_Check_Route(Sn, prodNo, Station, reTest);

	/*******�洢���̵���*******/
	/*******������*******/
	try
	{
		hr = m_pConnect->Open(strConnect, "", "", adModeUnknown);  //SQLServer�������ݿ�
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "���ݿ����Ӵ��� = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 2;
	}
	if (FAILED(hr))
		return 1;

	/*****����_RecordsetPtr����������Recordset�����Open���������ɴ�һ�����ݼ�*****/
	if (FAILED(pRecordset.CreateInstance(__uuidof(Recordset))))
		return 1;

	/*****ִ�����ݿ�洢���̵��ã����ѽ��������һ�����ݼ��϶���*****/
	try
	{
		hr = pRecordset->Open(strSQL, _variant_t((IDispatch*)m_pConnect), adOpenStatic, adLockOptimistic, adCmdText);
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "SQL������� = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 3;
	}
	if (FAILED(hr))
		return 1;

	write_log(pFile, "�洢����SQL��� = %s\n", (LPCSTR)_bstr_t(strSQL));

	/****ͨ����������ȡ���****/
	while (!pRecordset->adoEOF)  //��ȡ������������
	{
		msgId = pRecordset->GetCollect("msgId");      //CheckRoute�ӿڲ��Խ������ֵ 0ΪPASS��1ΪFAIL
		msgStr = pRecordset->GetCollect("msgStr");    //��������  ��msgIdΪ0����msgStrΪ��
		pRecordset->MoveNext();                       //�ƶ�����һ������
	}

	/****�������ý������_variant_t����ת���ɲ��Թ��߶�Ӧ����****/
	if (msgId.vt != VT_NULL)
	{
		if (msgId.lVal == 1)
		{
			Result = msgId.lVal;
			strcpy(Msg, (const char *)_bstr_t(msgStr));
			write_log(pFile, "����Result = %d\n", Result);
		}
		else if(msgId.lVal == 0)
		{
			write_log(pFile, "����ErrorMessage = %s\n", Msg);
			Result = msgId.lVal;
			Msg = NULL;
		}
	}

	write_log(pFile, "����Result = %d\n", Result);
	write_log(pFile, "����ErrorMessage = %s\n", Msg);
	
	write_log(pFile, "��ǰ������ˮ�Žӿڵ�����ɣ� %s\n", Sn);
	write_log(pFile, "------------------------------------------------------------------------------------------------------------------\n");

	fclose(pFile);

	pRecordset->Close();
	pRecordset = NULL;
	m_pConnect->Close();
	m_pConnect = NULL;

	return Result;
}


MESAPI int BindData(char* Sn, char* FieldName, char* FieldValue, char* Msg)
{
	string logName;
	logName = GetLogName(Sn);
	//TRACE("#####TestTime = %s", logName.c_str());
	char LOGName[256] = { 0 };
	strcpy(LOGName, logName.c_str());
	char mkdir[128] = "md ";
	strcat(mkdir, GetLogFolder().c_str());
	//system("md f:\\mydir");
	system(mkdir);

	char logname[128] = { 0 };
	char mkdir1[2] = "\\";
	strcpy(logname, GetLogFolder().c_str());
	strcat(logname, mkdir1);
	strcat(logname, LOGName);

	FILE *pFile = fopen(logname, "a+");
	write_log(pFile, "------------------------------------------------------------------------------------------------------------------\n");
	//FILE *pFile = fopen(LOGName, "a+");
	write_log(pFile, "��ǰ������ˮ�ţ� %s\n", Sn);

	//�������
	_variant_t msgId, msgStr;
	int Result = 1;
	char * ErrorInfo = new char[256];

	/*******��ʼ������*******/
	CoInitialize(NULL);  //��ʼ��com
	_ConnectionPtr m_pConnect = NULL;
	HRESULT hr = m_pConnect.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))
		return 1;
	_RecordsetPtr pRecordset;
	/*******��ʼ���������*******/

	/*******��ʼ����SQLServer���ݿ�*******/
	/*******��ȡ�����ַ���*******/
	//SQL���=_bstr_t strConnect = "Provider=SQLOLEDB;Data Source=192.168.5.14;User ID=tdmes;Password=Tdmes755473ABC;Initial Catalog=SmartMES";
	string str = ConnectString();
	_bstr_t strConnect = (str.c_str());

	write_log(pFile, "SQLServer���ݿ������ִ� = %s\n", str.c_str());

	char path[256] = { 0 }, prodNo[256] = { 0 }, reTest[2] = { 0 };
	_getcwd(path, 256);
	char mes[16] = "\\MESConnect.ini";
	strcat_s(path, mes);

	GetPrivateProfileStringA("SqlConnect", "prodNo", "", prodNo, sizeof(prodNo), path);     //��ini�ļ���ȡ�����


	/*******SQL���ݿ�ָ��*******/
	//_variant_t strSQL = "execute checkRoute 'PestpcbSeq','PestprodNo','PeststationNo',0";
	_variant_t strSQL = SqlCommand_BindData(prodNo, Sn, FieldName, FieldValue);

	write_log(pFile, "�洢����SQL��� = %s\n", (LPCSTR)_bstr_t(strSQL));

	/*******�洢���̵���*******/
	/*******������*******/
	try
	{
		hr = m_pConnect->Open(strConnect, "", "", adModeUnknown);  //SQLServer�������ݿ�
	}
	catch (_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "���ݿ����Ӵ��� = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 2;
	}
	if (FAILED(hr))
		return 1;

	/*****����_RecordsetPtr����������Recordset�����Open���������ɴ�һ�����ݼ�*****/
	if (FAILED(pRecordset.CreateInstance(__uuidof(Recordset))))
		return 1;

	/*****ִ�����ݿ�洢���̵��ã����ѽ��������һ�����ݼ��϶���*****/
	try
	{
		hr = pRecordset->Open(strSQL, _variant_t((IDispatch*)m_pConnect), adOpenStatic, adLockOptimistic, adCmdText);
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "���ݿ����Ӵ��� = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 3;
	}
	if (FAILED(hr))
		return 1;

	write_log(pFile, "�洢����SQL��� = %s\n", (LPCSTR)_bstr_t(strSQL));

	/****ͨ����������ȡ���****/
	while (!pRecordset->adoEOF)  //��ȡ������������
	{
		msgId = pRecordset->GetCollect("msgId");      //CheckRoute�ӿڲ��Խ������ֵ 0ΪPASS  1ΪFAIL
		msgStr = pRecordset->GetCollect("msgStr");    //��������  ��msgIdΪ0����msgStrΪ��
		pRecordset->MoveNext();  //�ƶ�����һ������
	}

	//�������ý������_variant_t����ת���ɲ��Թ��߶�Ӧ����
	if (msgId.vt != VT_NULL)
	{
		Result = msgId.lVal;
		write_log(pFile, "����Result = %d\n", Result);
	}

	strcpy(ErrorInfo, (const char *)_bstr_t(msgStr));

	write_log(pFile, "����Result = %d\n", Result);
	write_log(pFile, "����ErrorMessage = %s\n", ErrorInfo);

	write_log(pFile, "��ǰ������ˮ�Žӿڵ�����ɣ� %s\n", Sn);
	write_log(pFile, "------------------------------------------------------------------------------------------------------------------\n");

	fclose(pFile);

	pRecordset->Close();
	pRecordset = NULL;
	m_pConnect->Close();
	m_pConnect = NULL;

	return Result;
}

MESAPI int QueryData(char* Sn, char* FieldName, char* FieldValue, char* Msg)
{
	string logName;
	logName = GetLogName(Sn);
	//TRACE("#####TestTime = %s", logName.c_str());
	char LOGName[256] = { 0 };
	strcpy(LOGName, logName.c_str());
	char mkdir[128] = "md ";
	strcat(mkdir, GetLogFolder().c_str());
	//system("md f:\\mydir");
	system(mkdir);

	char logname[128] = { 0 };
	char mkdir1[2] = "\\";
	strcpy(logname, GetLogFolder().c_str());
	strcat(logname, mkdir1);
	strcat(logname, LOGName);

	FILE *pFile = fopen(logname, "a+");
	write_log(pFile, "------------------------------------------------------------------------------------------------------------------\n");
	//FILE *pFile = fopen(LOGName, "a+");
	write_log(pFile, "��ǰ������ˮ�ţ� %s\n", Sn);
	
	//�������
	_variant_t ResultValue;
	int Result = 1;
	char * ErrorInfo = new char[256];

	/*******��ʼ������*******/
	CoInitialize(NULL);  //��ʼ��com
	_ConnectionPtr m_pConnect = NULL;
	HRESULT hr = m_pConnect.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))
		return 1;
	_RecordsetPtr pRecordset;
	/*******��ʼ���������*******/

	/*******��ʼ����SQLServer���ݿ�*******/
	//��ȡ�����ַ���
	//_bstr_t strConnect = "Provider=SQLOLEDB;Data Source=192.168.5.14;User ID=tdmes;Password=Tdmes755473ABC;Initial Catalog=SmartMES";
	string str = ConnectString();
	_bstr_t strConnect = (str.c_str());

	write_log(pFile, "SQLServer���ݿ������ִ� = %s\n", str.c_str());

	/*****��ini�ļ���ȡ������*****/
	char path[256] = { 0 }, prodNo[256] = { 0 };
	_getcwd(path, 256);
	char mes[16] = "\\MESConnect.ini";
	strcat_s(path, mes);
	GetPrivateProfileStringA("SqlConnect", "prodNo", "", prodNo, sizeof(prodNo), path);

	_variant_t strSQL = SqlCommand_QueryData(prodNo, Sn, FieldName);

	write_log(pFile, "�洢����SQL��� = %s\n", (LPCSTR)_bstr_t(strSQL));

	/*******�洢���̵���*******/
	/*******������*******/
	try
	{
		hr = m_pConnect->Open(strConnect, "", "", adModeUnknown);  //SQLServer�������ݿ�
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "���ݿ����Ӵ��� = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 2;
	}
	if (FAILED(hr))
		return 1;

	/*****����_RecordsetPtr����������Recordset�����Open���������ɴ�һ�����ݼ�*****/
	if (FAILED(pRecordset.CreateInstance(__uuidof(Recordset))))
		return 1;

	/*****ִ�����ݿ�洢���̵��ã����ѽ��������һ�����ݼ��϶���*****/
	try
	{
		hr = pRecordset->Open(strSQL, _variant_t((IDispatch*)m_pConnect), adOpenStatic, adLockOptimistic, adCmdText);
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "SQL������� = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 3;
	}
	if (FAILED(hr))
		return 1;

	write_log(pFile, "�洢����SQL��� = %s\n", (LPCSTR)_bstr_t(strSQL));

	/****ͨ����������ȡ���****/
	while (!pRecordset->adoEOF)  //��ȡ������������
	{
		ResultValue = pRecordset->GetCollect(FieldName);
		pRecordset->MoveNext();  //�ƶ�����һ������
	}

	if (ResultValue.vt != VT_NULL)
	{
		string str = (const char *)_bstr_t(ResultValue);

		strcpy(FieldValue, str.c_str());

		write_log(pFile, "��ȡ����Ϊ = %s\n", FieldValue);

		Result = 0;
	}
	else
	{
		write_log(pFile, "��ȡ����Ϊ��\n");

		Result = 1;
	}

	write_log(pFile, "��ǰ������ˮ�Žӿڵ�����ɣ� %s\n", Sn);
	write_log(pFile, "------------------------------------------------------------------------------------------------------------------\n");

	fclose(pFile);

	pRecordset->Close();
	pRecordset = NULL;
	m_pConnect->Close();
	m_pConnect = NULL;

	return Result;
}

MESAPI int Save2NextStation(char* Sn, char* Station, char* Msg)
{
	string logName;
	logName = GetLogName(Sn);
	//TRACE("#####TestTime = %s", logName.c_str());
	char LOGName[256] = { 0 };
	strcpy(LOGName, logName.c_str());
	char mkdir[128] = "md ";
	strcat(mkdir, GetLogFolder().c_str());
	//system("md f:\\mydir");
	system(mkdir);

	char logname[128] = { 0 };
	char mkdir1[2] = "\\";
	strcpy(logname, GetLogFolder().c_str());
	strcat(logname, mkdir1);
	strcat(logname, LOGName);

	FILE *pFile = fopen(logname, "a+");
	write_log(pFile, "------------------------------------------------------------------------------------------------------------------\n");
	//FILE *pFile = fopen(LOGName, "a+");
	write_log(pFile, "��ǰ������ˮ�ţ� %s\n", Sn);

	//�������
	_variant_t msgId, msgStr;
	int Result = 1;
	char * ErrorInfo = new char[256];

	/*******��ʼ������*******/
	CoInitialize(NULL);  //��ʼ��com
	_ConnectionPtr m_pConnect = NULL;
	HRESULT hr = m_pConnect.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))
		return 1;
	_RecordsetPtr pRecordset;
	/*******��ʼ���������*******/

	/*******��ʼ����SQLServer���ݿ�*******/
	//��ȡ�����ַ���
	//_bstr_t strConnect = "Provider=SQLOLEDB;Data Source=192.168.5.14;User ID=tdmes;Password=Tdmes755473ABC;Initial Catalog=SmartMES";
	string str = ConnectString();
	_bstr_t strConnect = (str.c_str());

	write_log(pFile, "SQLServer���ݿ������ִ� = %s\n", str.c_str());

	/*****��ȡ������*****/
	char path[256] = { 0 }, prodNo[256] = { 0 };
	_getcwd(path, 256);
	char mes[16] = "\\MESConnect.ini";
	strcat_s(path, mes);
	GetPrivateProfileStringA("SqlConnect", "prodNo", "", prodNo, sizeof(prodNo), path);

	_variant_t strSQL = SqlCommand_Save2NextStation(Sn, prodNo, Station);

	write_log(pFile, "�洢����SQL��� = %s\n", (LPCSTR)_bstr_t(strSQL));

	/*******�洢���̵���*******/
	/*******������*******/
	try
	{
		hr = m_pConnect->Open(strConnect, "", "", adModeUnknown);  //SQLServer�������ݿ�
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "���ݿ����Ӵ��� = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 2;
	}
	if (FAILED(hr))
		return 1;
	write_log(pFile, "�洢����SQL��� = %s\n", (char*)strConnect);

	/*****����_RecordsetPtr����������Recordset�����Open���������ɴ�һ�����ݼ�*****/
	if (FAILED(pRecordset.CreateInstance(__uuidof(Recordset))))
		return 1;

	/*****ִ�����ݿ�洢���̵��ã����ѽ��������һ�����ݼ��϶���*****/
	try
	{
		hr = pRecordset->Open(strSQL, _variant_t((IDispatch*)m_pConnect), adOpenStatic, adLockOptimistic, adCmdText);
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "SQL������� = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 3;
	}
	if (FAILED(hr))
		return 1;

	write_log(pFile, "�洢����SQL��� = %s\n", (LPCSTR)_bstr_t(strSQL));

	/****ͨ����������ȡ���****/
	while (!pRecordset->adoEOF)  //��ȡ������������
	{
		msgId = pRecordset->GetCollect("msgId");
		pRecordset->MoveNext();  //�ƶ�����һ������
	}
	
	if (msgId.vt != VT_NULL)
	{
		if (msgId.lVal == 1)
		{
			Result = msgId.lVal;
			strcpy(Msg, (const char *)_bstr_t(msgStr));
			write_log(pFile, "����Result = %d\n", Result);
		}
		else if(msgId.lVal == 0)
		{
			Result = msgId.lVal;
			Msg = NULL;
			write_log(pFile, "����Result = %d\n", Result);
			write_log(pFile, "����ErrorMessage = %s\n", Msg);
		}
	}

	write_log(pFile, "�������� = %s\n", Msg);

	write_log(pFile, "��ǰ������ˮ�Žӿڵ�����ɣ� %s\n", Sn);
	write_log(pFile, "------------------------------------------------------------------------------------------------------------------\n");

	fclose(pFile);

	pRecordset->Close();
	pRecordset = NULL;
	m_pConnect->Close();
	m_pConnect = NULL;

	return Result;
}

