/***********************************************************/
/*    NAME: Henrik Schmidt                                  */
/*    ORIGINAL: Don Eickstedt                                  */
/*    FILE: BHV_HCloseRange.h                                  */
/***********************************************************/
 
#ifndef BHV_HCLOSERANGE_HEADER
#define BHV_HCLOSERANGE_HEADER

#include "IvPBehavior.h"

using namespace std;

class IvPDomain;
class BHV_HCloseRange : public IvPBehavior {
public:
  BHV_HCloseRange(IvPDomain);
  ~BHV_HCloseRange() {};
  
  IvPFunction*  onRunState();
  void          onIdleState();
  bool          setParam(std::string, std::string);

protected:
  
  int decode(string);
  double getRelevance(double);

  double heading,speed,ty,tx,range_max,range_min,curr_range,c_course;
  double min_val,max_val,relevance,time;
  int    state;

  int    new_state;
  bool   use_relevance;
  

};
#endif

