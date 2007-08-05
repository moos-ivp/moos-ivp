/***********************************************************/
/*    NAME: M. Benjamin / P.Newman                         */
/*    FILE: BHV_Resolve.cpp                                */
/*    DATE: Sep 07 2005                                    */
/***********************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <map>
#include <list>
#include <sstream>
#include <iostream>
#include <math.h> 
#include "BHV_Resolve.h"
#include "AOF_Resolve.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "OF_Reflector.h"
#include "BuildUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Resolve::BHV_Resolve(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_resolve");
  this->setParam("unifbox", "course=2,  speed=2, tol=2");
  this->setParam("gridbox", "course=10, speed=4, tol=6");

  domain = subDomain(domain, "course,speed,tol");

  osX   = -1;
  osY   = -1;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("MOW_HISTORY");
  info_vars.push_back("TIME_SINCE_UPGRADE");

  resolver.Configure(150,150,5);
  time_since_upgrade = 0.0;
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Resolve::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF
static double dfTSLU=0;
IvPFunction *BHV_Resolve::produceOF() 
{
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
  bool ok1, ok2;
  osX   = info_buffer->dQuery("NAV_X", ok1);
  osY   = info_buffer->dQuery("NAV_Y", ok2);

#if 0
  static double dfLastFake = 0;
 
  string new_history;
  bool bNewHistory = false;
  double dfT = info_buffer->getCurrTime();
  if(dfT>2)
    {
      dfLastFake = info_buffer->getCurrTime();
      static list<pair<double, double> > History;

      History.push_back(pair<double, double>(osX, osY));
      while(History.size() > 10) {
	History.pop_front();
      }

      ostringstream os;
      os<<"History=["<<History.size()<<"x2]{";
      
      list<pair<double,double> > ::iterator p;
      for(p = History.begin();p!=History.end();p++)
	{
	  os<<p->first<<','<<p->second<<',';
	}
      os<<'}';
      new_history = os.str();
      dfTSLU = int(dfT)%60;
      bNewHistory = true;

      cout<<"New (Fake) History"<<os.str()<<endl;
      cout<<"New TSLU : "<< dfTSLU<<endl;
    }
#endif

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2)
    postEMessage("No ownship X/Y info in info_buffer.");
  
  bool   ok;
  string new_history = info_buffer->sQuery("MOW_HISTORY", ok);
  
  if(new_history != history) {
    ok = resolver.OnNewHistory(new_history);
    history = new_history;
  }

  if(!ok) {
    postMessage("BHV_RESOLVE_ACTIVE", 0.0);
    postWMessage("Insufficient MOW_HISTORY info in info_buffer.");
    return(0);
  }

  time_since_upgrade = 0.0;
  time_since_upgrade = info_buffer->dQuery("TIME_SINCE_UPGRADE", ok);
  
  if(!ok) {
    postMessage("BHV_RESOLVE_ACTIVE", 0.0);
    postWMessage("No TIME_SINCE_UPGRADE info in info_buffer.");
    return(0);
  }

  if(time_since_upgrade == 0) {
    postMessage("BHV_RESOLVE_ACTIVE", 0.0);
    return(0);
  }

  double relevance = getRelevance();
  if(relevance == 0) {
    postMessage("BHV_RESOLVE_ACTIVE", 0.0);
    return(0);
  }

  AOF_Resolve aof(domain, &resolver);
  aof.setParam("os_lat", osY);
  aof.setParam("os_lon", osX);

  OF_Reflector reflector(&aof, 1);
  reflector.create_uniform(unif_box, 0);
  IvPFunction *of = reflector.extractOF();

  //cout << "Min Weight: " << of->getPDMap()->getMinWT() << endl;
  //cout << "Max Weight: " << of->getPDMap()->getMaxWT() << endl;

  of->setPWT(relevance * priority_wt);

  postMessage("BHV_RESOLVE_WT", (relevance * priority_wt));
  cout << "bhv_resolve_wt:" << (relevance * priority_wt) << endl;
  postMessage("BHV_RESOLVE_ACTIVE", 1.0);
  return(of);
}

//-----------------------------------------------------------
// Procedure: getRelevance
//   Purpose: Return relevance between [0,1] by time_since_upgrade
//            through a sigmoid function:
//
//                          1
//            f(x) =   -----------
//                     1 + e^{-ax}
//                                                     

double BHV_Resolve::getRelevance()
{
  double val        = time_since_upgrade;
#if 0
  double step_point = 20.0;  // seconds
  double exp        = 3.0;
  double range_low  = 0.0;
  double range_high = 1.0;
  double e          = 2.718281;
  
  double denominator = (1.0+ pow(e,(-1.0*exp*(val-step_point))));
  double relevance   = (range_low + (range_high / denominator));
#endif

  double relevance = 0.5 * (1 + tanh((val - 20) / 10));

  cout << "bhv_resolve relevance:" << endl;

  return(relevance);
}

