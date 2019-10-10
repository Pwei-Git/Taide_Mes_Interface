#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_BindData : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_BindData)
public:
    CMES_BindData(void);
    virtual ~CMES_BindData(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
