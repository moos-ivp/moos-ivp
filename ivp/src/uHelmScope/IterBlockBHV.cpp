/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IterBlockBHV.cpp                                     */
/*    DATE: Apr 12th 2008                                        */
/*****************************************************************/

#include "IterBlockBHV.h"

using namespace std;

//------------------------------------------------------------
// Procedure: addErrorMsg()
// Procedure: addWarningMsg()
// Procedure: addPostingMsg()
// Procedure: addInfoMsg()

void IterBlockBHV::addErrorMsg(string msg)
{
  m_error_msgs.push_back(msg);
}
    
void IterBlockBHV::addWarningMsg(string msg)
{
  m_warning_msgs.push_back(msg);
}
    
void IterBlockBHV::addPostingMsg(string msg)
{
  m_posting_msgs.push_back(msg);
}
    
void IterBlockBHV::addInfoMsg(string msg)
{
  m_info_msgs.push_back(msg);
}
    

