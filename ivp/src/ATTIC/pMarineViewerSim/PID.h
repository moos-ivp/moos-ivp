/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PID.h                                                */
/*    DATE: May 14th 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

/******************************************************************/
/* This file is a slightly augmented version of P.Newman's        */
/* ScalarPID class found in MOOSGenLib.                           */
/*                                                                */
/* It is augmented w/ a copy constructor and assignment operator. */
/*                                                                */
/* All variable names remain the same, some comments are added.   */
/*                                                                */
/* Name of the class is changed from "CScalarPID" to "ScalarPID"  */
/******************************************************************/

#ifndef PID_HEADER
#define PID_HEADER

#include <string>
#include <list>

class PID  
{
public:
  PID() {};
  PID(double dfKp, double dfKd, double dfKi,
	    double dfIntegralLimit, double dfOutputLimit);
  PID(const PID&);
  virtual ~PID() {};
  
public:
  const PID &operator=(const PID&);
  void   SetLimits(double dfIntegralLimit, double dfOutputLimit);
  void   SetGains(double dfKp, double dfKd, double dfKi);
  double Run(double dfeIn, double dfErrorTime);

 public:
  double get_Ki() {return(m_dfKi);};
  double get_Kd() {return(m_dfKi);};
  double get_Kp() {return(m_dfKi);};
  double get_ilim() {return(m_dfIntegralLimit);};
  double get_olim() {return(m_dfOutputLimit);};

protected: // Central parameters
  double m_dfKi;
  double m_dfKd;
  double m_dfKp;
  double m_dfIntegralLimit;
  double m_dfOutputLimit;

protected: // Data persistent between runs
  double m_dfeOld;
  double m_dfOldTime;
  double m_dfOut;
  int    m_nHistorySize;
  std::list<double> m_DiffHistory;

  double m_dfe;
  double m_dfeSum;
  double m_dfeDiff;
  double m_dfDT;

  int    m_nIterations;

};

#endif




