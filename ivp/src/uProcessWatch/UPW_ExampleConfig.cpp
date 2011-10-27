/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UPW_ExampleConfig.cpp                                */
/*    DATE: Aug 8th 2011                                         */
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
 
#include <iostream>
#include "UPW_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
  blk("=============================================================== ");
  blk("uProcessWatch Example MOOS Configuration                        ");
  blk("=============================================================== ");
  blu("Blue lines:     ","Default configuration                        ");
  mag("Magenta lines:  ","Non-default configuration                    ");
  blk("                                                                ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blu("  WATCH_ALL = true                                              ");
  blk("                                                                ");
  mag("  WATCH  = pNodeReporter, iChauffeur:CHAUFEUR_OK                ");
  mag("  WATCH  = pHelmIvP                                             ");
  blk("                                                                ");
  mag("  NOWATCH = uXMS*                                               ");
  blk("                                                                ");
  mag("  SUMMARY_WAIT = 10  ","// Seconds                              ");
  blk("                                                                ");
  mag("  POST_MAPPING = PROC_WATCH_FULL_SUMMARY, UPW_FULL_SUMMARY      ");
  blk("}                                                               ");
}

