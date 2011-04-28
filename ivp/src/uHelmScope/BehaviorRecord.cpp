/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: BehaviorRecord.cpp                                   */
/*    DATE: March 21st 2011                                      */
/*****************************************************************/

#include "MBUtils.h"
#include "BehaviorRecord.h"

using namespace std;

//------------------------------------------------------------
// Constructor

BehaviorRecord::BehaviorRecord()
{
  m_time_stamp     = 0;
  m_set_time_stamp = false;
}

//------------------------------------------------------------
// Procedure: setTimeStamp

void BehaviorRecord::setTimeStamp(double v)
{
  m_time_stamp = v;
  m_set_time_stamp = true;
}

//------------------------------------------------------------
// Procedure: getSummary()

string BehaviorRecord::getSummary(double tstamp) const
{
  string summary = m_bhv_name + " ";

  if(m_set_time_stamp) {
    if(m_time_stamp <= 0)
      summary += "[always]";
    else {
      double elapsed_time = m_time_stamp;
      if(tstamp >= m_time_stamp)
	elapsed_time = (tstamp - m_time_stamp);
      summary += "[" + doubleToString(elapsed_time, 2) + "] ";
    }
  }
  else
    summary += "[always]";
  
  if(m_priority != "")
    summary += "(pwt=" + m_priority + ") ";
  if(m_pieces != "")
    summary += "(pcs=" + m_pieces + ") ";
  if(m_time_cpu != "")
    summary += "(cpu=" + m_time_cpu + ") ";
  if(m_updates != "")
    summary += "(upd=" + m_updates + ")";

  return(summary);
}
