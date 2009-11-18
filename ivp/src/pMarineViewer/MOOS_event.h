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
