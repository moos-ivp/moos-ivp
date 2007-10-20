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
  void   setRepeat(int);
  void   setCurrIndex(int);
  
  double getCurrX()       {return(m_curr_x);};
  double getCurrY()       {return(m_curr_y);};
  double getPointX(int);
  double getPointY(int);  
  double getPointX()      {return(m_seglist.get_vx(m_curr_ix));};
  double getPointY()      {return(m_seglist.get_vy(m_curr_ix));};
  bool   isComplete()     {return(m_complete);};
  int    getCurrIndex()   {return(m_curr_ix);};

  XYSegList getSegList()  {return(m_seglist);};

  void   updatePosition(double);

 protected:
  XYSegList m_seglist;
  XYSegList m_seglist_raw;

  int       m_curr_ix;
  bool      m_reverse;
  bool      m_complete;

  double    m_start_time;  // seconds
  double    m_start_x;     // meters
  double    m_start_y;     // meters
  double    m_velocity;    // m/s
  double    m_curr_x;
  double    m_curr_y;
};

#endif






