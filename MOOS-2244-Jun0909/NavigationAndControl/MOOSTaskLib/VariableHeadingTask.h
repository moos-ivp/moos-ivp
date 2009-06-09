#ifndef VARIABLEHEADINGH
#define VARIABLEHEADINGH
#include "ConstantHeadingTask.h"

class CVariableHeadingTask : public CConstantHeadingTask
{
private:
    typedef CConstantHeadingTask BASE;
public:
    CVariableHeadingTask(void);
    ~CVariableHeadingTask(void);

            bool OnNewMail(MOOSMSG_LIST &NewMail);
            bool Run(CPathAction &DesiredAction);
    virtual bool GetRegistrations(STRING_LIST &List);
    virtual bool RegularMailDelivery(double dfTimeNow);

protected:
    std::map<std::string,double> m_SetPointTimes;


 };
#endif
