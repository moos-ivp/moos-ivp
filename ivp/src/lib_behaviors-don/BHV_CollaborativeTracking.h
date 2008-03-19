/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_CollaborativeTracking.h                    */
/***********************************************************/
 
#ifndef BHV_COLLABORATIVETRACKING_HEADER
#define BHV_COLLABORATIVETRACKING_HEADER

#include "IvPBehavior.h"

using namespace std;

class IvPDomain;

class BHV_CollaborativeTracking : public IvPBehavior {
public:
  BHV_CollaborativeTracking(IvPDomain);
  ~BHV_CollaborativeTracking() {};
  
  IvPFunction*  onRunState();
  bool setParam(std::string, std::string);

protected:
  
  int decode(string);
  double getRelevance();

  double sigmapos,sigmatheta,ty,tx,heading,speed,time; 
  int state,node;
};
#endif

