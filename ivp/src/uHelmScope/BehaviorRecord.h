/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: MIT Cambridge MA                                     */
/*    FILE: BehaviorRecord.h                                     */
/*    DATE: Mar 21st 2011                                        */
/*****************************************************************/

#ifndef BEHAVIOR_RECORD_HEADER
#define BEHAVIOR_RECORD_HEADER

#include <vector>
#include <string>

class BehaviorRecord
{
public:
  BehaviorRecord();
  virtual ~BehaviorRecord() {};

  // Setters
  void   setTimeStamp(double v);
  void   setName(std::string s)      {m_bhv_name = s;};
  void   setPriority(std::string s)  {m_priority = s;};
  void   setTimeCPU(std::string s)   {m_time_cpu = s;};
  void   setPieces(std::string s)    {m_pieces = s;};
  void   setUpdates(std::string s)   {m_updates = s;};

  // Getters
  double getTimeStamp() const       {return(m_time_stamp);};
  std::string getName() const       {return(m_bhv_name);};
  std::string getTimeCPU() const    {return(m_time_cpu);};
  std::string getPriority() const   {return(m_priority);};
  std::string getPieces() const     {return(m_pieces);};
  std::string getUpdates() const    {return(m_updates);};

  std::string getSummary(double tstamp=0) const;

protected:
  double      m_time_stamp;
  bool        m_set_time_stamp;

  std::string m_bhv_name;
  std::string m_time_cpu;
  std::string m_priority;
  std::string m_pieces;
  std::string m_updates;

};

#endif 
