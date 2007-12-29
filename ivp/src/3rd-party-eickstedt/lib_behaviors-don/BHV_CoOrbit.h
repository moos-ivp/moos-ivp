/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_CoOrbit.h                                  */
/***********************************************************/
 
#ifndef BHV_COORBIT_HEADER
#define BHV_COORBIT_HEADER

#include <string>
#include <vector>
#include "IvPBehavior.h"
#include "IvPDomain.h"

class BHV_CoOrbit : public IvPBehavior {
public:
  BHV_CoOrbit(IvPDomain);
  ~BHV_CoOrbit() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

 protected:
  bool   parseStatString(std::string, int&, double&, double&);
  double calcPriority(double, double);

protected:
  std::string them_name;

  double  politeness;
  double  wait_speed,cruise_speed;
  int     lag;
  int     legs;
  int     ahead;
  int     them_laps,us_laps,last_them_index,last_us_index;
};
#endif

