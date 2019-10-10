#pragma once
#include "ImpBase.h"

//////////////////////////////////////////////////////////////////////////
class CMES_CheckRoute : public CImpBase
{
    DECLARE_RUNTIME_CLASS(CMES_CheckRoute)
public:
    CMES_CheckRoute(void);
    virtual ~CMES_CheckRoute(void);

protected:
    virtual BOOL    LoadXMLConfig(void);
    virtual SPRESULT __PollAction(void);

private:
    std::string m_strProject;

    enum UPLOAD
    {
        NONE  = 0,
        SYNC  = 1,
        ASYNC = 2
    } m_eUpload;
};
