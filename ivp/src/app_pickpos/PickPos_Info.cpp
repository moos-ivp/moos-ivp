/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PickPos_Info.cpp                                     */
/*    DATE: Sep 22nd, 2018                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <cstdlib> 
#include <iostream>
#include "ColorParse.h"
#include "ReleaseInfo.h"
#include "PickPos_Info.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showSynopsis

void showSynopsis()
{
  blk("SYNOPSIS:                                                       ");
  blk("------------------------------------                            ");
  blk("  pick_pos is a utility for generating starting points for      ");
  blk("  simulated vehicles. It can generate points from a set of pts  ");
  blk("  given in a file, or by picking random points from within one  ");
  blk("  or more convex polygons. The starting heading position can    ");
  blk("  also be chosen, in a random range, or relative to a given     ");
  blk("  position.                                                     ");
  blk("  Vehicle names and group names may also be chosen randomly.    ");
}

//----------------------------------------------------------------
// Procedure: showHelpAndExit

void showHelpAndExit()
{
  cout << "=====================================================" << endl;
  cout << "Usage: pickpos [OPTIONS]                             " << endl;
  cout << "=====================================================" << endl;
  cout << "                                                     " << endl;
  showSynopsis();
  cout << "                                                     " << endl;
  cout << "Options:                                             " << endl;
  cout << "  --help, -h                                         " << endl;
  cout << "     Display this help message.                      " << endl;
  cout << "  --version,-v                                       " << endl;
  cout << "     Display the release version of pickpos.         " << endl;
  cout << "  --verbose                                          " << endl;
  cout << "     Produce more verbose output                     " << endl;
  cout << "  --amt=<num>                                        " << endl;
  cout << "     Specify number of points to make (default=10)   " << endl;
  cout << "  --buffer=<num>                                     " << endl;
  cout << "     Distance between random points if selecting from" << endl;
  cout << "     a polygon(s). May not always be able to respect " << endl;
  cout << "     this distance depending on size of poly and pts." << endl;
  cout << "     The default value is 10.                        " << endl;
  cout << "  --maxtries=<num>                                   " << endl;
  cout << "     The number of times the random number generator " << endl;
  cout << "     will retry to make a new random point if the one" << endl;
  cout << "     chosen is less than the buffer dist to any pts. " << endl;
  cout << "     The default value is 1000.                      " << endl;
  cout << "  --posfile=<filename>                               " << endl;
  cout << "     Name of file from which to make random subset of" << endl;
  cout << "     points. File must contain more lines than points" << endl;
  cout << "     to generate.                                    " << endl;
  cout << "  --psnap=<num>                                      " << endl;
  cout << "     Snap value for rounding x,y coords. The default " << endl;
  cout << "     is 1, which rounds to nearest integer. A value  " << endl;
  cout << "     of 0.1 rounds to the nearest tenth and so on.   " << endl;
  cout << "  --hsnap=<num>                                      " << endl;
  cout << "     Snap value for rounding heading val. The default" << endl;
  cout << "     is 1, which rounds to nearest integer. A value  " << endl;
  cout << "     of 0.1 rounds to the nearest tenth and so on.   " << endl;
  cout << "  --ssnap=<num>                                      " << endl;
  cout << "     Snap value for rounding speed val. Default is   " << endl;
  cout << "     0.1, which rounds to nearest 1/10th meters/sec. " << endl;
  cout << "  --polygon=<poly>                                   " << endl;
  cout << "     Specify a polygon region of the form:           " << endl;
  cout << "     \"0,0 : 50,0 : 50,50 : 0,50\"                   " << endl;
  cout << "  --vnames                                           " << endl;
  cout << "     Generate a set of unique vehicle names, one for " << endl;
  cout << "     each position or speed being generated.         " << endl;
  cout << "  --group_names_one, -g1                             " << endl;
  cout << "     Generate a set of names from Set 1              " << endl;
  cout << "  --group_names_one, -g2                             " << endl;
  cout << "     Generate a set of names from Set 2              " << endl;
  cout << "  --group_names_one, -g3                             " << endl;
  cout << "     Generate a set of names from Set 3              " << endl;
  cout << "  --group_names_one, -g4                             " << endl;
  cout << "     Generate a set of names from Set 4              " << endl;
  cout << "  --vix=<index>                                      " << endl;
  cout << "     Generate vehicle names starting at this index   " << endl;
  cout << "  --reverse_names, -r                                " << endl;
  cout << "     Generate a set of unique vehicle names, same as " << endl;
  cout << "     --vnames but in reverse order.                  " << endl;
  cout << "  --vnames=kobe,tom,bernie                           " << endl;
  cout << "     Generate a set of unique vehicle names, one for " << endl;
  cout << "     each position or speed being generated. First   " << endl;
  cout << "     names chosen from the given list, otherwise the " << endl;
  cout << "     names names are chosen from stock list.         " << endl;
  cout << "  --colors                                           " << endl;
  cout << "     Generate a set of unique colors.                " << endl;
  cout << "  --colors=red,white,blue,green                      " << endl;
  cout << "     Generate a set of colors. First colors chosen   " << endl;
  cout << "     from the given list, otherwise the colors are   " << endl;
  cout << "     chosen from stock list.                         " << endl;
  cout << "  --hdg=<config>                                     " << endl;
  cout << "     Specify how initial heading vals are chosen. If " << endl;
  cout << "     left unspecified, no heading val will be made.  " << endl;
  cout << "     Config type: <Number:Number>: A random heading  " << endl;
  cout << "        between the two numbers will be chosen.      " << endl;
  cout << "     Config type: <X,Y,RelBearing> A heading will be " << endl;
  cout << "        chosen such that the given X,Y point will    " << endl;
  cout << "        have specified relative bearing to the point." << endl;
  cout << "  --spd=<config>                                     " << endl;
  cout << "     Specify how initial speed values are chosen. If " << endl;
  cout << "     left unspecified, no speed value will be made.  " << endl;
  cout << "     Config type: <Number:Number>: A random speed    " << endl;
  cout << "        between the two numbers will be chosen.      " << endl;
  cout << "  --grps=one,two,three[:alt]                         " << endl;
  cout << "     Choose a group name for each vehicle from the   " << endl;
  cout << "     given set. Choices will be random unless :alt   " << endl;
  cout << "     suffix is used, in which case they alternate.   " << endl;
  cout << "                                                     " << endl;
  cout << "  --web,-w   Open browser to:                        " << endl;
  cout << "             https://oceanai.mit.edu/ivpman/apps/pickpos " << endl;
  cout << "                                                     " << endl;
  cout << "                                                     " << endl;
  cout << "Examples:                                            " << endl;
  cout << "  pickpos --amt=5 --vnames                           " << endl;
  cout << "  pickpos --amt=5 --vnames=kobe,jack,wim,pete,sam    " << endl;
  cout << "  pickpos --amt=4 --grps=red,blue,blue               " << endl;
  cout << "  pickpos --amt=4 --grps=true,false:alt              " << endl;
  cout << "  pickpos --amt=5 --poly=pavlab                      " << endl;
  cout << "  pickpos --amt=5 --poly=\"60,-40:60,-160:150,-160:180,-100\" --hdg=-45:45" << endl;
  cout << "  pickpos --amt=5 --poly=\"60,-40:60,-160:150,-160:180,-100\" --hdg=-10,10,-45 --spd=1:5" << endl;
  cout << "  pickpos --amt=5 --poly=\"60,-40:60,-160:150,-160:180,-100\" --buffer=10 --maxtries=200 " << endl;
  cout << "                                                     " << endl;
  cout << "Notes:                                               " << endl;
  cout << "  (1) The --spd switch can be used for generating a  " << endl;
  cout << "      random number for anything, not just speed.    " << endl;
  cout << "  (1) The --grps switch can be used for any kind of   " << endl;
  cout << "      group, e.g., true,false or clockwise,counter.  " << endl;
  cout << "                                                     " << endl;
  
  exit(0);
}


//----------------------------------------------------------------
// Procedure: showReleaseInfoAndExit

void showReleaseInfoAndExit()
{
  showReleaseInfo("pickpos", "gpl");
  exit(0);
}

