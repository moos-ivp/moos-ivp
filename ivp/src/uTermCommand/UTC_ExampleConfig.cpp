/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UTC_ExampleConfig.cpp                                */
/*    DATE: Aug 25th 2011                                        */
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
#include "UTC_ExampleConfig.h"
#include "ColorParse.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: showExampleConfig

void showExampleConfig()
{
  blk("=============================================================== ");
  blk("uTermCommand Example MOOS Configuration                         ");
  blk("=============================================================== ");
  blk("                                                                ");
  blk("ProcessConfig = uTermCommand                                    ");
  blk("{                                                               ");
  blu("  AppTick   = 4                                                 ");
  blu("  CommsTick = 4                                                 ");
  blk("                                                                ");
  blk("  CMD  =  1 -->  DEPLOY -->               false                 ");
  blk("  CMD  =  2 -->  DEPLOY -->               false                 ");
  blk("  CMD  =  3 -->  MOOS_MANUAL_OVERIDE -->  true                  ");
  blk("  CMD  =  4 -->  MOOS_MANUAL_OVERIDE -->  false                 ");
  blk("  CMD  =  5 -->  STATION_KEEP -->         true                  ");
  blk("  CMD  =  6 -->  STATION_KEEP -->         false                 ");
  blk("  CMD  =  7 -->  RETURN -->               true                  ");
  blk("  CMD  =  8 -->  RETURN -->               false                 ");
  blk("                                                                ");
  blk("  CMD  =  px --> PERMUTATIONS -->         false                 ");
  blk("  CMD  =  pg --> PERMUTATIONS -->         true                  ");
  blk("  CMD  =  pr --> LOITER_REMAP -->         true                  ");
  blk("                                                                ");
  blk("  CMD  =  hv --> HELM_VERBOSE -->         verbose               ");
  blk("  CMD  =  ht --> HELM_VERBOSE -->         terse                 ");
  blk("  CMD  =  hq --> HELM_VERBOSE -->         quiet                 ");
  blk("}                                                               ");
  blk("                                                                ");
}

