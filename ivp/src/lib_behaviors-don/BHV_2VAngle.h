/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_2VAngle.h                                  */
/***********************************************************/
 
#ifndef BHV_2VAngle_HEADER
#define BHV_2VAngle_HEADER

#include "IvPBehavior.h"

#define NO_TRACK 0
#define LR_TRACK 1
#define TRACKING 2

using namespace std;

class IvPDomain;
class BHV_2VAngle : public IvPBehavior {
public:
  BHV_2VAngle(IvPDomain);
  ~BHV_2VAngle() {};
  
  IvPFunction*  onRunState();
  bool setParam(std::string, std::string);

protected:
  
  int decode(string);
  double getRelevance(double,double,double,double);

  double heading,speed,ty,tx,time,range_max,range_min,init_range,curr_range;
  int    state,meas_min,new_state,sign;
  
  

};
#endif

