/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE:                                                      */
/*    DATE:                                                      */
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
#ifndef MOOS_EVENT_HEADER
#define MOOS_EVENT_HEADER

#include <string>
#include <vector>
#include "MOOSMsg.h"

/// Represents the events associated with MOOSApp's callbacks.  In the
/// case of OnNewMessage, this represents one of the possibly multiple 
/// mail messages that can be delivered in a single OnNewMail invocation.
struct MOOS_event {
  // type is one of {"OnNewMail", "Iterate", "OnStartUp"}
  std::string type;  
  double moos_time;
   
  struct Mail_message {
    Mail_message(CMOOSMsg v) : msg(v) {}
    CMOOSMsg msg;
  };
  
  // Iff type=="OnNewMail", the following is fields are filled in
  // accordingly. The i'th element in the vector is the i'th element 
  // retieved from the MOOSMSG_LIST iterator in the OnNewMail(...) method.
  std::vector<Mail_message> mail;
};

#endif

