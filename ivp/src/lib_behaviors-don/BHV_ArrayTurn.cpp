/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_ArrayTurn.cpp                              */
/*    DATE: July 03 2005                                   */
/***********************************************************/

#include <string>
#include <iostream>
#include <math.h> 
#include "BHV_ArrayTurn.h"
#include "MBUtils.h"
#include "AOF_ArrayTurn.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_ArrayTurn::BHV_ArrayTurn(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{

  this->setParam("descriptor", "(d)bhv_ArrayTurn");
  this->setParam("unifbox", "course=3");
  this->setParam("gridbox", "course=9");

  domain = subDomain(domain, "course");

  course_fixed = false;
  current_state = 0;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  // changed to AEL_HEADING. HS 103106
  info_vars.push_back("AEL_HEADING");
  // info_vars.push_back("NAV_HEADING");
  //info_vars.push_back("TRACK_STAT");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_ArrayTurn::setParam(string param, string val) 
{

  IvPBehavior::setParamCommon(param, val);

  return true;
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_ArrayTurn::produceOF() 
{
  bool ok1,ok2,ok3,ok4;
  //get current course
  // changed to AEL_HEADING. HS 103106
  double osCourse = info_buffer->dQuery("AEL_HEADING", ok1);
  // double osCourse = info_buffer->dQuery("NAV_HEADING", ok1);
  //get current x
  double osX = info_buffer->dQuery("NAV_X", ok2);
  //get current heading
  double osY = info_buffer->dQuery("NAV_Y", ok3);
  //get current tracking state
  //  string tState = info_buffer->sQuery(us_name,"TRACK_STAT", ok4);

  if(!ok1 || !ok2 || !ok3){
    postEMessage("error,BHV_ArrayTurn: ownship data not available");
    return (0);
  }
    
  //new_state = decode(tState);

  //if not determining L/R do nothing
  //if ((new_state == NO_TRACK)||(new_state == TRACKING))
  //  return(0);

  //if determining L/R, maneuver vehicle
  //else if (new_state == LR_TRACK){
  //  if (current_state != LR_TRACK)
  if (!course_fixed){
      course_fix = osCourse;
      course_fixed = true;
  }
  //  }

  //current_state = new_state;

  AOF_ArrayTurn aof_track(domain,course_fix,osCourse);
  
  OF_Reflector reflector(&aof_track,1);
 
  reflector.createUniform(unif_box,grid_box);
 
  IvPFunction *of = reflector.extractOF();

  of->setPWT(priority_wt);
 
  return(of);
}



