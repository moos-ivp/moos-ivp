/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMGen_Holonomic.h                                    */
/*    DATE: Oct 24th, 2023                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef PMGEN_HOLONOMIC_HEADER
#define PMGEN_HOLONOMIC_HEADER

#include <string>
#include "PMGen.h"

class PMGen_Holonomic : public PMGen
{
 public:
  PMGen_Holonomic() {};
  ~PMGen_Holonomic() {};

public: // Setters
  PlatModel generate(double osx, double osy,
		     double osh, double osv);

  bool setParam(std::string, std::string) {return(true);}
  bool setParam(std::string, double) {return(true);}

  std::string getAlg() const {return("holo");}
};

#endif 
