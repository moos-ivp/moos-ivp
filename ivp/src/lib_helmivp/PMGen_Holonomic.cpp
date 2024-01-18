/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PMGen_Holonomic.cpp                                  */
/*    DATE: Oct 24th, 2023                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include "MBUtils.h"
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "PMGen_Holonomic.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: generate()

PlatModel PMGen_Holonomic::generate(double osx, double osy,
				    double osh, double osv)
{
  PlatModel pmodel("holo");
  return(pmodel);
}
