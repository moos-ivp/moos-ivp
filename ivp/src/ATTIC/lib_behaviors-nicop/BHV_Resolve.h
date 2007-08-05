/***********************************************************/
/*    NAME: M. Benjamin / P. Newman                        */
/*    FILE: BHV_Resolve.h                                  */
/*    DATE: Sep 07 2005                                    */
/***********************************************************/
 
#ifndef BHV_RESOLVE_HEADER
#define BHV_RESOLVE_HEADER

#include <string>
#include <vector>
#include "IvPBehavior.h"
#include "Resolver.h"

class BHV_Resolve : public IvPBehavior {
public:
  BHV_Resolve(IvPDomain);
  ~BHV_Resolve() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);
  
 protected:
  double getRelevance();

protected:
  double    osX;
  double    osY;

  CResolver   resolver;
  std::string history;
  double      time_since_upgrade;
};
#endif

