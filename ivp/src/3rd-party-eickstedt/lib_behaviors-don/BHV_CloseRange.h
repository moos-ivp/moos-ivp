/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_CloseRange.h                                  */
/***********************************************************/
 
#ifndef BHV_CLOSERANGE_HEADER
#define BHV_CLOSERANGE_HEADER

#include "IvPBehavior.h"

using namespace std;

class IvPDomain;
class BHV_CloseRange : public IvPBehavior {
public:
  BHV_CloseRange(IvPDomain);
  ~BHV_CloseRange() {};
  
  IvPFunction*  onRunState();
  bool setParam(std::string, std::string);

protected:
  
  int decode(string);
  double getRelevance(double);

  double heading,speed,ty,tx,range_max,range_min,curr_range;
  double min_val,max_val,relevance,time;
  int    state;

  int    new_state;
  bool   use_relevance;
  

};
#endif

