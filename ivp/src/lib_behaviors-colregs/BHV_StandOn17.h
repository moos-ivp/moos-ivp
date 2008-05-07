/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_StandOn17.h                                      */
/*    DATE:                                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
 
#ifndef BHV_STANDON_HEADER
#define BHV_STANDON_HEADER

#include <string>
#include "IvPBehavior.h"

extern "C" IvPBehavior *createBehavior(IvPDomain d);  

class IvPDomain;
class BHV_StandOn17 : public IvPBehavior {
public:
  BHV_StandOn17(IvPDomain);
  ~BHV_StandOn17() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  double getRelevance(double, double, double, double, double, double, double, double);
  double getPriority();
  
private:
  std::string them_name; // Name for them in InfoBuffer

  double range;

};
#endif







