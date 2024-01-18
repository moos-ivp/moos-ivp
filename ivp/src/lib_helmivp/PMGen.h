/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMGen.h                                              */
/*    DATE: Oct 24th, 2023                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef PMGEN_HEADER
#define PMGEN_HEADER

#include <string>
#include <list>
#include "PlatModel.h"

class PMGen
{
 public:
  PMGen() {};
  virtual ~PMGen() {};

public: // Setters
  virtual PlatModel generate(double osx, double osy,
			     double osh, double osv) = 0;

  virtual bool setParam(std::string, std::string) {return(true);}

  virtual std::string getAlg() const {return("");}
  virtual std::string getParamStr(std::string) const {return("");}
  virtual double getParamDbl(std::string) const {return(0);}  

  void setHdgHist(std::list<double> hdgs, std::list<double> utcs);

  double getHdgAvg(double time_window);
  
protected:
  std::string m_algorithm;

  std::list<double> m_recent_val_osh;
  std::list<double> m_recent_utc_osh;
};

#endif 
