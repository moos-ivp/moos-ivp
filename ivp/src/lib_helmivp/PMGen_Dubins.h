/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMGen_Dubins.h                                       */
/*    DATE: Oct 24th, 2023                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef PMGEN_DUBINS_HEADER
#define PMGEN_DUBINS_HEADER

#include <string>
#include <vector>
#include "XYSeglr.h"
#include "PMGen.h"

class PMGen_Dubins : public PMGen
{
 public:
  PMGen_Dubins();
  ~PMGen_Dubins() {};

public: // Setters
  PlatModel generate(double osx, double osy,
		     double osh, double osv);

  bool setParam(std::string, std::string);
  bool setParam(std::string, double);

  std::string getParamStr(std::string) const {return("");}
  double getParamDbl(std::string) const;

  bool valid() const;

  std::string getAlg() const {return("dubins");}

protected:
  bool init(double osx, double osy, double osh);
  
protected:
  bool   m_valid;
  
  double m_radius;
  double m_spoke_degs;

  // Vector ix corresponds to one unit m_spoke_degs
  std::vector<double>  m_star_spoke_vx;
  std::vector<double>  m_star_spoke_vy;
  std::vector<double>  m_port_spoke_vx;
  std::vector<double>  m_port_spoke_vy;
  std::vector<XYSeglr> m_star_seglrs;
  std::vector<XYSeglr> m_port_seglrs;
};

#endif 
