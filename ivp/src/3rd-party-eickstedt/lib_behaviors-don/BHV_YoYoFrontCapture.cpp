#include <iostream>
#include <math.h> 
#include "BHV_YoYoFrontCapture.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

BHV_YoYoFrontCapture::BHV_YoYoFrontCapture(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_YoYoFrontCapture");

  domain = subDomain(domain, "course");

  desired_heading = 0;
  peakwidth       = 0;
  basewidth       = 4;

  // The default duration at the IvPBehavior level is "-1", which
  // indicates no duration applied to the behavior by default. By
  // setting to zero here, we force the user to provide a duration
  // value otherwise it will timeout immediately.
  m_duration      = 0;

  initialized = false;

  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("CTD_TEMPERATURE");
  info_vars.push_back("NAV_DEPTH");
  info_vars.push_back("YoYo_Direc");
}

//-----------------------------------------------------------
// Procedure: setParam

bool BHV_YoYoFrontCapture::setParam(string param, string val) 
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
  if(param == "segments") {
    if(!isNumber(val))
      return(false);
    segments = atoi(val.c_str());
    return(true);
  }
  if(param == "segment_index") {
    if(!isNumber(val))
      return(false);
    segment_index = atoi(val.c_str());
    return(true);
  }
  if(param == "upper") {
    if(!isNumber(val))
      return(false);
    upper = atof(val.c_str());
    return(true);
  }
  if(param == "lower") {
    if(!isNumber(val))
      return(false);
    lower = atof(val.c_str());
    return(true);
  }
  
  return(false);
}

//-----------------------------------------------------------
// Procedure: produceOF
//

IvPFunction *BHV_YoYoFrontCapture::produceOF() 
{
  if(!domain.hasDomain("course")) {
    postEMessage("No 'heading' variable in the helm domain");
    return(0);
  }

  if(!initialized)
    {
      initialize(w_azimuth,near,far,upper,lower,segments,segment_index,threshold,desired_heading);
      initialized = true;
    }

  bool ok1,ok2,ok3,ok4,ok5;
  //get current x
  double osX = info_buffer->dQuery("NAV_X", ok1);
  //get current heading
  double osY = info_buffer->dQuery("NAV_Y", ok2);
  //get current tracking state 
  double ctdTemp = info_buffer->dQuery("CTD_TEMPERATURE", ok3);
  //get current depth 
  double osDepth = info_buffer->dQuery("NAV_DEPTH", ok4);
  //get current vertical yoyo direction
  int yoyo_direc = (int) info_buffer->dQuery("YoYo_Direc", ok5);

  //give an error if we don't have the information we need
  if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5){
    postEMessage("error,BHV_YoYOFrontCapture: data not available");
    return (0);
  }

  //we have our data so set the measurement
  set_measurement(ctdTemp,osX,osY,osDepth,yoyo_direc);

  //now check for a new azimuth
  desired_heading = get_azimuth();

  //log the value in the MOOS DB
  postMessage("BHV_YoYoFrontCapture-desired heading",desired_heading);

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

void BHV_YoYoFrontCapture::initialize(double w_azimuth,double near, double far, double upper,double lower,int segments,int segment_index,double threshold,double init_heading)
{

  CapFro.Initialize(w_azimuth,near,far,upper,lower,segments,segment_index,threshold,init_heading);
}

void BHV_YoYoFrontCapture::set_measurement(double ctd_temp,double auv_x,double auv_y,double auv_depth,int updown)
{
  CapFro.Set_measurement(ctd_temp,auv_x,auv_y,auv_depth,updown);
}

double BHV_YoYoFrontCapture::get_azimuth()
{
  return(CapFro.Get_waypoint());
}

template <class T>
void DingWang_YoYo_CapFro<T>::Initialize(const T w_azimuth, const T near, const T far, const T upper, const T lower, const int segments, const int segment_index, const T threshold, const T starting_azimuth)
{
  W_azimuth=w_azimuth;
  Near_bound=near;
  Far_bound=far;
  Upper_bound=upper;
  Lower_bound=lower;
  Temperature_threshold=threshold;
  Starting_azimuth=starting_azimuth;
  near_far=-1; // at the beginning, suppose the AUV is near the near bound 
  original_T.newsize(segments);
  original_going_up_down=-1;// at the beginning, the direction is going down
  Direction=starting_azimuth; // at the beginning, the AUV must be going to far
  Temperature.newsize(segments);
  Mean_Tp.newsize(segments);
  Temperature_N.newsize(segments);
  Segments=segments;
  Segment_index=segment_index;
}

template <class T>
void DingWang_YoYo_CapFro<T>::Set_measurement(const T temperature, const T x, const T y, const T d, const T going_up_down)
{//cout<<"temperature="<<temperature<<" x="<<x<<" y="<<y<<" d="<<d<<" going_up_down="<<going_up_down<<endl;
  int index=(int)floor((d-Upper_bound)/((Lower_bound-Upper_bound)/Segments))+1;
  if (index>=1 && index<=Segments)
    {
      Temperature(index)+=temperature;//add all measurements for each depth segment
      Temperature_N(index)+=1;//count the number of measurements in each depth segment
    }
  Current_W=x*sin(W_azimuth/180.*M_PI)+y*cos(W_azimuth/180.*M_PI);//corrdinates transform;
  Current_going_up_down=going_up_down;
}

template <class T>
T DingWang_YoYo_CapFro<T>::Get_waypoint(void)
{
  if (original_going_up_down!=Current_going_up_down)//only when AUV's vertical direction gets changed
    {
      original_going_up_down=Current_going_up_down;	
      for (int i=1; i<=Segments; i++)
	{
	  if (Temperature_N(i)>=1)
	    {
	      Mean_Tp(i)=Temperature(i)/Temperature_N(i);
	      Temperature(i)=0;
	      Temperature_N(i)=0;
	    }
	}

      if (original_T(Segment_index)<=0)
	{
	  original_T=Mean_Tp;
	}

      else if (abs(Mean_Tp(Segment_index)-original_T(Segment_index))>=Temperature_threshold && near_far==-1)
	{
	  near_far=1;
	  original_T=Mean_Tp;
	}// is out of the fronts
      else if (abs(Mean_Tp(Segment_index)-original_T(Segment_index))>=Temperature_threshold && near_far==1)
	{
	  near_far=-1;
	  original_T=Mean_Tp;
	}// between the coast line and fronts
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
  //cout<<"Mean_Tp="<<Mean_Tp<<endl;
  //cout<<original_T<<endl;
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

