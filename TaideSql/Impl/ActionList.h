
#include "../resource.h"

#ifdef ACTION
/*     
            FnTag,            Version,       ClassName,          ActionName,        rcID,            rcName,                Description     
    ----------------------------------------------------------------------------------------------------------------------------------------
    Example:
    ==
    1. Action with RC 
    ACTION(L"Dummy",       MAKEWORD(0, 1),  L"CDummyAction",    L"Dummy",      IDR_XML_DUMMY,   TOWSTR(IDR_XML_DUMMY),  L"Action with    RC"),
    
    2. Action without RC
    ACTION(L"Dummy",       MAKEWORD(0, 1),  L"CDummyAction",    L"Dummy",      0,               NULL,                   L"Action without RC"),

 */
	ACTION(L"TaideMES", L"MES_CheckRoute",  L"CMES_CheckRoute",  L"MES_CheckRoute", IDR_XML_CHECK_ROUTE,     TOWSTR(IDR_XML_CHECK_ROUTE),    L"MES_CheckRoute [MES检查过站]"),
    ACTION(L"TaideMES", L"MES_Save2NextStation",  L"CMES_Save2NextStation",    L"MES_Save2NextStation",    0,     NULL,    L"MES_Save2NextStation(MES测试通过增加过站记录)"),
    ACTION(L"TaideMES", L"MES_BindData",  L"CMES_BindData",    L"MES_BindData",    0,     NULL,    L"MES_BindData[MES串号绑定]"),
	ACTION(L"TaideMES", L"MES_QueryData",  L"CMES_QueryData",    L"MES_QueryData",    0,     NULL,    L"MES_QueryData[MES串号查询比较]"),
	ACTION(L"TaideMES", L"MES_Init",  L"CMES_Init",    L"Init",    0,     NULL,    L"Init[重新获取接口地址]"),

#endif 
