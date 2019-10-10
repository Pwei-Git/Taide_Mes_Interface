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
/*****记录日志*****/
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
	write_log(pFile, "当前测试流水号： %s\n", Sn);

	_variant_t msgId, msgStr;
	int Result = 1;

	/*******初始化环境*******/
	CoInitialize(NULL);  //初始化com
	_ConnectionPtr m_pConnect = NULL;
	HRESULT hr = m_pConnect.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))
		return 1;
	_RecordsetPtr pRecordset;
	/*******初始化环境完成*******/

	/*******开始连接SQLServer数据库*******/
	//获取连接字符串
	//_bstr_t strConnect = "Provider=SQLOLEDB;Data Source=192.168.5.14;User ID=tdmes;Password=Tdmes755473ABC;Initial Catalog=SmartMES";
	string str = ConnectString();
	_bstr_t strConnect = (str.c_str());
	write_log(pFile, "SQLServer数据库连接字串 = %s\n", str.c_str());

	char path[256] = { 0 }, prodNo[256] = { 0 }, reTest[2] = { 0 };
	_getcwd(path, 256);
	char mes[16] = "\\MESConnect.ini";
	strcat_s(path, mes);

	GetPrivateProfileStringA("SqlConnect", "prodNo", "", prodNo, sizeof(prodNo), path);     //从ini文件获取制令单号
	GetPrivateProfileStringA("SqlConnect", "reTest", "", reTest, sizeof(reTest), path);     //reTest==是否允许重测 1允许重复测试，0不允许

	/*******SQL数据库指令*******/
	//_variant_t strSQL = "execute checkRoute 'PestpcbSeq','PestprodNo','PeststationNo',0";
	_variant_t strSQL = SqlCommand_Check_Route(Sn, prodNo, Station, reTest);

	/*******存储过程调用*******/
	/*******打开连接*******/
	try
	{
		hr = m_pConnect->Open(strConnect, "", "", adModeUnknown);  //SQLServer连接数据库
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "数据库连接错误 = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 2;
	}
	if (FAILED(hr))
		return 1;

	/*****声明_RecordsetPtr变量，调用Recordset对象的Open方法，即可打开一个数据集*****/
	if (FAILED(pRecordset.CreateInstance(__uuidof(Recordset))))
		return 1;

	/*****执行数据库存储过程调用，并把结果储存在一个数据集合对象*****/
	try
	{
		hr = pRecordset->Open(strSQL, _variant_t((IDispatch*)m_pConnect), adOpenStatic, adLockOptimistic, adCmdText);
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "SQL命令错误 = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 3;
	}
	if (FAILED(hr))
		return 1;

	write_log(pFile, "存储过程SQL语句 = %s\n", (LPCSTR)_bstr_t(strSQL));

	/****通过参数名获取结果****/
	while (!pRecordset->adoEOF)  //获取表中所有数据
	{
		msgId = pRecordset->GetCollect("msgId");      //CheckRoute接口测试结果返回值 0为PASS，1为FAIL
		msgStr = pRecordset->GetCollect("msgStr");    //错误描述  如msgId为0，则msgStr为空
		pRecordset->MoveNext();                       //移动到下一条数据
	}

	/****解析调用结果：将_variant_t类型转换成测试工具对应类型****/
	if (msgId.vt != VT_NULL)
	{
		if (msgId.lVal == 1)
		{
			Result = msgId.lVal;
			strcpy(Msg, (const char *)_bstr_t(msgStr));
			write_log(pFile, "测试Result = %d\n", Result);
		}
		else if(msgId.lVal == 0)
		{
			write_log(pFile, "测试ErrorMessage = %s\n", Msg);
			Result = msgId.lVal;
			Msg = NULL;
		}
	}

	write_log(pFile, "测试Result = %d\n", Result);
	write_log(pFile, "测试ErrorMessage = %s\n", Msg);
	
	write_log(pFile, "当前测试流水号接口调用完成： %s\n", Sn);
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
	write_log(pFile, "当前测试流水号： %s\n", Sn);

	//定义变量
	_variant_t msgId, msgStr;
	int Result = 1;
	char * ErrorInfo = new char[256];

	/*******初始化环境*******/
	CoInitialize(NULL);  //初始化com
	_ConnectionPtr m_pConnect = NULL;
	HRESULT hr = m_pConnect.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))
		return 1;
	_RecordsetPtr pRecordset;
	/*******初始化环境完成*******/

	/*******开始连接SQLServer数据库*******/
	/*******获取连接字符串*******/
	//SQL语句=_bstr_t strConnect = "Provider=SQLOLEDB;Data Source=192.168.5.14;User ID=tdmes;Password=Tdmes755473ABC;Initial Catalog=SmartMES";
	string str = ConnectString();
	_bstr_t strConnect = (str.c_str());

	write_log(pFile, "SQLServer数据库连接字串 = %s\n", str.c_str());

	char path[256] = { 0 }, prodNo[256] = { 0 }, reTest[2] = { 0 };
	_getcwd(path, 256);
	char mes[16] = "\\MESConnect.ini";
	strcat_s(path, mes);

	GetPrivateProfileStringA("SqlConnect", "prodNo", "", prodNo, sizeof(prodNo), path);     //从ini文件获取制令单号


	/*******SQL数据库指令*******/
	//_variant_t strSQL = "execute checkRoute 'PestpcbSeq','PestprodNo','PeststationNo',0";
	_variant_t strSQL = SqlCommand_BindData(prodNo, Sn, FieldName, FieldValue);

	write_log(pFile, "存储过程SQL语句 = %s\n", (LPCSTR)_bstr_t(strSQL));

	/*******存储过程调用*******/
	/*******打开连接*******/
	try
	{
		hr = m_pConnect->Open(strConnect, "", "", adModeUnknown);  //SQLServer连接数据库
	}
	catch (_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "数据库连接错误 = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 2;
	}
	if (FAILED(hr))
		return 1;

	/*****声明_RecordsetPtr变量，调用Recordset对象的Open方法，即可打开一个数据集*****/
	if (FAILED(pRecordset.CreateInstance(__uuidof(Recordset))))
		return 1;

	/*****执行数据库存储过程调用，并把结果储存在一个数据集合对象*****/
	try
	{
		hr = pRecordset->Open(strSQL, _variant_t((IDispatch*)m_pConnect), adOpenStatic, adLockOptimistic, adCmdText);
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "数据库连接错误 = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 3;
	}
	if (FAILED(hr))
		return 1;

	write_log(pFile, "存储过程SQL语句 = %s\n", (LPCSTR)_bstr_t(strSQL));

	/****通过参数名获取结果****/
	while (!pRecordset->adoEOF)  //获取表中所有数据
	{
		msgId = pRecordset->GetCollect("msgId");      //CheckRoute接口测试结果返回值 0为PASS  1为FAIL
		msgStr = pRecordset->GetCollect("msgStr");    //错误描述  如msgId为0，则msgStr为空
		pRecordset->MoveNext();  //移动到下一条数据
	}

	//解析调用结果：将_variant_t类型转换成测试工具对应类型
	if (msgId.vt != VT_NULL)
	{
		Result = msgId.lVal;
		write_log(pFile, "测试Result = %d\n", Result);
	}

	strcpy(ErrorInfo, (const char *)_bstr_t(msgStr));

	write_log(pFile, "测试Result = %d\n", Result);
	write_log(pFile, "测试ErrorMessage = %s\n", ErrorInfo);

	write_log(pFile, "当前测试流水号接口调用完成： %s\n", Sn);
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
	write_log(pFile, "当前测试流水号： %s\n", Sn);
	
	//定义变量
	_variant_t ResultValue;
	int Result = 1;
	char * ErrorInfo = new char[256];

	/*******初始化环境*******/
	CoInitialize(NULL);  //初始化com
	_ConnectionPtr m_pConnect = NULL;
	HRESULT hr = m_pConnect.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))
		return 1;
	_RecordsetPtr pRecordset;
	/*******初始化环境完成*******/

	/*******开始连接SQLServer数据库*******/
	//获取连接字符串
	//_bstr_t strConnect = "Provider=SQLOLEDB;Data Source=192.168.5.14;User ID=tdmes;Password=Tdmes755473ABC;Initial Catalog=SmartMES";
	string str = ConnectString();
	_bstr_t strConnect = (str.c_str());

	write_log(pFile, "SQLServer数据库连接字串 = %s\n", str.c_str());

	/*****从ini文件获取工单号*****/
	char path[256] = { 0 }, prodNo[256] = { 0 };
	_getcwd(path, 256);
	char mes[16] = "\\MESConnect.ini";
	strcat_s(path, mes);
	GetPrivateProfileStringA("SqlConnect", "prodNo", "", prodNo, sizeof(prodNo), path);

	_variant_t strSQL = SqlCommand_QueryData(prodNo, Sn, FieldName);

	write_log(pFile, "存储过程SQL语句 = %s\n", (LPCSTR)_bstr_t(strSQL));

	/*******存储过程调用*******/
	/*******打开连接*******/
	try
	{
		hr = m_pConnect->Open(strConnect, "", "", adModeUnknown);  //SQLServer连接数据库
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "数据库连接错误 = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 2;
	}
	if (FAILED(hr))
		return 1;

	/*****声明_RecordsetPtr变量，调用Recordset对象的Open方法，即可打开一个数据集*****/
	if (FAILED(pRecordset.CreateInstance(__uuidof(Recordset))))
		return 1;

	/*****执行数据库存储过程调用，并把结果储存在一个数据集合对象*****/
	try
	{
		hr = pRecordset->Open(strSQL, _variant_t((IDispatch*)m_pConnect), adOpenStatic, adLockOptimistic, adCmdText);
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "SQL命令错误 = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 3;
	}
	if (FAILED(hr))
		return 1;

	write_log(pFile, "存储过程SQL语句 = %s\n", (LPCSTR)_bstr_t(strSQL));

	/****通过参数名获取结果****/
	while (!pRecordset->adoEOF)  //获取表中所有数据
	{
		ResultValue = pRecordset->GetCollect(FieldName);
		pRecordset->MoveNext();  //移动到下一条数据
	}

	if (ResultValue.vt != VT_NULL)
	{
		string str = (const char *)_bstr_t(ResultValue);

		strcpy(FieldValue, str.c_str());

		write_log(pFile, "获取串号为 = %s\n", FieldValue);

		Result = 0;
	}
	else
	{
		write_log(pFile, "获取串号为空\n");

		Result = 1;
	}

	write_log(pFile, "当前测试流水号接口调用完成： %s\n", Sn);
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
	write_log(pFile, "当前测试流水号： %s\n", Sn);

	//定义变量
	_variant_t msgId, msgStr;
	int Result = 1;
	char * ErrorInfo = new char[256];

	/*******初始化环境*******/
	CoInitialize(NULL);  //初始化com
	_ConnectionPtr m_pConnect = NULL;
	HRESULT hr = m_pConnect.CreateInstance(__uuidof(Connection));
	if (FAILED(hr))
		return 1;
	_RecordsetPtr pRecordset;
	/*******初始化环境完成*******/

	/*******开始连接SQLServer数据库*******/
	//获取连接字符串
	//_bstr_t strConnect = "Provider=SQLOLEDB;Data Source=192.168.5.14;User ID=tdmes;Password=Tdmes755473ABC;Initial Catalog=SmartMES";
	string str = ConnectString();
	_bstr_t strConnect = (str.c_str());

	write_log(pFile, "SQLServer数据库连接字串 = %s\n", str.c_str());

	/*****获取工单号*****/
	char path[256] = { 0 }, prodNo[256] = { 0 };
	_getcwd(path, 256);
	char mes[16] = "\\MESConnect.ini";
	strcat_s(path, mes);
	GetPrivateProfileStringA("SqlConnect", "prodNo", "", prodNo, sizeof(prodNo), path);

	_variant_t strSQL = SqlCommand_Save2NextStation(Sn, prodNo, Station);

	write_log(pFile, "存储过程SQL语句 = %s\n", (LPCSTR)_bstr_t(strSQL));

	/*******存储过程调用*******/
	/*******打开连接*******/
	try
	{
		hr = m_pConnect->Open(strConnect, "", "", adModeUnknown);  //SQLServer连接数据库
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "数据库连接错误 = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 2;
	}
	if (FAILED(hr))
		return 1;
	write_log(pFile, "存储过程SQL语句 = %s\n", (char*)strConnect);

	/*****声明_RecordsetPtr变量，调用Recordset对象的Open方法，即可打开一个数据集*****/
	if (FAILED(pRecordset.CreateInstance(__uuidof(Recordset))))
		return 1;

	/*****执行数据库存储过程调用，并把结果储存在一个数据集合对象*****/
	try
	{
		hr = pRecordset->Open(strSQL, _variant_t((IDispatch*)m_pConnect), adOpenStatic, adLockOptimistic, adCmdText);
	}
	catch(_com_error e)
	{
		string err = e.Description();
		strcpy(Msg, err.c_str());
		write_log(pFile, "SQL命令错误 = %s\n", Msg);
		TRACE("##########ErrorInfo = %s\n", Msg);
		return 3;
	}
	if (FAILED(hr))
		return 1;

	write_log(pFile, "存储过程SQL语句 = %s\n", (LPCSTR)_bstr_t(strSQL));

	/****通过参数名获取结果****/
	while (!pRecordset->adoEOF)  //获取表中所有数据
	{
		msgId = pRecordset->GetCollect("msgId");
		pRecordset->MoveNext();  //移动到下一条数据
	}
	
	if (msgId.vt != VT_NULL)
	{
		if (msgId.lVal == 1)
		{
			Result = msgId.lVal;
			strcpy(Msg, (const char *)_bstr_t(msgStr));
			write_log(pFile, "测试Result = %d\n", Result);
		}
		else if(msgId.lVal == 0)
		{
			Result = msgId.lVal;
			Msg = NULL;
			write_log(pFile, "测试Result = %d\n", Result);
			write_log(pFile, "测试ErrorMessage = %s\n", Msg);
		}
	}

	write_log(pFile, "错误描述 = %s\n", Msg);

	write_log(pFile, "当前测试流水号接口调用完成： %s\n", Sn);
	write_log(pFile, "------------------------------------------------------------------------------------------------------------------\n");

	fclose(pFile);

	pRecordset->Close();
	pRecordset = NULL;
	m_pConnect->Close();
	m_pConnect = NULL;

	return Result;
}

