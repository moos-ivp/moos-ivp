/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SafetyRetreatMain.cpp                                */
/*    DATE: August 2010                                          */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include "MOOSLib.h"
#include "MBUtils.h"
#include "ReleaseInfo.h"
#include "MOOSGenLib.h"
#include "SafetyRetreat.h"

using namespace std;

int main(int argc, char *argv[])
{
  string mission_file = "SafetyRetreat.moos";
	
  // Look for a request for version information
  if(scanArgs(argc, argv, "-v", "--version", "-version")) {
    showReleaseInfo("pSafetyRetreat", "gpl");
    return(0);
  }
  // Look for a request for example configuration information
  //if(scanArgs(argc, argv, "-e", "--example", "-example")) {
  //  showExampleConfig();
  //  return(0);
  // }


  if(argc > 1)
    mission_file = argv[1];
	
  SafetyRetreat SafetyRetreat;
	
  SafetyRetreat.Run("pSafetyRetreat", mission_file.c_str());

  return(0);
}


