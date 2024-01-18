/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PlatModelGenerator.h                                 */
/*    DATE: Oct 24th, 2023                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef PLAT_MODEL_GENERATOR_HEADER
#define PLAT_MODEL_GENERATOR_HEADER

#include <string>
#include "PMGen.h"

class PlatModelGenerator 
{
 public:
  PlatModelGenerator();
  ~PlatModelGenerator() {};

public: // Setters
  PlatModel generate(double osx, double osy, double osh, double osv=0);
  bool setParams(std::string);
  void setCurrTime(double utc) {m_curr_utc=utc;}

  std::string getParamStr(std::string) const;
  double getParamDbl(std::string) const;

  bool stalePModel() {return(true);}

  double getTurnRate();
  
protected:
  void updateHdgHist(double osh);
  
private: // Config vars
  std::string m_alg;
  double m_hdg_hist_length;  // seconds
  
private: // State vars

  bool   m_stale_pmodel;
  
  PMGen *m_pmgen;

  std::list<double> m_recent_val_osh;
  std::list<double> m_recent_utc_osh;

  double m_curr_utc;
  
  unsigned int m_count;
};

#endif 
