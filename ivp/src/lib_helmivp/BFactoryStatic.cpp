/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BFactoryStatic.h                                     */
/*    DATE: Feb 11th, 2010                                       */
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

#include "BFactoryStatic.h"
#include "MBUtils.h"
#include <iostream>
#include <cstdlib>

// CORE Behaviors
#include "BHV_Waypoint.h"
#include "BHV_Loiter.h"
#include "BHV_OpRegion.h"
#include "BHV_MaxDepth.h"
#include "BHV_MaxSpeed.h"
#include "BHV_ConstantDepth.h"
#include "BHV_ConstantHeading.h"
#include "BHV_MaintainHeading.h"
#include "BHV_ConstantSpeed.h"
#include "BHV_PeriodicSpeed.h"
#include "BHV_PeriodicSurface.h"
#include "BHV_Trail.h"
#include "BHV_Convoy.h"

#include "BHV_Shadow.h"
#include "BHV_Timer.h"
#include "BHV_TestFailure.h"
#include "BHV_HSLine.h"
#include "BHV_BearingLine.h"
#include "BHV_HeadingChange.h"
#include "BHV_StationKeep.h"
#include "BHV_RStationKeep.h"
#include "BHV_CutRange.h"
#include "BHV_AvoidCollision.h"
#include "BHV_AvoidObstacle.h"
#include "BHV_GoToDepth.h"
#include "BHV_MemoryTurnLimit.h"
#include "BHV_Hysteresis.h"
#include "BHV_Attractor.h"
#include "BHV_RubberBand.h"
#include "BHV_HeadingBias.h"
#include "BHV_MinAltitudeX.h"
#include "BHV_AvdColregsV17.h"
#include "BHV_AvdColregsV19.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: isKnownBehavior()

bool BFactoryStatic::isKnownBehavior(string bhv_name) const 
{
  if((bhv_name == "BHV_OpRegion")        ||
     (bhv_name == "BHV_Waypoint")        || 
     (bhv_name == "BHV_ConstantSpeed")   || 
     (bhv_name == "BHV_Trail")           || 
     (bhv_name == "BHV_Convoy")          || 
     (bhv_name == "BHV_MaxDepth")        || 
     (bhv_name == "BHV_MaxSpeed")        || 
     (bhv_name == "BHV_ConstantDepth")   || 
     (bhv_name == "BHV_ConstantHeading") || 
     (bhv_name == "BHV_MaintainHeading") || 
     (bhv_name == "BHV_Loiter")          || 
     (bhv_name == "BHV_StationKeep")     || 
     (bhv_name == "BHV_RStationKeep")    || 
     (bhv_name == "BHV_Timer")           || 
     (bhv_name == "BHV_TestFailure")     || 
     (bhv_name == "BHV_HSLine")          || 
     (bhv_name == "BHV_BearingLine")     || 
     (bhv_name == "BHV_HeadingChange")   || 
     (bhv_name == "BHV_Shadow")          || 
     (bhv_name == "BHV_CutRange")        || 
     (bhv_name == "BHV_AvoidCollision")  || 
     (bhv_name == "BHV_AvoidObstacle")   || 
     (bhv_name == "BHV_PeriodicSpeed")   || 
     (bhv_name == "BHV_PeriodicSurface") || 
     (bhv_name == "BHV_GoToDepth")       || 
     (bhv_name == "BHV_MemoryTurnLimit") || 
     (bhv_name == "BHV_Hysteresis")      || 
     (bhv_name == "BHV_Attractor")       || 
     (bhv_name == "BHV_RubberBand")      || 
     (bhv_name == "BHV_HeadingBias")     ||
     (bhv_name == "BHV_MinAltitudeX")    ||
     (bhv_name == "BHV_AvdColregsV17")   ||
     (bhv_name == "BHV_AvdColregsV19"))
    return(true);
  else
    return(false);
}

//--------------------------------------------------------------------
// Procedure: newBehavior()

IvPBehavior* BFactoryStatic::newBehavior(string bhv_name) const 
{
  if(m_domain.size() == 0)
    return(0);
  
  IvPBehavior *bhv = 0;

  if(bhv_name == "BHV_OpRegion")
    bhv = new BHV_OpRegion(m_domain);
  else if(bhv_name == "BHV_Waypoint")
    bhv = new BHV_Waypoint(m_domain);
  else if(bhv_name == "BHV_ConstantSpeed")     
    bhv = new BHV_ConstantSpeed(m_domain);
  else if(bhv_name == "BHV_Trail")      
    bhv = new BHV_Trail(m_domain);
  else if(bhv_name == "BHV_Convoy")      
    bhv = new BHV_Convoy(m_domain);
  else if(bhv_name == "BHV_MaxDepth")      
    bhv = new BHV_MaxDepth(m_domain);
  else if(bhv_name == "BHV_MaxSpeed")      
    bhv = new BHV_MaxSpeed(m_domain);
  else if(bhv_name == "BHV_ConstantDepth")      
    bhv = new BHV_ConstantDepth(m_domain);
  else if(bhv_name == "BHV_ConstantHeading")      
    bhv = new BHV_ConstantHeading(m_domain);
  else if(bhv_name == "BHV_MaintainHeading")      
    bhv = new BHV_MaintainHeading(m_domain);
  else if(bhv_name == "BHV_Loiter")     
    bhv = new BHV_Loiter(m_domain);
  else if(bhv_name == "BHV_StationKeep")     
    bhv = new BHV_StationKeep(m_domain);
  else if(bhv_name == "BHV_RStationKeep")     
    bhv = new BHV_RStationKeep(m_domain);
  else if(bhv_name == "BHV_Timer")     
    bhv = new BHV_Timer(m_domain);
  else if(bhv_name == "BHV_TestFailure")     
    bhv = new BHV_TestFailure(m_domain);
  else if(bhv_name == "BHV_HSLine")     
    bhv = new BHV_HSLine(m_domain);
  else if(bhv_name == "BHV_BearingLine")     
    bhv = new BHV_BearingLine(m_domain);
  else if(bhv_name == "BHV_HeadingChange")     
    bhv = new BHV_HeadingChange(m_domain);
  else if(bhv_name == "BHV_Shadow")     
    bhv = new BHV_Shadow(m_domain);
  else if(bhv_name == "BHV_CutRange")   
    bhv = new BHV_CutRange(m_domain);
  else if(bhv_name == "BHV_AvoidCollision") 
    bhv = new BHV_AvoidCollision(m_domain);
  else if(bhv_name == "BHV_AvoidObstacle") 
    bhv = new BHV_AvoidObstacle(m_domain);
  else if(bhv_name == "BHV_PeriodicSpeed") 
    bhv = new BHV_PeriodicSpeed(m_domain);
  else if(bhv_name == "BHV_PeriodicSurface") 
    bhv = new BHV_PeriodicSurface(m_domain);
  else if(bhv_name == "BHV_GoToDepth")      
    bhv = new BHV_GoToDepth(m_domain);
  else if(bhv_name == "BHV_MemoryTurnLimit")      
    bhv = new BHV_MemoryTurnLimit(m_domain);
  else if(bhv_name == "BHV_Hysteresis")      
    bhv = new BHV_Hysteresis(m_domain);
  else if(bhv_name == "BHV_Attractor")      
    bhv = new BHV_Attractor(m_domain);
  else if(bhv_name == "BHV_RubberBand")      
    bhv = new BHV_RubberBand(m_domain);
  else if(bhv_name == "BHV_HeadingBias")     
    bhv = new BHV_HeadingBias(m_domain);
  else if(bhv_name == "BHV_MinAltitudeX")     
    bhv = new BHV_MinAltitudeX(m_domain);
  else if(bhv_name == "BHV_AvdColregsV17")     
    bhv = new BHV_AvdColregsV17(m_domain);
  else if(bhv_name == "BHV_AvdColregsV19")     
    bhv = new BHV_AvdColregsV19(m_domain);

  return(bhv);
}
