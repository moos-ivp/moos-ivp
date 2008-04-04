/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_HArrayTurn.cpp                             */
/*    DATE: July 03 2005                                   */
/*    MODIFIED: H. Schmidt, Feb.20, 2007                   */
/***********************************************************/

#include <string>
#include <iostream>
#include <math.h> 
#include "BHV_HArrayTurn.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "ZAIC_PEAK.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_HArrayTurn::BHV_HArrayTurn(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{

  this->setParam("descriptor", "(d)bhv_1BTrack");
  this->setParam("unifbox", "course=3");
  this->setParam("gridbox", "course=9");

  m_domain = subDomain(m_domain, "course");

  course_fixed = false;
  current_state = 0;
  curr_turn = 0;
  next_turn = 1;
  first_time = true;
  course_new = 0;
  course_lr_init = 0;
  turn_angle = 120.0;

  addInfoVars("NAV_X");
  addInfoVars("NAV_Y");
  // changed to AEL_HEADING. HS 103106
  addInfoVars("AEL_HEADING");
  // addInfoVars("NAV_HEADING");
  addInfoVars("TRACK_STAT");
}

//-----------------------------------------------------------
// Procedure: onIdleState
//      Note: This function overrides the onIdleState() virtual
//            function defined for the IvPBehavior superclass
//            This function will be executed by the helm each
//            time the behavior FAILS to meet its run conditions.

void BHV_HArrayTurn::onIdleState()
{
  // Do your thing here
  first_time=true;
}


//-----------------------------------------------------------
// Procedure: setParam

bool BHV_HArrayTurn::setParam(string param, string val) 
{

  IvPBehavior::setParamCommon(param, val);

  val = stripBlankEnds(val);

  if(param == "turn_angle") 
    {
      double dval = atof(val.c_str());
      if((dval <= 0) || (!isNumber(val)))
	return(false);
      else
	turn_angle = dval;
    } 

  return true;
}


//-----------------------------------------------------------
// Procedure: onRunState

IvPFunction *BHV_HArrayTurn::onRunState() 
{
  bool ok1,ok2,ok3,ok4;
  //get current course
  // changed to AEL_HEADING. HS 103106
  double osCourse = getBufferDoubleVal("AEL_HEADING", ok1);
  // double osCourse = getBufferDoubleVal("NAV_HEADING", ok1);
  //get current x
  double osX = getBufferDoubleVal("NAV_X", ok2);
  //get current heading
  double osY = getBufferDoubleVal("NAV_Y", ok3);
  //get current tracking state
  string tState = getBufferStringVal("TRACK_STAT", ok4);
  
  if(!ok1 || !ok2 || !ok3 || !ok4)
    {
      postEMessage("error,BHV_HArrayTurn: ownship data not available");
      return (0);
    }
  
  new_state = decode(tState);

  if ( first_time )
    {
      course_lr_init = osCourse;
      curr_turn=0;
      //    double b_ang = (90.0-osCourse)*M_PI/180.0;
      double b_ang = headingToRadians(osCourse);
      double x_b=cos(b_ang);
      double y_b=sin(b_ang);
      //    double h_ang=(90.0-heading)*M_PI/180.0;
      double h_ang = headingToRadians(heading);
      double x_h=cos(h_ang);
      double y_h=sin(h_ang);
      double c_ang = headingToRadians(course_lr_init);
      double x_c=cos(c_ang);
      double y_c=sin(c_ang);
      double v_prod= x_c*y_h - x_h*y_c ;
      if (v_prod > 0)
	// Turn in direction of target heading
	next_turn = -1;
      else
	next_turn = 1; 
      curr_turn=next_turn;
      course_new=angle360(course_lr_init + curr_turn*turn_angle);
      first_time = false;
      return(0);
    }
  else
    {
      curr_turn=next_turn;
      course_new=angle360(course_lr_init + curr_turn*turn_angle);
      
    }
  
  if ( fabs( angle180(course_new - osCourse) ) < 5.0)
    {
      // turn completed, try to turn the other way next time
      //	  course_lr_init = os_course;
      //      next_turn=-next_turn;
      first_time = true;  
    }

  //  postMessage("LR_STATE",new_state);
  postMessage("LR_INIT_HEADING",course_lr_init);
  postMessage("LR_TURN_HEADING",course_new);
  postMessage("NEXT_TURN",next_turn);

  current_state = new_state;



#if 1 // Version with ZAIC having ONE peak
  ZAIC_PEAK crs_zaic(m_domain,"course");
  crs_zaic.setSummit(course_new);
  crs_zaic.setValueWrap(true);
  crs_zaic.setPeakWidth(60.0);
  crs_zaic.setBaseWidth(120.0);
  crs_zaic.setSummitDelta(50.0); // mikerb added
#endif

#if 0 // Version with ZAIC having TWO peaks
  double other_course = angle360(course_new + 180);

  ZAIC_PEAK crs_zaic(m_domain,"course");
  crs_zaic.setSummit(course_new);
  crs_zaic.setValueWrap(true);
  crs_zaic.setPeakWidth(120.0);
  crs_zaic.setSummitDelta(100.0);
  crs_zaic.setMinMaxUtil(0,100);

  int new_index = crs_zaic.addComponent();
  crs_zaic.setSummit(other_course, new_index);
  crs_zaic.setPeakWidth(60.0, new_index);
  crs_zaic.setSummitDelta(50.0, new_index);
  crs_zaic.setMinMaxUtil(0,50,new_index);
#endif

  IvPFunction *of = crs_zaic.extractOF();

  of->setPWT(m_priority_wt);
 
  return(of);
}

int BHV_HArrayTurn::decode(string status)
{
  vector<string> svector;
  vector<string> svector2;


  // Parse the waypoint status string for "us"  
  svector = parseString(status, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    svector2 = parseString(svector[i], '=');
    if(svector2.size() != 2) {
      postEMessage("error,BHV_HArrayTurn: Invalid waypoint string");
      return(0);
    }

    string left  = stripBlankEnds(svector2[0]);
    string right = stripBlankEnds(svector2[1]);
    if(left == "state")    state = atoi(right.c_str());
    if(left == "x")        tx     = atof(right.c_str());
    if(left == "y")        ty     = atof(right.c_str());
    if(left == "heading")     heading  = atof(right.c_str());
    if(left == "speed")       speed  = atof(right.c_str());
    if(left == "time")        time  = atof(right.c_str());
}
    
  return(state);
}


