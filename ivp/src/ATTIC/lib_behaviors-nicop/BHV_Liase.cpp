/***********************************************************/
/*    NAME: Michael Benjamin                               */
/*    FILE: BHV_Liase.cpp                                  */
/*    DATE: Sept 2005                                      */
/***********************************************************/

#ifdef _WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4503)
#endif
#include <iostream>
#include <math.h> 
#include "BHV_Liase.h"
#include "AOF_WPT2D.h"
#include "OF_Reflector.h"
#include "MBUtils.h"
#include "AngleUtils.h"
#include "GeomUtils.h"
#include "BuildUtils.h"

using namespace std;

bool MOOSVectorFromString(const string& sStr, vector<double>& dfValVec,
			  int & nRows, int & nCols);
bool MOOSValFromString(vector<double> &dfValVec, int &nRows, int &nCols,
		       const string& sStr, const string& sToken);

//-----------------------------------------------------------
// Procedure: Constructor

BHV_Liase::BHV_Liase(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_waypoint");
  this->setParam("unifbox", "course=3, speed=2");
  this->setParam("gridbox", "course=9, speed=4");

  domain = subDomain(domain, "course,speed,tol");

  cruise_speed = 0;  // Meters/second

  osX   = -1;
  osY   = -1;
  ptX   = -1;
  ptY   = -1;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("MOW_TARGET");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_Liase::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if((param == "them") || (param == "contact")) {
    if(!param_lock) {
      them_name = toupper(val);
      info_vars.push_back(them_name+"_NAV_X");
      info_vars.push_back(them_name+"_NAV_Y");
      info_vars.push_back(them_name+"_NAV_SPEED");
      info_vars.push_back(them_name+"_NAV_HEADING");
    }
    return(true);
  }  
  else if(param == "speed") {
    cruise_speed = atof(val.c_str());
    return(cruise_speed > 0);
  }
  return(false);
}


//-----------------------------------------------------------
// Procedure: produceOF

IvPFunction *BHV_Liase::produceOF() 
{
  if(!unif_box || !grid_box) {
    postEMessage("Null UnifBox or GridBox.");
    return(0);
  }
  
  // Set osX, osY, ptX, ptY;
  bool valid_point = setNextLiasePoint();

  if(!valid_point)
    return(0);

  AOF_WPT2D aof(domain);
  aof.setParam("oslat", osY);
  aof.setParam("oslon", osX);
  aof.setParam("ptlat", ptY);
  aof.setParam("ptlon", ptX);
  aof.setParam("desired_speed", cruise_speed);
  aof.initialize();

  OF_Reflector reflector(&aof, 1);

  reflector.create_uniform(unif_box, grid_box);
  IvPFunction *of = reflector.extractOF();

  of->setPWT(priority_wt);

  if(!silent) {
    IvPBox mpt = of->getPDMap()->getGrid()->getMaxPt();
    cout << "BHV_Liase::produceOF():" << endl;
    cout << "maxpt:" << endl;
    mpt.print();
  }

  return(of);
}

//-----------------------------------------------------------
// Procedure: setNextLiasPoint

bool BHV_Liase::setNextLiasePoint()
{
  // Need to know the name of ownship to query position
  if(us_name == "") {
    postEMessage("ownship name not known");
    return(false);
  }
  // Need to know the name of them to query position
  if(them_name == "") {
    postEMessage("them name not known");
    return(false);
  }
  
  bool ok1, ok2;
  osX   = info_buffer->dQuery("NAV_X", ok1);
  osY   = info_buffer->dQuery("NAV_Y", ok2);

  // Must get ownship position from InfoBuffer
  if(!ok1 || !ok2) {
    postEMessage("No ownship X/Y info in info_buffer.");
    return(false);
  }

  bool ok;
  string info = info_buffer->sQuery(them_name+"_MOW_TARGET", ok);

  // Channel=0,Target=[2x1]{45,3},Scores = [30x3]{45,3,0.1,...}
  // Channel is the "ID" of the tracked vehicle, Goal is the best 
  // (ML) location of the tracked vehcile and Scores is nx3 
  // matrix of possible alternative solutions and their weights.
  // (sorted)
  
  std::vector<double> gvector;
  int nRows, nCols;
  if(MOOSValFromString(gvector, nRows, nCols, info, "Target")) {
    if(nRows!=2) {
      postEMessage("Bad MOW_TARGET data string: " + info);
      return(false);
    }
    
    double ptX = gvector[0];
    double ptY = gvector[1];
  }

#if 0
  //to get all possible targets (the first will be the same
  //as "Target" above..
  
  if(MOOSValFromString(gvector, nRows, nCols, s, "Scores")) {
    for(int i = 0; i<nRows; i++) {
      int n = 3*i-3;
      double dfX = gvector[n];
      double dfY = gvector[n+1];
      double dfW = gvector[n+2];
    }
  }
#endif

return(true);
}

//-----------------------------------------------------------
// Procedure: MOOSVectorFromString

bool MOOSVectorFromString(const string& sStr, vector<double>& dfValVec,
			  int & nRows, int & nCols)
{
  int nPos = nPos = sStr.find('[',nPos);
  if(nPos==string::npos)
    return false;
  
  nRows = atoi( sStr.data()+nPos+1);
  
  //if we have [456] then implicitlyt we mean [456x1]
  int nXPos = sStr.find('x',nPos);
  nCols = 1;
  if(nXPos!=string::npos) {
    nCols = atoi( sStr.data()+nXPos+1);
    nPos = nXPos;
  }
  
  nPos = sStr.find('{',nPos);
  if(nPos==string::npos)
    return(false);
  if(nCols==0 ||nRows==0)
    return(false);
    
  dfValVec.reserve(nRows*nCols);
  //Mat.ReSize(nRows,nCols);
    
  for(int i = 1; i<=nRows;i++) {
    for(int j = 1; j<=nCols;j++) {
      double dfVal = atof(sStr.data()+nPos+1);
      //Mat(i,j) = dfVal;
      dfValVec.push_back(dfVal);
      nPos = sStr.find(',',nPos+1);
    }
  }
  return(true);
}

//-----------------------------------------------------------
// Procedure: MOOSValFromString

bool MOOSValFromString(vector<double> &dfValVec, int &nRows, int &nCols,
		       const string& sStr, const string& sToken)
{
  int nPos = sStr.find(sToken+'=');
  if(nPos==string::npos)
    return false;
  return MOOSVectorFromString(sStr.substr(nPos),dfValVec,nRows,nCols);
}



