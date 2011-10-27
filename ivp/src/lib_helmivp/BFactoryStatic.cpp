/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BFactoryStatic.h                                     */
/*    DATE: Feb 11th, 2010                                       */
/*                                                               */
/* (IvPHelm) The IvP autonomous control Helm is a set of         */
/* classes and algorithms for a behavior-based autonomous        */
/* control architecture with IvP action selection.               */
/*                                                               */
/* The algorithms embodied in this software are protected under  */
/* U.S. Pat. App. Ser. Nos. 10/631,527 and 10/911,765 and are    */
/* the property of the United States Navy.                       */
/*                                                               */
/* Permission to use, copy, modify and distribute this software  */
/* and its documentation for any non-commercial purpose, without */
/* fee, and without a written agreement is hereby granted        */
/* provided that the above notice and this paragraph and the     */
/* following three paragraphs appear in all copies.              */
/*                                                               */
/* Commercial licences for this software may be obtained by      */
/* contacting Patent Counsel, Naval Undersea Warfare Center      */
/* Division Newport at 401-832-4736 or 1176 Howell Street,       */
/* Newport, RI 02841.                                            */
/*                                                               */
/* In no event shall the US Navy be liable to any party for      */
/* direct, indirect, special, incidental, or consequential       */
/* damages, including lost profits, arising out of the use       */
/* of this software and its documentation, even if the US Navy   */
/* has been advised of the possibility of such damage.           */
/*                                                               */
/* The US Navy specifically disclaims any warranties, including, */
/* but not limited to, the implied warranties of merchantability */
/* and fitness for a particular purpose. The software provided   */
/* hereunder is on an 'as-is' basis, and the US Navy has no      */
/* obligations to provide maintenance, support, updates,         */
/* enhancements or modifications.                                */
/*****************************************************************/

#include "BFactoryStatic.h"
#include "MBUtils.h"
#include <iostream>
#include <cstdlib>

// CORE Behaviors
#include "BHV_Waypoint.h"
#include "BHV_Loiter.h"
#include "BHV_OpRegion.h"
#include "BHV_ConstantDepth.h"
#include "BHV_ConstantHeading.h"
#include "BHV_MaintainHeading.h"
#include "BHV_ConstantSpeed.h"
#include "BHV_PeriodicSpeed.h"
#include "BHV_PeriodicSurface.h"
#include "BHV_Trail.h"
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
#include "BHV_AvoidObstacles.h"
#include "BHV_GoToDepth.h"
#include "BHV_MemoryTurnLimit.h"
#include "BHV_Hysteresis.h"
#include "BHV_Attractor.h"
#include "BHV_RubberBand.h"
#include "BHV_HeadingBias.h"
#include "BHV_MinAltitudeX.h"

using namespace std;

//--------------------------------------------------------------------
// Procedure: isKnownBehavior()

bool BFactoryStatic::isKnownBehavior(string bhv_name) const 
{
  if((bhv_name == "BHV_OpRegion")        ||
     (bhv_name == "BHV_Waypoint")        || 
     (bhv_name == "BHV_ConstantSpeed")   || 
     (bhv_name == "BHV_Trail")           || 
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
     (bhv_name == "BHV_AvoidObstacles")  || 
     (bhv_name == "BHV_PeriodicSpeed")   || 
     (bhv_name == "BHV_PeriodicSurface") || 
     (bhv_name == "BHV_GoToDepth")       || 
     (bhv_name == "BHV_MemoryTurnLimit") || 
     (bhv_name == "BHV_Hysteresis")      || 
     (bhv_name == "BHV_Attractor")       || 
     (bhv_name == "BHV_RubberBand")      || 
     (bhv_name == "BHV_HeadingBias")     ||
     (bhv_name == "BHV_MinAltitudeX"))
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
  else if(bhv_name == "BHV_AvoidObstacles") 
    bhv = new BHV_AvoidObstacles(m_domain);
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

  return(bhv);
}


