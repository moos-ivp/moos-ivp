/*************************************************************/
/*    NAME: Michael Benjamin                                 */
/*    FILE: FV_MOOSApp.cpp                                   */
/*    DATE: May 12th 2006                                    */
/*************************************************************/
#include <iostream>
#include <iterator>
#include "FV_MOOSApp.h"
#include "FunctionEncoder.h"
#include "MBUtils.h"
#include "OF_Reflector.h"
#include "AOF_Rings.h"
#include "BuildUtils.h"

using namespace std; 

//----------------------------------------------------------
// Procedure: Constructor

FV_MOOSApp::FV_MOOSApp()
{
  loc_x      = 50;
  loc_y      = 50;
  iteration  = 0;
  start_time = 0;
  delta      = 10;
  model      = 0;
  viewer     = 0;
  gui        = 0;
  ipf_name   = "BHV_IPF";
//   iterate_should_return_false = false;
}

//----------------------------------------------------------
// Procedure: OnNewMail()

bool FV_MOOSApp::OnNewMail(MOOSMSG_LIST &NewMail)
{
  try {
     demuxer_lock.Lock();

     // The main thread will may be blocking on a call to 'Fl::wait()'.
     // This will wake up the main thread if that's the case.
     Fl::awake();

     MOOSMSG_LIST::iterator p;
     for(p=NewMail.begin(); p!=NewMail.end(); p++) {
        CMOOSMsg &msg = *p;

        if(msg.m_sKey == ipf_name) {
           string ipf_str = msg.m_sVal;
           demuxer.addMuxPacket(ipf_str, MOOSTime());
        }
     }
  }
  catch (...) {
     demuxer_lock.UnLock();
     throw;
  }

  demuxer_lock.UnLock();
  return(true);
}

//----------------------------------------------------------
// Procedure: OnConnectToServer()
//      Note: Register for variables here. 
//            Possibly read info from the mission file.
//            m_MissionReader.GetConfigurationParam("Name", <string>);
//            m_Comms.Register("VARNAME", is_float(int));

bool FV_MOOSApp::OnConnectToServer()
{
  if(!m_MissionReader.GetValue("ipf_name", ipf_name)) {
    MOOSTrace("Function to be monitored not provided\n");
    //return(false);
  }

  if(ipf_name != "")
    m_Comms.Register(ipf_name, 0);
  return(true);
}


//----------------------------------------------------------
// Procedure: Iterate()

bool FV_MOOSApp::Iterate()
{
//   return (! iterate_should_return_false);
  return true;
  
  // This processing has all been moved to the process_demuxer_content()
  // method, so that it can happen in the same thread as other FLTK operations.
}

//----------------------------------------------------------
// Procedure: process_demuxer_content()


void FV_MOOSApp::process_demuxer_content()
{
  try {
    demuxer_lock.Lock();

    iteration++;

    if((iteration %10)==0) {
      double hz = (double)(iteration) / ((MOOSTime() - start_time));
      cout << "Iteration: (" << iteration << ")";
      cout << "(" << hz << "/sec)" << endl;
    }

    cout << "[" << iteration << "]";
    string str = demuxer.getDemuxString();
    bool redraw_needed = false;
    while(str != "") {
      redraw_needed = true;
      cout << "+";
      if(model) {
          cout << "Add Demux TO Model" << endl;
          model->addIPF(str);
      }
      str = demuxer.getDemuxString();
    }
    cout << endl;
    if(redraw_needed) {
      viewer->resetQuadSet();
      viewer->redraw();
    }
    if(gui)
      gui->updateFields();
  }
  catch (...) {
    demuxer_lock.UnLock();
    throw;
  }

  demuxer_lock.UnLock();
}

//----------------------------------------------------------
// Procedure: OnStartUp()
//      Note: Happens before connection is open

bool FV_MOOSApp::OnStartUp()
{
  start_time = MOOSTime();  
  return(true);
}


//----------------------------------------------------------
// Procedure: return_from_Run()
//      Note: Call this to cause this object's CMOOSApp::Run(...) method to 
//      return.

// void FV_MOOSApp::return_from_Run()
// {
// // //   iterate_should_return_false = true;
//   SetQuitOnFailedIterate(true);
// }
