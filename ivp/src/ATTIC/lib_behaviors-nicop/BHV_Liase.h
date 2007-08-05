/***********************************************************/
/*    NAME: Michael Benjamin                               */
/*    FILE: BHV_Liase.h                                    */
/*    DATE: Sept 2005                                      */
/***********************************************************/
 
#ifndef BHV_LIASE_HEADER
#define BHV_LIASE_HEADER

#include <string>
#include <vector>
#include "IvPBehavior.h"
//#include "XYPolygon.h"

class BHV_Liase : public IvPBehavior {
public:
  BHV_Liase(IvPDomain);
  ~BHV_Liase() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);

protected:
  bool setNextLiasePoint();

protected:
  std::string them_name; // Name for them in InfoBuffer

  double    cruise_speed;
  double    osX;
  double    osY;
  double    ptX;
  double    ptY;

};
#endif

