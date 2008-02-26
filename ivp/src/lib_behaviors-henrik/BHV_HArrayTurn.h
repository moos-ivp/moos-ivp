/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_HArrayTurn.h                                  */
/***********************************************************/
 
#ifndef BHV_HARRAYTURN_HEADER
#define BHV_HARRAYTURN_HEADER

#include "IvPBehavior.h"

#define NO_TRACK 0
#define LR_TRACK 1
#define TRACKING 2

using namespace std;

class IvPDomain;
class BHV_HArrayTurn : public IvPBehavior {
public:
  BHV_HArrayTurn(IvPDomain);
  ~BHV_HArrayTurn() {};
  
  IvPFunction*  onRunState();
  void          onIdleState();
  bool          setParam(std::string, std::string);

protected:
  
  int decode(string);
  //double opt_course(double,double);

  double course_fix,course_new,course_lr_init,turn_angle;
  double heading,speed,ty,tx,time;
  int    state;
  bool course_fixed, first_time;
  int current_state,new_state,next_turn,curr_turn;


};
#endif

