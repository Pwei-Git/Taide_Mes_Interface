#include "stdafx.h"
#include "main.h"
#include <direct.h>

string ConnectString()
{
	MES_PARAM_DATA_INFO ConnectInfo = { 0x00 };
	char Separate = ';';
	string connectInfo[4] = { "Data Source=" , "User ID=", "Password=", "Initial Catalog=" };
	string connectString = "Provider=SQLOLEDB;";

	char path[256] = { 0 }, szStation[256] = { 0 };
	_getcwd(path, 256);
	char mes[16] = "\\MESConnect.ini";
	strcat_s(path, mes);
	GetPrivateProfileStringA("SqlConnect", "DataSource", "", ConnectInfo.DataSource, sizeof(ConnectInfo.DataSource), path);
	GetPrivateProfileStringA("SqlConnect", "UserID", "", ConnectInfo.UserID, sizeof(ConnectInfo.UserID), path);
	GetPrivateProfileStringA("SqlConnect", "Password", "", ConnectInfo.Password, sizeof(ConnectInfo.Password), path);
	GetPrivateProfileStringA("SqlConnect", "InitialCatalog", "", ConnectInfo.InitialCatalog, sizeof(ConnectInfo.InitialCatalog), path);
	string connectInfoValue[4] = { ConnectInfo.DataSource,  ConnectInfo.UserID, ConnectInfo.Password, ConnectInfo.InitialCatalog };

	for (int i = 0; i < 4; i++)
	{
		if (i < 3)
		{
			connectString = connectString + connectInfo[i] + connectInfoValue[i] + Separate;
		}
		if (i == 3)
		{
			connectString = connectString + connectInfo[i] + connectInfoValue[i];
		}
	}
	return connectString;
}

_variant_t SqlCommand_Check_Route(char * pcbSeq, char * prodNo, char * stationNo, char * retest)
{
	string commandHeadler = "execute checkRoute ";
	string symbol = "'";
	string comma = ",";

	string PcbaSn = symbol + pcbSeq + symbol;
	string ProdNo = symbol + prodNo + symbol;
	string StationNo = symbol + stationNo + symbol;
	string Retest = retest;

	string command_Parameter[4] = { PcbaSn, ProdNo, StationNo, Retest };

	for (int i = 0; i < 4; i++)
	{
		if (i == 3)
		{
			commandHeadler = commandHeadler + command_Parameter[i];
		}
		else
		{
			commandHeadler = commandHeadler + command_Parameter[i] + comma;
		}
	}
	const char *SqlCommand = commandHeadler.c_str();

	return _variant_t(SqlCommand);

	//return commandHeadler;
}

_variant_t SqlCommand_QueryData(char * prodNo, char * pcbSeq, char * fieldName)
{
	string commandHeadler = "exec getProdField ";
	string symbol = "'";
	string comma = ",";

	string PcbaSn = symbol + prodNo + symbol;
	string FieldName = symbol + pcbSeq + symbol;
	string FieldValue = symbol + fieldName + symbol;

	string command_Parameter[3] = { PcbaSn, FieldName, FieldValue };

	for (int i = 0; i < 3; i++)
	{
		if (i == 2)
		{
			commandHeadler = commandHeadler + command_Parameter[i];
		}
		else
		{
			commandHeadler = commandHeadler + command_Parameter[i] + comma;
		}
	}
	const char *SqlCommand = commandHeadler.c_str();

	return _variant_t(SqlCommand);

	//return commandHeadler;
}

_variant_t SqlCommand_Save2NextStation(char * pcbSeq, char * prodNo, char * stationNo)
{
	string commandHeadler = "execute createRoute ";
	string symbol = "'";
	string comma = ",";

	string PcbaSn = symbol + pcbSeq + symbol;
	string ProdNo = symbol + prodNo + symbol;
	string StationNo = symbol + stationNo + symbol;
	string Result = symbol + "PASS" + symbol;
	string Item5 = symbol + "" + symbol;
	string User = symbol + "admin" + symbol;
	string Item8_12 = symbol + "" + symbol;

	string command_Parameter[6] = { PcbaSn, ProdNo, StationNo ,Result ,Item5 , User };

	for (int i = 0; i < 12; i++)
	{
		if (i < 6)
		{
			commandHeadler = commandHeadler + command_Parameter[i] + comma;
		}
		else if (i == 6)
		{
			commandHeadler = commandHeadler + "0,";
		}
		else if (i == 11)
		{
			commandHeadler = commandHeadler + Item8_12;
		}
		else
		{
			commandHeadler = commandHeadler + Item8_12 + comma;
		}
	}
	const char *SqlCommand = commandHeadler.c_str();

	return _variant_t(SqlCommand);

	//return commandHeadler;
}


_variant_t SqlCommand_BindData(char* ProNo, char * pcbSeq, char * fieldName, char * fieldValue)
{
	string commandHeadler = "exec UpdateProdField ";
	string symbol = "'";
	string comma = ",";

	string proNo = symbol + ProNo + symbol;
	string PcbaSn = symbol + pcbSeq + symbol;
	string FieldName = symbol + fieldName + symbol;
	string FieldValue = symbol + fieldValue + symbol;

	string command_Parameter[4] = { proNo, PcbaSn, FieldName, FieldValue };

	for (int i = 0; i < 4; i++)
	{
		if (i == 3)
		{
			commandHeadler = commandHeadler + command_Parameter[i];
		}
		else
		{
			commandHeadler = commandHeadler + command_Parameter[i] + comma;
		}
	}
	const char *SqlCommand = commandHeadler.c_str();

	return _variant_t(SqlCommand);

	//return commandHeadler;
}

/*****¼ÇÂ¼ÈÕÖ¾*****/
int write_log(FILE* pFile, const char *format, ...)
{
	va_list arg;
	int done;

	va_start(arg, format);

	time_t time_log = time(NULL);
	struct tm* tm_log = localtime(&time_log);
	fprintf(pFile, "%04d-%02d-%02d %02d:%02d:%02d ", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);

	done = vfprintf(pFile, format, arg);
	va_end(arg);

	fflush(pFile);
	return done;
}

int replace(char* Dst, int DsrLen, char chr1, char chr2)
{
	if(Dst != NULL)
	{
		char * End = Dst;
		for(int i = 0; i < DsrLen; i++)
		{
			if(End[i] == chr1)
			{
				End[i] = chr2;
			}
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

string GetLogName(char * Sn)
{
	time_t t = time(0);
	char tmp[64] = { 0 };
	char format[5] = ".txt";
	struct tm* tm_log = localtime(&t);
	strftime(tmp, sizeof(tmp), "%Y-%m-%d-", localtime(&t));
	//replace(tmp, sizeof(tmp), ':', '-');
	char logName[256] = { 0 };
	memset(logName, 0x00, sizeof(logName));
	char Data[64] = { 0 };
	strcpy_s(Data, tmp);
	strcat_s(Data, Sn);
	strcat_s(logName, Data);
	strcat_s(logName, format);
	return logName;
}

string GetLogFolder()
{
	time_t t = time(0);
	char data[64] = { 0 }, path[256] = { 0 };
	char format[128] = "\\InterfaceLog\\";

	struct tm* tm_log = localtime(&t);
	strftime(data, sizeof(data), "%Y-%m-%d", localtime(&t));
	char logFolder[256] = { 0 };
	_getcwd(path, sizeof(path));
	TRACE("#####TestLogFolderPath = %s", path);
	strcat_s(format, data);
	strcat_s(path, format);
	return path;
}