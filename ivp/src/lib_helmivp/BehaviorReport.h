/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BehaviorReport.h                                     */
/*    DATE: Apr 16th 2008                                        */
/*****************************************************************/

#ifndef BEHAVIOR_REPORT_HEADER
#define BEHAVIOR_REPORT_HEADER

#include <string>
#include <vector>
#include "VarDataPair.h"

class BehaviorReport {
friend class BehaviorSet;
public:
  BehaviorReport() {reset();};
  virtual ~BehaviorReport() {};
  
 public:
  void         reset();
  std::string  getDescriptor()    {return(m_descriptor);};
  std::string  getUpdateVar()     {return(m_update_var);};
  int          getGoodUpdates()   {return(m_good_updates);};
  int          getBadUpdates()    {return(m_bad_updates);};
  double       getLastUpdateAge() {return(m_update_age);};
  double       getDuration()      {return(m_duration);};
  double       getDurationLeft()  {return(m_duration_left);};
  double       getStartTime()     {return(m_start_time);};
  std::string  getState()         {return(m_state);};
  int          getPieces()        {return(m_pieces);};
  double       getPriority()      {return(m_priority);};
  double       getCreateTime()    {return(m_create_time);};
  std::string  getUnifPiece()     {return(m_unif_piece);};
  double       getAvgError()      {return(m_avg_error);};
  double       getWstError()      {return(m_wst_error);};
  int          getSamples()       {return(m_samples);};

 protected: // relevant always
  std::string  m_descriptor;

  std::string  m_update_var;
  int          m_good_updates;
  int          m_bad_updates;
  double       m_update_age;

  std::vector<VarDataPair>  m_messages;

  double       m_duration;
  double       m_start_time;

 protected: // unknown until runtime
  std::string  m_state;

 protected: // relevant when active
  int         m_pieces;
  double      m_priority;
  double      m_duration_left;
  double      m_create_time;
  
 protected: // relevant when active and N>1 dimensions
  std::string m_unif_piece;
  double      m_avg_error;
  double      m_wst_error;
  int         m_samples;
};

#endif 
