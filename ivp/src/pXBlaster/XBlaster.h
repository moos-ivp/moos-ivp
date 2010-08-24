/************************************************************/
/*    NAME: Michael Benjamin                                */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge             */
/*    FILE: XBlaster.h                                      */
/*    DATE: Feb 20th 2010                                   */
/************************************************************/

#ifndef P_XBLASTER_HEADER
#define P_XBLASTER_HEADER

#include "MOOSLib.h"

class XBlaster : public CMOOSApp
{
 public:
  XBlaster();
  virtual ~XBlaster() {};

  bool OnNewMail(MOOSMSG_LIST&) {return(true);};
  bool Iterate();
  bool OnConnectToServer() {return(true);};
  bool OnStartUp();
  void RegisterVariables();

 protected:
  std::string randomString(unsigned int length, bool lower=true);

 protected:
  unsigned int m_tally_sent;
  unsigned int m_iterations;

  unsigned int m_var_sizes;
  unsigned int m_string_sizes;
  unsigned int m_posts_per_iter;
};

#endif 
