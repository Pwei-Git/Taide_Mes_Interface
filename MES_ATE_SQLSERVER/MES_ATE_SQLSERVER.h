#ifndef __MES_ATE_SQLSERVER_H__ 
#define __MES_ATE_SQLSERVER_H__ 


#ifdef __cplusplus
extern "C"
{
#endif



#define MESAPI _declspec(dllexport)  
#define TDAPI // __stdcall

MESAPI int TDAPI CheckRoute(char* Sn, char* Station, char* Msg) ;
MESAPI int TDAPI Save2NextStation(char* Sn, char* Station) ;
MESAPI int TDAPI BindData(char* Sn, char* DataType, char* DataValue) ;
MESAPI int TDAPI QueryData(char* Sn, char* DataType, char* DataValue) ;


#ifdef __cplusplus
}
#endif

#endif // __MES_ATE_SQLSERVER_H__


