/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: TMOOSClient.h                                        */
/*   DATE: Dec 2nd 2011                                         */
/****************************************************************/

#ifndef TMOOS_CLIENT_HEADER
#define TMOOS_CLIENT_HEADER

#include <string>
#include "MOOSLib.h"

class TMOOSClient : public CMOOSApp
{
 public:
  TMOOSClient();
  virtual ~TMOOSClient() {};

  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

  void SetPrefix(const std::string& s) {m_postvar="UMC_"+s;};

 protected:
  void RegisterVariables();

 protected:
  unsigned int m_counter;
  unsigned int m_posts_per_iter;
  std::string  m_postvar;
};

#endif 



#if 0


// Sorted List of ints
// Each new mail, push the new int into the list
// Pop the list if (a) the list size > N
//                 (b) the popped item's next item is +1
//              else
//                post a warning


#endif
