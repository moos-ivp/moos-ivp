/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BHV_AllStop.cpp                                      */
/*    DATE: May 26th 2007                                        */
/*****************************************************************/

#ifndef BHV_ALL_STOP_HEADER
#define BHV_ALL_STOP_HEADER

#include "IvPBehavior.h"

class BHV_AllStop : public IvPBehavior {
public:
  BHV_AllStop(IvPDomain);
  ~BHV_AllStop() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);

protected:
};
#endif








