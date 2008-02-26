/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: AOF_HArrayTurn.cpp                                */
/*    BORN: 23 July 05                                     */
/***********************************************************/
#include <iostream>
#include <math.h> 
#include <assert.h>
#include "AOF_HArrayTurn.h"
#include "AngleUtils.h"
#include "IvPDomain.h"

using namespace std;

//----------------------------------------------------------
// Procedure: Constructor

AOF_HArrayTurn::AOF_HArrayTurn(IvPDomain g_domain, 
			     double course_fix, double course)
  : AOF(g_domain)
{
  int crs_ix = g_domain.getIndex("course");
  
  assert(crs_ix != -1);

  crsDelta = g_domain.getVarDelta(crs_ix);
  crsBase  = g_domain.getVarLow(crs_ix);

  leftabs = course_fix-90.0;
  if (leftabs < 0.0)
    leftabs += 360.0;

  rightabs = course_fix + 90.0;
  if(rightabs > 360.0)
    rightabs -= 360.0;

  //half the width of the range of courses to 
  //consider centered around the desired course
  double width = 20;

  hwidth  = width;
  osCourse = course;

}

//----------------------------------------------------------------
// Procedure: evalBox
//   Purpose: Evaluates a given course
//     

double AOF_HArrayTurn::evalBox(const IvPBox *b) const
{
 
  double evalCRS  = crsBase + ((double)(b->pt(0,0)) * crsDelta);
  double mval;
 
  mval = (lrmetric(evalCRS));

  return mval;
}

//----------------------------------------------------------------
// Procedure: metric

double AOF_HArrayTurn::lrmetric(double evalCRS) const
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
      mval = (200-right_perr)/2.0;
    }
  else
    {
    if (right_perr <= left_perr)
      mval = (((200-right_perr)/2.0) + (90.0 - right_cerr)/2.0);
    else
      mval =(200-left_perr)/2.0;
    }

    if ((left_perr > hwidth) && (right_perr > hwidth))
       mval = 0.0;

    return mval;  
}


