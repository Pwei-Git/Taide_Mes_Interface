#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_Save2NextStation : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_Save2NextStation)
public:
    CMES_Save2NextStation(void);
    virtual ~CMES_Save2NextStation(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);
};
