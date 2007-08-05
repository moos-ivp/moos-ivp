/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: VRange.cpp                                           */
/*    DATE: March 4th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "VReport.h"

using namespace std;

//---------------------------------------------------------------
// Constructor

VReport::VReport(int vehicles)
{
  vector<double> slots;
  slots.push_back(2);
  slots.push_back(5);
  slots.push_back(10);
  slots.push_back(15);
  slots.push_back(20);
  slots.push_back(25);
  slots.push_back(30);
  slots.push_back(40);
  slots.push_back(50);
  slots.push_back(60);
  slots.push_back(70);
  slots.push_back(80);
  slots.push_back(90);
  slots.push_back(100);

  for(int i=0; i<vehicles; i++) {
    SlotReport slot_report;
    slot_report.setSlots(slots);
    prox_report.push_back(slot_report);
  }
    

  slots.clear();
  slots.push_back(1);
  slots.push_back(3);
  slots.push_back(5);
  for(int j=0; j<vehicles; j++) {
    SlotReport slot_report;
    slot_report.setSlots(slots);
    stat_report.push_back(slot_report);
  }
}


//---------------------------------------------------------------
// Procedure: addDistInfo
//      Note: ix -   vehicle index
//            time - duration of time at this distance
//            dist - distance to some (doesn't matter) vehicle

void VReport::addDistInfo(int ix, double time, double dist)
{
  int vsize = prox_report.size();
  if((ix < 0) || (ix >= vsize))
    return;
  if(time <= 0)
    return;

  prox_report[ix].addInfo(dist, time);
}


//---------------------------------------------------------------
// Procedure: addStatInfo
//      Note: ix -   vehicle index
//            time - duration of time at this distance
//            dist - distance to some station

void VReport::addStatInfo(int ix, double time, double dist)
{
  int vsize = stat_report.size();
  if((ix < 0) || (ix >= vsize))
    return;
  if(time <= 0)
    return;

  stat_report[ix].addInfo(dist, time);
}


//---------------------------------------------------------------
// Procedure: getProxReport

SlotReport VReport::getProxReport(int ix)
{
  SlotReport report;
  int vsize = prox_report.size();
  if((ix < 0) || (ix >= vsize))
    return(report);
  else
    return(prox_report[ix]);
}


//---------------------------------------------------------------
// Procedure: getStatReport

SlotReport VReport::getStatReport(int ix)
{
  SlotReport report;
  int vsize = stat_report.size();
  if((ix < 0) || (ix >= vsize))
    return(report);
  else
    return(stat_report[ix]);
}






