/***************************************************************/
/*    NAME: Michael Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                                  */
/*    FILE: VelocityFilter.h                                   */
/*    DATE: Apr 23rd, 2022                                     */
/*                                                             */
/* This is unreleased BETA code. No permission is granted or   */
/* implied to use, copy, modify, and distribute this software  */
/* except by the author(s), or those designated by the author. */
/***************************************************************/

#ifndef VELOCITY_FILTER_HEADER
#define VELOCITY_FILTER_HEADER

#include <string>

class VelocityFilter
{
 public:
  VelocityFilter();
  ~VelocityFilter() {}; 

 public: // Setters
  void   setMinSpd(double v)    {m_min_spd=v;}
  void   setMaxSpd(double v)    {m_max_spd=v;}
  void   setMinSpdPct(double v) {m_min_spd_pct=v;}

  void   setSpdOS(double v) {m_osv=v;}
  void   setSpdCN(double v) {m_cnv=v;}
  
 public: // Getters
  double getMinSpd()    const {return(m_min_spd);}
  double getMaxSpd()    const {return(m_max_spd);}
  double getMinSpdPct() const {return(m_min_spd_pct);}
  double getSpdOS()     const {return(m_osv);}
  double getSpdCN()     const {return(m_cnv);}

  double getFilterPct() const;
  
  std::string getSpec() const {return("");}

  bool valid() const;
  
 private: // config vars
  double   m_min_spd;       // meters/sec
  double   m_max_spd;       // meters/sec
  double   m_min_spd_pct;   // range [0,100]

 private: // state vars
  double   m_osv;           // meters/sec
  double   m_cnv;           // meters/sec
};

VelocityFilter stringToVelocityFilter(std::string);

#endif 
