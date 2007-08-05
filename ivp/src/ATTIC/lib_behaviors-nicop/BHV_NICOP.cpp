#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include "BHV_NICOP.h"
#include "AOF_NICOP.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_NICOP::BHV_NICOP(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_nicop");
  this->setParam("unifbox", "course=10, speed=4, tol=4");
  this->setParam("gridbox", "course=20, speed=8, tol=8");
  
  domain = subDomain(domain, "course,speed,tol");

  completed = false;
  
  osX   = -1;
  osY   = -1;
  osSPD = -1;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
}

//-----------------------------------------------------------
// Procedure: setParam
//     Notes: We expect the "waypoint" entries will be of the form
//            "xposition,yposition".
//            The "radius" parameter indicates what it means to have
//            arrived at the waypoint.

bool BHV_NICOP::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if(param == "param") {
    double param = atof(val.c_str());
    return(true);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_NICOP::produceOF() 
{
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
  bool ok1, ok2;
  osX = info_buffer->dQuery("NAV_X", ok1);
  osY = info_buffer->dQuery("NAV_Y", ok2);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }

  IvPFunction *of = 0;

  AOF_NICOP aof_nicop(domain, osX, osY);

  OF_Reflector reflector(&aof_nicop, 1);
  reflector.create_uniform(unif_box, grid_box);
  of = reflector.extractOF();
  of->setPWT(priority_wt);

  return(of);
}

