/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ThrustMap.h                                          */
/*    DATE: Dec 16th 2010                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef THRUST_MAP_HEADER
#define THRUST_MAP_HEADER

#include <string>
#include <vector>
#include <map>
#include "Figlog.h"

class ThrustMap
{
public:
  ThrustMap();
  ~ThrustMap() {}

 public: // Setters
  bool   addPair(double, double);
  void   setThrustFactor(double);
  void   setMinMaxThrust(double, double);
  void   setReflect(bool v)  {m_reflect_negative=v;}
 
 public: // Getters
  double getSpeedValue(double thrust) const;
  double getThrustValue(double speed) const;
  double getThrustFactor() const;
  bool   isAscending() const;
  bool   isConfigured();
  bool   usingThrustFactor() const;
  bool   usingReflect() const {return(m_reflect_negative);}

  std::string getMapPos() const;
  std::string getMapNeg() const;

 public: // Actions
  void   print() const;
  void   clear();
  void   clearFiglog() {m_figlog.clear();}

 protected:
  bool   isAscendingMap(std::map<double, double>) const;
  double getSpeedValueNeg(double thrust) const;
  double getSpeedValuePos(double thrust) const;
  double getThrustValueNeg(double speed) const;
  double getThrustValuePos(double speed) const;

 protected:
  std::map<double, double> m_pos_mapping;
  std::map<double, double> m_neg_mapping;

  double m_min_thrust;
  double m_max_thrust;
  bool   m_reflect_negative;

  double m_thrust_factor;

  Figlog m_figlog;
};

#endif 














