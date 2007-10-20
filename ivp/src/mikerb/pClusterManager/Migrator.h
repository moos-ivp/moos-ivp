/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Migrator.h                                           */
/*    DATE: May 10th, 2007                                       */
/*****************************************************************/
 
#ifndef MIGRATOR_UTILITY_HEADER
#define MIGRATOR_UTILITY_HEADER

#include "XYSegList.h"

class Migrator {
 public:
  Migrator();
  ~Migrator() {};

  void   setSegList(double x, double y);
  void   setSegList(const XYSegList& seglist);
  void   setInitialConditions(double, double, double);
  void   setReverse(bool);
  void   setCurrIndex(int);
  
  double getCurrX() const      {return(m_curr_x);};
  double getCurrY() const      {return(m_curr_y);};
  double getPointX(int) const;
  double getPointY(int) const;  
  double getPointX() const     {return(m_seglist.get_vx(m_curr_ix));};
  double getPointY() const     {return(m_seglist.get_vy(m_curr_ix));};

  bool   isComplete() const    {return(m_complete);};
  int    getCurrIndex() const  {return(m_curr_ix);};
  int    size() const          {return(m_seglist.size());};

  XYSegList getSegList() const {return(m_seglist);};

  void   updatePosition(double);

  std::string getFlagVar() const   {return(m_flag_var);};
  std::string getFlagSVar() const  {return(m_flag_sval);};
  std::string getUpdateVar() const {return(m_update_var);};  
  bool   isActive() const          {return(m_active);};
  void   setActive(bool v)           {m_active = v;};
  void   setFlagVar(std::string s)   {m_flag_var = s;};
  void   setFlagSVal(std::string s)  {m_flag_sval = s;};
  void   setUpdateVar(std::string s) {m_update_var = s;};

 protected:
  XYSegList m_seglist;
  XYSegList m_seglist_raw;

  int       m_curr_ix;
  bool      m_reverse;
  bool      m_complete;
  bool      m_active;

  std::string m_flag_var;
  std::string m_flag_sval;
  std::string m_update_var;
  
  double    m_start_time;  // seconds
  double    m_start_x;     // meters
  double    m_start_y;     // meters
  double    m_velocity;    // m/s
  double    m_curr_x;
  double    m_curr_y;
};

#endif






