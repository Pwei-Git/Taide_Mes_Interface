#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_QueryData : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_QueryData)
public:
    CMES_QueryData(void);
    virtual ~CMES_QueryData(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
