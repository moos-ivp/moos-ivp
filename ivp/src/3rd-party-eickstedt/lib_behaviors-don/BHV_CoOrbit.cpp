/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: BHV_CoOrbit.cpp                                */
/*    DATE:                                                */
/***********************************************************/
#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <string>
#include <iostream>
#include <math.h>
#include "BHV_CoOrbit.h"
#include "IvPDomain.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_CoOrbit::BHV_CoOrbit(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "CoOrbit");

  m_domain = subDomain(m_domain, "speed");

  politeness = 50.0;   
  wait_speed = 0.0;
  us_laps = 0;
  them_laps = 0;
  last_us_index = 0;
  last_them_index = 0;

  addInfoVars("VEHICLE_WPT_STAT");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition,tag" where the tag is an optional
//            descriptor of the waypoint.
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_CoOrbit::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if((param == "them") || (param == "contact")) {
    them_name = toupper(val);
    addInfoVars(them_name+"_NAV_X");
    addInfoVars(them_name+"_NAV_Y");
    addInfoVars(them_name+"_NAV_SPEED");
    addInfoVars(them_name+"_NAV_HEADING");
    return(true);
  }  
  else if(param == "politeness") {
    politeness = atof(val.c_str());
    return(true);
  }
  else if(param == "wait_speed") {
    wait_speed = atof(val.c_str());
    return(true);
  }
  else if(param == "cruise_speed") {
    cruise_speed = atof(val.c_str());
    return(true);
  }
  else if(param == "lag") {
    lag = (int) atof(val.c_str());
    return(true);
  }
  else if(param == "legs") {
    legs = (int) atof(val.c_str());
    return(true);
  }
  return(false);
}



//-----------------------------------------------------------
// Procedure: produceOF
//
//          ^     should-speed    
//      100-|           .
//          |          / \
//          | piece-1 /   \ piece-2
//          |        /     \
// Utility  |       /       \
//          |      /         \
//          |     /           \
//          |    /             \
//          |___/_______________\__________________
//                    Speed
//
//

IvPFunction *BHV_CoOrbit::produceOF() 
{
  // Need to know name of us and them to query own waypt progress
  if(them_name == "") {
    postEMessage("contact ID not set");
    return(0);
  }
  
  bool ok1, ok2;
  string us_waypt_stat = m_info_buffer->sQuery("VEHICLE_WPT_STAT", ok1);
  string them_waypt_stat = m_info_buffer->sQuery(them_name+"_VEHICLE_WPT_STAT", ok2);

#if 0
  cout << "+++++BHV_CoOrbit::produceOF() " << us_name << endl;
  cout << "  us_waypt_stat:" << us_waypt_stat << endl;
  cout << "  them_waypt_stat:" << them_waypt_stat << endl;
#endif

  // Must get waypoint status for both vehicles from WorldModel
  if(!ok1) {
    postEMessage("No ownship waypoint progress in WorldModel");
    return(0);
  }
  if(!ok2) {
    postEMessage("No contact waypoint progress in WorldModel");
    return(0);
  }
 
  int    us_index, them_index;
  double us_dist,  them_dist;
  double us_eta,   them_eta;

  ok1 = parseStatString(us_waypt_stat, us_index, us_dist, us_eta);
  ok2 = parseStatString(them_waypt_stat, them_index, them_dist, them_eta);

  if(!ok1 || !ok2) {
    //kludge to defeat kvsim bug-take out when code trees merged
    us_index = 0;
    them_index = 0;
    us_dist = 100;
    them_dist = 100;
    us_eta = 33;
    them_eta = 33;
  }
  //return(0);

  double peak_utility = 100;
  double raw_should_speed;

  int    spdIndex  = m_domain.getIndex("speed");
  double spdBase   = m_domain.getVarLow(spdIndex);
  double spdHigh   = m_domain.getVarHigh(spdIndex);
  double spdDelta  = m_domain.getVarDelta(spdIndex);
  int    spdPoints = m_domain.getVarPoints(spdIndex);

  //keep track of laps
  if (them_index == 0)
    if (last_them_index > 0)
      them_laps +=1;

  last_them_index = them_index;

  if (us_index == 0)
    if (last_us_index > 0)
      us_laps +=1;

  last_us_index = us_index;
 

  //figure out how many legs we're ahead or behind
  int diff = (us_index-them_index);
  
  if ((diff > 0) && (them_laps > us_laps))
    diff = us_index-them_index-legs;

   if ((diff < 0) && (us_laps > them_laps))
    diff = them_index-us_index;
  
  ahead = diff - lag;

#if 0
  cout << us_name << " ahead = " << ahead << " diff = " << diff << endl;
#endif

  //calculate should speed
  if (ahead > 0){
    //we're ahead so slow down
     if((wait_speed >= 0) && (wait_speed <= spdHigh))
      raw_should_speed = wait_speed;
    else
      raw_should_speed = 0;
  }

  if (ahead == 0)
    //we're on the right leg at least so get to next wp at same time
    //using our cruising speed
    raw_should_speed = cruise_speed + fabs(((us_dist/them_eta)-(them_dist/us_eta)));
  
  if (ahead < 0)
    //we're behind schedule so open it up
    raw_should_speed = spdHigh;

  double should_speed = raw_should_speed;
  
  if(should_speed > spdHigh)
    should_speed = spdHigh;
  
#if 0
  cout << us_name << "should speed = " << should_speed << endl;
#endif

  // Convert the "should" speed into an index in the IvPDomain
  double double_index = (should_speed - spdBase) / spdDelta;
  int    domain_index = (int)(floor(double_index + 0.5))+1;


  // Build the one or two pieces that capture what we want
  double slope = peak_utility / domain_index;

  IvPBox universe = IvPBox(1,1);
  IvPBox *piece0 = new IvPBox(1,1);
  IvPBox *piece1 = 0;

  universe.setPTS(0, 0, spdPoints-1);
  piece0->setPTS(0, 0, domain_index);
  piece0->wt(0) = slope;
  piece0->wt(1) = 0.0;

  if(domain_index < spdPoints) {
    piece1 = new IvPBox(1,1);
    piece1->setPTS(0, domain_index+1, spdPoints);
    piece1->wt(0) = -slope;
    piece1->wt(1) = 2.0 * peak_utility;
  }

  // Build a PDMap out of the pieces
  PDMap *pdmap;
  if(piece1) {
    pdmap = new PDMap(2, m_domain, 1);
    pdmap->bx(1) = piece1;
  }
  else
    pdmap = new PDMap(1, m_domain, 1);
  pdmap->bx(0) = piece0;

  // Build an objective function from the PDMap
  IvPFunction *of = new IvPFunction(pdmap);

  // Calc and Set the Priority of the objective function
  double weight = calcPriority(us_eta, them_eta);
  of->setPWT(weight);

  return(of);
}

//-----------------------------------------------------------
// Procedure: calcPriority
//     Notes: Depending on the situation, (a) how much the discrepency
//            is, and (b) the scale of the discrepency, we want to 
//            adjust the priority weight.
//     Notes: By default the priority weight can fluctuation between
//            its set value and 1.5 times that value for "dire" out
//            of sync situations. The value of pwt_max can change this 
//            upper bound.


double BHV_CoOrbit::calcPriority(double us_eta, double them_eta)
{
  //if we're significantly ahead or behind schedule
  //increase our priority to give our speed choice more weight
  if ((ahead < 0) || (ahead > 0))
    return (1.5 * m_priority_wt);

  //otherwise we're on the right leg so stay cool
  return(m_priority_wt);
}


//-----------------------------------------------------------
// Procedure: parseStatString

bool BHV_CoOrbit::parseStatString(string str, int& index, 
				     double& dist, double& eta)
{
  index = -1; 
  dist  = -1;
  eta   = -1;

  vector<string> svector;
  vector<string> svector2;

  // Parse the waypoint status string for "us"  
  svector = parseString(str, ',');
  for(int i=0; i<svector.size(); i++) {
    svector2 = parseString(svector[i], '=');
    if(svector2.size() != 2) {
      postEMessage("Invalid waypoint string");
      return(0);
    }

    string left  = stripBlankEnds(svector2[0]);
    string right = stripBlankEnds(svector2[1]);
    if(left == "index") index = atoi(right.c_str());
    if(left == "dist")  dist  = atof(right.c_str());
    if(left == "eta")   eta   = atof(right.c_str());
}
  
  if((index==-1) || (dist==-1) || (eta==-1)) {
    postEMessage("Incomplete waypoint string");
    return(0);
  }
  return(true);
}
  
