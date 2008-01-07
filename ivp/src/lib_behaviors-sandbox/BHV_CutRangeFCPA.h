/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_CutRangeFCPA.h                                   */
/*    DATE: Nov 4th 2006                                         */
/*****************************************************************/
 
#ifndef BHV_CUTRANGE_FCPA_HEADER
#define BHV_CUTRANGE_FCPA_HEADER

#include <string>
#include "IvPBehavior.h"

class IvPDomain;
class BHV_CutRangeFCPA : public IvPBehavior {
public:
  BHV_CutRangeFCPA(IvPDomain);
  ~BHV_CutRangeFCPA() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  double getRelevance(double, double, double, double);
  double getPriority();
  
private:
  std::string them_name; // Name for them in InfoBuffer
  double range_max;      // if target not in range, 0 relevance
  double range_min;      // if target not in range, 0 relevance
  double tol;            // Time On Leg
};
#endif








