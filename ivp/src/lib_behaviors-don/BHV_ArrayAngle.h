/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_ArrayAngle.h                            */
/***********************************************************/
 
#ifndef BHV_ARRAYANGLE_HEADER
#define BHV_ARRAYANGLE_HEADER

#include "IvPBehavior.h"

using namespace std;

class IvPDomain;

class BHV_ArrayAngle : public IvPBehavior {
public:
  BHV_ArrayAngle(IvPDomain);
  ~BHV_ArrayAngle() {};
  
  IvPFunction*  onRunState();
  bool setParam(std::string, std::string);
  double getRelevance();

protected:
  
  int decode(string);

  double desired_angle,true_bearing;
  int width,pwidth;

};
#endif

