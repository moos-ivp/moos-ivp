/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_ArrayAngleNew.h                            */
/***********************************************************/
 
#ifndef BHV_HARRAYANGLE_HEADER
#define BHV_HARRAYANGLE_HEADER

#include "IvPBehavior.h"

using namespace std;

class IvPDomain;

class BHV_HArrayAngle : public IvPBehavior {
public:
  BHV_HArrayAngle(IvPDomain);
  ~BHV_HArrayAngle() {};
  
  IvPFunction*  onRunState();
  bool setParam(std::string, std::string);
  double getRelevance();

protected:
  
  int decode(string);

  double over_shoot;
  double desired_angle,true_bearing;
  int    width,pwidth;

};
#endif

