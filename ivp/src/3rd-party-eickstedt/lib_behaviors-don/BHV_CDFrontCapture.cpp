#include <iostream>
#include <math.h> 
#include "BHV_CDFrontCapture.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_CDFrontCapture::BHV_CDFrontCapture(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_CDFrontCapture");

  domain = subDomain(domain, "course");

  desired_heading = 0;
  peakwidth       = 0;
  basewidth       = 120;

  // The default duration at the IvPBehavior level is "-1", which
  // indicates no duration applied to the behavior by default. By
  // setting to zero here, we force the user to provide a duration
  // value otherwise it will timeout immediately.
  m_duration      = 0;

  initialized = false;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("CTD_TEMPERATURE");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_CDFrontCapture::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  if(param == "heading") {
    if(!isNumber(val))
      return(false);
    desired_heading = atof(val.c_str());
    return(true);
  }
  if(param == "peakwidth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    peakwidth = dval;
    return(true);
  }
  if(param == "basewidth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    basewidth = dval;
    return(true);
  }
  if(param == "w_azimuth") {
    if(!isNumber(val))
      return(false);
    w_azimuth = atof(val.c_str());
    return(true);
  }
  if(param == "near") {
    if(!isNumber(val))
      return(false);
    near = atof(val.c_str());
    return(true);
  }
  if(param == "far") {
    if(!isNumber(val))
      return(false);
    far = atof(val.c_str());
    return(true);
  }
  if(param == "threshold") {
    if(!isNumber(val))
      return(false);
    threshold = atof(val.c_str());
    return(true);
  }
  if(param == "points") {
    if(!isNumber(val))
      return(false);
    points = atoi(val.c_str());
    return(true);
  }

  return(false);
}

//-----------------------------------------------------------
// Procedure: produceOF
//

IvPFunction *BHV_CDFrontCapture::produceOF() 
{
  if(!domain.hasDomain("course")) {
    postEMessage("No 'heading' variable in the helm domain");
    return(0);
  }

  bool ok1,ok2,ok3;
  //get current x
  double osX = info_buffer->dQuery("NAV_X", ok1);
  //get current heading
  double osY = info_buffer->dQuery("NAV_Y", ok2);
  //get current tracking state 
  double ctdTemp = info_buffer->dQuery("CTD_TEMPERATURE", ok3);

  if(!initialized)
    {
      initialize(w_azimuth,near,far,points,threshold,desired_heading);
      initialized = true;
    }
 
  //give an error if we don't have the information we need
  if(!ok1 || !ok2 || !ok3){
    postEMessage("error,BHV_CTDFrontCapture: data not available");
    return (0);
  }

  //we have our data so set the measurement
  set_measurement(ctdTemp,osX,osY);

  //now check for a new azimuth
  desired_heading = get_azimuth();

  //log the value in the MOOS DB
  postMessage("BHV_CDFrontCapture-desired heading",desired_heading);

  //set the new desired heading
  ZAIC_PEAK zaic(domain, "course");
  zaic.setSummit(desired_heading);
  zaic.setBaseWidth(basewidth);
  zaic.setPeakWidth(peakwidth);
  zaic.setValueWrap(true);
  
  IvPFunction *ipf = zaic.extractOF();
  if(ipf)
    ipf->setPWT(priority_wt);

  return(ipf);
}

void BHV_CDFrontCapture::initialize(double w_azimuth,double near, double far, int points,double threshold,double init_heading)
{

  CapFro.Initialize(w_azimuth,near,far,points,threshold,init_heading);
}

void BHV_CDFrontCapture::set_measurement(double ctd_temp,double auv_x,double auv_y)
{
  CapFro.Set_measurement(ctd_temp,auv_x,auv_y);
}

double BHV_CDFrontCapture::get_azimuth()
{
  return(CapFro.Get_waypoint());
}


template <class T>
void DingWang_CD_CapFro<T>::Initialize(const T w_azimuth, const T near, const T far, const int points, const T threshold, const T starting_azimuth)
{
  W_azimuth=w_azimuth;
  Near_bound=near;
  Far_bound=far;
  N_points=points;
  Temperature_threshold=threshold;
  Starting_azimuth=starting_azimuth;
  sample_index=0;
  near_far=-1; // at the beginning, the AUV must be near the near bound
  original_T=-1.;
  Direction=starting_azimuth; // at the beginning, the AUV must be going to far
}

template <class T>
void DingWang_CD_CapFro<T>::Set_measurement(const T temperature, const T x, const T y)//set up measurement datum and location in x,y corrdinates
{
  sample_index=sample_index+1;
  Temperature.change_size(sample_index);
  Temperature(sample_index)=temperature;
  Current_W=x*sin(W_azimuth/180.*M_PI)+y*cos(W_azimuth/180.*M_PI);//corrdinates transform
  //cout<<"x="<<x<<"y="<<y<<"W="<<Current_W<<"W_azimuth="<<W_azimuth<<" original_T="<<original_T<<endl; 
}

template <class T>
T DingWang_CD_CapFro<T>::Get_waypoint(void)
{
  if (sample_index>=N_points)
    {
      T current_T=mean(Temperature);
      if (original_T<=0)
	{original_T=current_T;}

      else if (abs(current_T-original_T)>=Temperature_threshold && near_far==-1)
	{
	  near_far=1;
	  original_T=current_T;
	}// AUV is now between the fronts and the far bound
      else if (abs(current_T-original_T)>=Temperature_threshold && near_far==1)
	{
	  near_far=-1;
	  original_T=current_T;
	}// between the near bound and the fronts
      sample_index=0;
    }

  if (Current_W>=Far_bound)// boundary check
    {
      near_far=1;
      original_T=-1;//we need to reset the original_T if the bound is reached
    }
  else if (Current_W<=Near_bound)// boundary check
    {
      near_far=-1;
      original_T=-1;//we need to reset the original_T if the bound is reached
    }
  if (near_far<0)
    {Direction=Starting_azimuth;}// go to far
  else if (near_far>0)
    {Direction=180+Starting_azimuth+2*(W_azimuth-Starting_azimuth); }// go to near

  return Direction;
}

template <class T>
T mean(Vector<T> X)
{
  int N=X.dim();
  T sum_X=0;
  for (int i=1; i<=N; i++)
    {
      sum_X+=X(i);
    }
  return sum_X/N;
}
