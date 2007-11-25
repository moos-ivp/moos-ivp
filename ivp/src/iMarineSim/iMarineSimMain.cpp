/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: iMarineSimMain.cpp                                   */
/*    DATE: Jun 23rd 2006                                        */
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
#include "MOOSGenLib.h"
#include "IMS_MOOSApp.h"

//--------------------------------------------------------
// Procedure: main

int main(int argc ,char * argv[])
{
  char *sMissionFile = "Example.moos";
  
  if(argc>1)
    sMissionFile = argv[1];
  
  IMS_Model *ims_model = new IMS_Model;

  IMS_MOOSApp marine_sim;
  marine_sim.setModel(ims_model);

  marine_sim.Run("iMarineSim", sMissionFile);
 
  return(0);
}



