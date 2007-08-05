/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI - MIT Dept. Mechanical Engineering */
/*    FILE: BHV_NICOP.h                                          */
/*    DATE: Dec 2004                                             */
/*****************************************************************/
 
#ifndef BHV_NICOP_HEADER
#define BHV_NICOP_HEADER

#include <string>
#include "IvPBehavior.h"

class BHV_NICOP : public IvPBehavior {
public:
  BHV_NICOP(IvPDomain);
  ~BHV_NICOP() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);

protected:
  bool      completed;

protected: // intermediate or object global variables.
  double    osSPD;
  double    osX;
  double    osY;
};
#endif


