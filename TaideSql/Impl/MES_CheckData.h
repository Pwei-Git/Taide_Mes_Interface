#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_CheckData : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_CheckData)
public:
    CMES_CheckData(void);
    virtual ~CMES_CheckData(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
