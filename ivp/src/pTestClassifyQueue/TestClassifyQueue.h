/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT                                             */
/*    FILE: TestClassifyQueue.h                             */
/*    DATE: March 23rd, 2013                                */
/************************************************************/

#ifndef TEST_CLASSIFY_QUEUE_HEADER
#define TEST_CLASSIFY_QUEUE_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include "ClassifyQueue.h"

class TestClassifyQueue : public AppCastingMOOSApp
{
 public:
   TestClassifyQueue();
   ~TestClassifyQueue() {};

 protected: // Standard MOOSApp functions to overload  
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();

 protected: 
   bool handleMailClassifyEntry(std::string);

 protected: // Standard AppCastingMOOSApp function to overload 
   bool buildReport();

 protected:
   void registerVariables();

 private: // Configuration variables
   double m_queue_time;

 private: // State variables

   double        m_last_pop_time;
   ClassifyQueue m_classify_queue;
};

#endif 
