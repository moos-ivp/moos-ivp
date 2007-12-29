/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_ArrayTurn.h                                  */
/***********************************************************/
 
#ifndef BHV_ARRAYTURN_HEADER
#define BHV_ARRAYTURN_HEADER

#include "IvPBehavior.h"

#define NO_TRACK 0
#define LR_TRACK 1
#define TRACKING 2

using namespace std;

class IvPDomain;
class BHV_ArrayTurn : public IvPBehavior {
public:
  BHV_ArrayTurn(IvPDomain);
  ~BHV_ArrayTurn() {};
  
  IvPFunction*  onRunState();
  bool setParam(std::string, std::string);

protected:
  
  int decode(string);
  //double opt_course(double,double);

  double course_fix;
  double txdot,tydot,ty,tx;
  int    state;
  bool course_fixed;
  int current_state,new_state;


};
#endif

