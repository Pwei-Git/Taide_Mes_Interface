#ifndef __MAIN_H__
#define __MAIN_H__

#include <iostream>
#include <direct.h>
#include <Windows.h>
#include <sstream>
#include <string>
#include <time.h>
#include <fstream>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

#ifdef __cplusplus
extern "C"
{
#endif

#define MESAPI _declspec(dllexport)  
#define TDAPI

typedef struct _tagMesParamDataInfo
{
    char DataSource[256];      // 数据库IP
	char UserID[256];          // 账号
	char Password[256];        // 密码
	char InitialCatalog[256];  // 数据库名 

} MES_PARAM_DATA_INFO, *PMES_PARAM_DATA_INFO;


#ifdef __cplusplus
}
#endif

#endif   //__MAIN_H__