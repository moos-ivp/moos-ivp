/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SlotReport.cpp                                       */
/*    DATE: March 4th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include "SlotReport.h"

using namespace std;

//---------------------------------------------------------------
// Procedure: setSlots
                    
void SlotReport::setSlots(vector<double> new_slots)
{
  clear();
  
  // first ensure all slots are in strict order, if not - return.
  int vsize = new_slots.size();
  double prev_val = 0;
  for(int i=0; i<vsize; i++) {
    if(new_slots[i] < prev_val)
      return;
    prev_val = new_slots[i];
  }

  // If all ok, then re-initialize the slots and times vectors.
  slots = new_slots;
  for(int j=0; j<vsize; j++)
    times.push_back(0);

}


//---------------------------------------------------------------
// Procedure: addInfo
//
//  {25, 50}                                                
//     slots[0] = 0   times[0] = 0                                    
//     slots[1] = 25  times[1] = 0                                    
//     time_over = 0
//                                                                 
//  addInfo(10, 2), addInfo(25, 3), addInfo(80, 4)
//     times[0] = 2
//     times[1] = 3
//     time_over = 4
                    
void SlotReport::addInfo(double dist, double time)
{
  // first check for error condition
  if(dist < 0)
    return;
  
  time_total += time;

  double vsize = slots.size();
  for(int i=0; i<vsize; i++) {
    if(dist <= slots[i]) {
      times[i] += time;
      return;
    }
  }
  time_over += time;
}

//---------------------------------------------------------------
// Procedure: clear()
                    
void SlotReport::clear()
{
  slots.clear();
  times.clear();
  time_over = 0;
  time_total = 0;
}

//---------------------------------------------------------------
// Procedure: getSlotVal
                    
double SlotReport::getSlotVal(int ix)
{
  if((ix >= 0) && (ix < slots.size()))
    return(slots[ix]);
  else
    return(0);
}

//---------------------------------------------------------------
// Procedure: getSlotTime
                    
double SlotReport::getSlotTime(int ix)
{
  if((ix >= 0) && (ix < times.size()))
    return(times[ix]);
  else
    return(0);
}






