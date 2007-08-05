/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: AOF_ArrayAngle.cpp                             */
/*    BORN: 23 July 05                                     */
/***********************************************************/
#include <iostream>
#include <math.h> 
#include <assert.h>
#include "AOF_ArrayAngle.h"
#include "AngleUtils.h"
#include "IvPDomain.h"

using namespace std;

/******************************************************************************
 *                                                                            *
 *  Method: AOF_ArrayAngle(IvPDomain gdomain) : AOF(gdomain)                  *
 *                                                                            *
 *  Description: Constructor for the AOF_ArrayAngle class.                    *
 *                                                                            * 
 ******************************************************************************/

AOF_ArrayAngle::AOF_ArrayAngle(IvPDomain g_domain)
  : AOF(g_domain)
{
  crs_ix = g_domain.getIndex("course");
  spd_ix = g_domain.getIndex("speed");

  assert(crs_ix != -1);
  assert(spd_ix != -1);

}

/******************************************************************************
 *                                                                            *
 *  Method:evalBox(const IvPBox *b) const                                     *
 *                                                                            *
 *  Description: This method is called by the IvP solver and returns the      *
 *  value of the utility of a set of proposed control variables.              *
 *                                                                            * 
 ******************************************************************************/

double AOF_ArrayAngle::evalBox(const IvPBox *b) const
{
  double evalCRS,evalSPD,mval;

  m_domain.getVal(crs_ix,b->pt(crs_ix,0),evalCRS);
  m_domain.getVal(spd_ix,b->pt(spd_ix,0),evalSPD);
    
  mval = (Arraymetric(evalCRS,evalSPD));

  return mval;
}

/******************************************************************************
 *                                                                            *
 *  Method: Arraymetric(double evalCRS,double spd) const                      *
 *                                                                            *
 *  Description: This method is called by evalBox to compute the utility of   *
 *  a proposed set of course and speed control variables. It creates a bimodal*
 *  objective function with the course mode closest to the current ownship    *
 *  course weighted higher.                                                   *   
 *                                                                            * 
 ******************************************************************************/

double AOF_ArrayAngle::Arraymetric(double evalCRS,double spd) const
{
  double mval;

  double left_perr = fabs(evalCRS - leftabs);
  if (left_perr > 180.0)
    left_perr = 360.0 - left_perr;

  double right_perr = fabs(evalCRS - rightabs);
  if (right_perr > 180.0)
    right_perr = 360.0 - right_perr;

  double left_cerr = fabs(osCourse-leftabs);
  if (left_cerr > 180.0)
    left_cerr = 360.0 - left_cerr;

  double right_cerr = fabs(osCourse-rightabs);
  if (right_cerr > 180.0)
    right_cerr = 360.0 - right_cerr;


  if (left_cerr <= right_cerr)
    {
    if (left_perr < right_perr)
      mval = (((200-left_perr)/2.0) + (90.0-left_cerr)/2.0);
    else
      mval = (200-right_perr)/6.0;
    }
  else
    {
    if (right_perr <= left_perr)
      mval = (((200-right_perr)/2.0) + (90.0 - right_cerr)/2.0);
    else
      mval =(200-left_perr)/6.0;
    }

    if ((left_perr > hwidth) && (right_perr > hwidth))
       mval = 0.0;

    return (mval-(fabs(1.5-spd)*10));  
}

/******************************************************************************
 *                                                                            *
 *  Method: setParam(const string& param, double param_val)                   *
 *                                                                            *
 *  Description: This method is called by the BHV_ArrayAngle class and sets   *
 *  the needed parameters.                                                    * 
 *                                                                            * 
 ******************************************************************************/

bool AOF_ArrayAngle::setParam(const string& param, double param_val)
{
  if(param == "width") {
    hwidth = param_val;
    return(true);
  }
  else if(param == "osCourse") {
    osCourse = param_val;
    return(true);
  }  
   else if(param == "t_bearing") {
    t_bearing = param_val;
    return(true);
  }
  else if(param == "desired_angle") {
    desired_angle = param_val;
    return(true);
  }

  
  else
    return(false);
}

/******************************************************************************
 *                                                                            *
 *  Method: initialize()                                                      *
 *                                                                            *
 *  Description: This method is called by the BHV_ArrayAngle class to         *
 *  initialize the center of the two course modes.                            *
 *                                                                            * 
 ******************************************************************************/

bool AOF_ArrayAngle::initialize()
{
  
  raydirec=t_bearing;

  leftabs = raydirec-desired_angle;
  if (leftabs < 0.0)
    leftabs += 360.0;

  rightabs = raydirec + desired_angle;
  if(rightabs > 360.0)
    rightabs -= 360.0;

  return(true);
}
