#pragma once


/* =========================================================
����ԭ��: int Init(char* pszData, int iDataLen) ;  
��������: �ӿں���
�������: pszData -- ��Ҫ���������ָ���׵�ַ 
          iDataLen -- ���ݳ��� 
�������: ��
����ֵ: �ο� ENUM_MES_RESULT_TYPE 
==========================================================*/

typedef int (*pCheckRoute)(char* Sn, char* Station, char* Msg) ;
typedef int (*pSave2NextStation)(char* Sn, char* Station, char* Msg) ;
typedef int (*pBindData)(char* Sn, char* FieldName, char* FieldValue, char* Msg) ;
typedef int (*pQueryData)(char* Sn, char* FieldName, char* FieldValue, char* Msg) ;

typedef struct _tagMES_DRIVER_T
{
	pCheckRoute CheckRoute ;
	pSave2NextStation Save2NextStation ;
	pBindData BindData;
	pQueryData QueryData;

    _tagMES_DRIVER_T(void) {
        Reset();
    };

    void Reset(void) {
		CheckRoute = NULL;
		Save2NextStation = NULL;
		BindData = NULL;
		QueryData = NULL;
   };

    BOOL IsValid(void) {
        return (0x01
					&& (NULL != CheckRoute) 
					&& (NULL != Save2NextStation) 
					&& (NULL != BindData) 
					&& (NULL != QueryData) 
			);
    };

} MES_DRIVER_T;

