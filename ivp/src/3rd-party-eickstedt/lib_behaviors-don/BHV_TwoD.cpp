/******************************************************************************
 *                                                                            *
 *  Module: BHV_TwoD.cpp                                                      *
 *  Author: Don Eickstedt - MIT                                               *
 *  Date:   24 August, 2006                                                   *
 *                                                                            *
 *  Description: BHV_TwoD is a behavior that attempts to calculate the ocean  *
 *  thermal gradient from vehicle CTD data and then attempts to follow the    *
 *  gradient while continuing to sample it.                                   *
 *                                                                            *
 ******************************************************************************/

#include <iostream>
#include <math.h> 
#include <stdio.h>
#include "BHV_TwoD.h"
#include "BuildUtils.h"
#include "ZAIC_PEAK.h"
#include "MBUtils.h"

using namespace std;

/******************************************************************************
 *                                                                            *
 *  Method: BHV_TwoD(IvPDomain gdomain) : IvPBehavior(gdomain)                *
 *                                                                            *
 *  Description: Constructor for the BHV_TwoD behavior class.                 *
 *                                                                            * 
 ******************************************************************************/

BHV_TwoD::BHV_TwoD(IvPDomain gdomain) : 
  IvPBehavior(gdomain)
{
  this->setParam("descriptor", "(d)bhv_TwoD");

  domain = subDomain(domain, "course");

  desired_heading = 0;
  peakwidth       = 1;
  basewidth       = 25;

  // The default duration at the IvPBehavior level is "-1", which
  // indicates no duration applied to the behavior by default. By
  // setting to zero here, we force the user to provide a duration
  // value otherwise it will timeout immediately.
  m_duration      = -1;

  //behavior not initialized yet
  initialized = false;

  //subscribe to the required information from the MOOS DB
  info_vars.push_back("NAV_X");
  info_vars.push_back("NAV_Y");
  info_vars.push_back("CTD_SOUND_VELOCITY");
  info_vars.push_back("ZIGZAG_GO");

  postMessage("zigzagging","half");
}

/******************************************************************************
 *                                                                            *
 *  Method:setParam(string,string)                                            *
 *                                                                            *
 *  Description: The setParam method reads the initial behaior parameters     *
 *  from the behavior file and approriately sets the internal variables.      *
 *                                                                            *
 ******************************************************************************/

bool BHV_TwoD::setParam(string param, string val) 
{
  if(IvPBehavior::setParamCommon(param, val))
    return(true);

  //the mean initial heading for sampling
  if(param == "heading") {
    if(!isNumber(val))
      return(false);
    if(!param_lock)
      {
      desired_heading = atof(val.c_str());
      initialize(desired_heading);
      }
    return(true);
  }

  //the zigzag sampling pattern period
  if(param == "period") {
    if(!isNumber(val))
      return(false);
    if(!param_lock)
      {
      period = atof(val.c_str());
      }
    return(true);
  }

  //the zigzag sampling pattern amplitude
   if(param == "amplitude") {
    if(!isNumber(val))
      return(false);
    if(!param_lock)
      {
      amplitude = atof(val.c_str());
      }
    return(true);
  }

   //the length of the zigzag sampling pattern
   if(param == "length") {
    if(!isNumber(val))
      return(false);
    if(!param_lock)
      {
      length = atof(val.c_str());
      }
    return(true);
  }

   //the peak width of the objective function
  if(param == "peakwidth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    if(!param_lock)
      peakwidth = dval;
    return(true);
  }

  //the base width of the objective function
  if(param == "basewidth") {
    double dval = atof(val.c_str());
    if((dval < 0) || (!isNumber(val)))
      return(false);
    if(!param_lock)
      basewidth = dval;
    return(true);
  }

  return(false);
}

/******************************************************************************
 *                                                                            *
 *  Method: produceOF()                                                       *
 *                                                                            *
 *  Description: The produceOF method creates and returns the objective       *
 *  function for the BHV_TwoD behavior                                        *
 *                                                                            *
 ******************************************************************************/

IvPFunction *BHV_TwoD::produceOF() 
{
  if(!domain.hasDomain("course")) {
    postEMessage("No 'heading' variable in the helm domain");
    return(0);
  }

  //figure out where we are and what the sound speed is
  bool ok1,ok2,ok3,ok4;
  //get current x
  double osX = info_buffer->dQuery("NAV_X", ok1);
  //get current heading
  double osY = info_buffer->dQuery("NAV_Y", ok2);
  //get current tracking state
  double ctdSpeed = info_buffer->dQuery("CTD_SOUND_VELOCITY", ok3);
  //check to see if the zigzag is done
  string done = info_buffer->sQuery("ZIGZAG_GO", ok4);

  string falsestr = "FALSE";

  postMessage("doneequal",done.c_str());

  //give an error if we don't have the information we need
  if(!ok1 || !ok2 || !ok3 || !ok4){
    postEMessage("error,BHV_TwoD: data not available");
    return (0);
  }
  
  //starting the first sampling segment
  //the "ZIGZAG_STATUS" variable is used by a Waypoint behavior
  if(!initialized)
    {
      char buff[300];
      sprintf(buff,"points=zigzag:%.5f,%.5f,%.5f,%.5f,%.5f,%.5f",osX,osY,desired_heading,length,period,amplitude);
      postMessage("ZIGZAG_STATUS",buff);
      initialized = true;
    }

  //incorporate the latest CTD measurement
  set_measurement(ctdSpeed,osX,osY);
  
  //now check for a new azimuth
   if(done == falsestr) //zigzag maneuver is done
    {       
      postMessage("inloop","herenow");
      //get the new heading
      desired_heading = get_azimuth();

      //post the new azimuth for the zigzag
      char buff2[300];
      sprintf(buff2,"points=zigzag:%.5f,%.5f,%.5f,%.5f,%.5f,%.5f",osX,osY,desired_heading,length,period,amplitude);
      postMessage("ZIGZAG_STATUS",buff2);

      //start the next zigzag
      postMessage("ZIGZAG_GO","TRUE");
    }

  ZAIC_PEAK zaic(domain, "course");
  zaic.setSummit(desired_heading);
  zaic.setBaseWidth(basewidth);
  zaic.setPeakWidth(peakwidth);
  zaic.setValueWrap(true);
  
  IvPFunction *ipf = zaic.extractOF();
  if(ipf)
    ipf->setPWT(priority_wt);

  return(0);
}

/******************************************************************************
 *                                                                            *
 *  Method: initialize(double init_azimuth)                                   *
 *                                                                            *
 *  Description: The initialize method initializes the algorithm for computing*
 *  the thermal gradient.                                                     *
 *                                                                            *
 ******************************************************************************/
void BHV_TwoD::initialize(double init_azimuth)
{
  TwoD_Grad.Initialize(init_azimuth);
}


/******************************************************************************
 *                                                                            *
 *  Method: set_measurement(double ctd_speed,double auv_x,double auv_y)       *
 *                                                                            *
 *  Description: The set_measurement method is a simple wrapper method that   *
 *  calls the Set_measurement function for the gradient algorithm.            *
 *                                                                            *
 ******************************************************************************/
void BHV_TwoD::set_measurement(double ctd_speed,double auv_x,double auv_y)
{
  //don't add redundant measurments
  if ((auv_x != last_x)&&(auv_y != last_y)&&(last_ss != ctd_speed))
    {
      TwoD_Grad.Set_measurement(ctd_speed,auv_x,auv_y);
      last_x = auv_x;
      last_y = auv_y;
      last_ss = ctd_speed;
    }

}

/******************************************************************************
 *                                                                            *
 *  Method: get_azimuth()                                                     *
 *                                                                            *
 *  Description: The get_azimuth method returns the estimated direction of    *
 *  the thermal gradient                                                      *
 *                                                                            *
 ******************************************************************************/

double BHV_TwoD::get_azimuth()
{
  return(TwoD_Grad.Get_gradient_azimuth());
}

/******************************************************************************
 *                                                                            *
 *  Method: DingWang_2D_Gradient<T>::Initialize(const T init_azimuth)         *
 *                                                                            *
 *  Description: Initializes the DingWang_2D_Gradient object.                 *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/

template <class T>
void DingWang_2D_Gradient<T>::Initialize(const T init_azimuth)
{
  Azimuth=init_azimuth;
  sample_index=0;
}

/**********************************************************************************************
 *                                                                                            *
 *  Method:DingWang_2D_Gradient<T>::Set_measurement(const T temperature, const T x, const T y)* 
 *                                                                                            * 
 *  Description: sets the current measurement in the DingWang_2D_Gradient object.             *
 *                                                                                            *
 *                                                                                            *
 **********************************************************************************************/

template <class T>
void DingWang_2D_Gradient<T>::Set_measurement(const T temperature, const T x, const T y)//call this funtion after every CTD datum
{
  sample_index=sample_index+1;
  Temperature.change_size(sample_index);
  Temperature(sample_index)=temperature;//input the temperature data
  X.change_size(sample_index);
  X(sample_index)=x;// input x corrdinates
  Y.change_size(sample_index);
  Y(sample_index)=y;// input y corrdinates
}

/******************************************************************************
 *                                                                            *
 *  Method: DingWang_2D_Gradient<T>::Get_gradient_azimuth(void)               *
 *                                                                            *
 *  Description: Return the new estimate of the gradient direction            *
 *                                                                            *
 *                                                                            *
 ******************************************************************************/

template <class T>
T DingWang_2D_Gradient<T>::Get_gradient_azimuth(void)//call this function when the target point is reached
{
  Azimuth=t_gra_azimuth(Temperature, X, Y);
  sample_index=0;
  return Azimuth;
}

/******************************************************************************
 *                                                                            *
 *  Function: t_gra_azimuth(Vector<T> temperature, Vector<T> x, Vector<T> y)  *
 *                                                                            *
 *  Description: Compute the new estimate of the gradient direction using a   *
 *  linear least squares fitting method.                                      *
 *                                                                            *
 ******************************************************************************/

template <class T>
T t_gra_azimuth(Vector<T> temperature, Vector<T> x, Vector<T> y) // horizontal gradient of temperature
{
  int N=x.dim();
  int M=y.dim();
  assert(N>=3 && N==M);
  Fortran_matrix<T> XY1(N, 3, 1);
  Fortran_matrix<T> Tp(N, 1, temperature);
  Fortran_matrix<T> A_B_C(3,1);

  for (int j=1; j<=N; j++)
    {
      XY1(j,1)=x(j);
      XY1(j,2)=y(j);
    }

  A_B_C=inv(transpose(XY1)*XY1)*transpose(XY1)*Tp;
 
  T azimuth;
  if (A_B_C(1,1)>=0 && A_B_C(2,1)>=0)
    azimuth=atan(A_B_C(1,1)/A_B_C(2,1))*180/M_PI;
  else if (A_B_C(1,1)>=0 && A_B_C(2,1)<=0)
    azimuth=atan(A_B_C(1,1)/A_B_C(2,1))*180/M_PI+180;
  else if (A_B_C(1,1)<=0 && A_B_C(2,1)<=0)
    azimuth=atan(A_B_C(1,1)/A_B_C(2,1))*180/M_PI+180;
  else if (A_B_C(1,1)<=0 && A_B_C(2,1)>=0)
    azimuth=atan(A_B_C(1,1)/A_B_C(2,1))*180/M_PI+360;
  return azimuth;
}
