/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SplitHandlerBundle.cpp                               */
/*    DATE: February 5th, 2015                                   */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "SplitHandlerBundle.h"
#include "LogUtils.h"

using namespace std;

//--------------------------------------------------------
// Procedure: addALogFile

void SplitHandlerBundle::addALogFile(string alog_file)
{
  m_alog_files.push_back(alog_file);
}

//--------------------------------------------------------
// Procedure: checkALogFiles()

bool SplitHandlerBundle::checkALogFiles()
{
  for(unsigned int i=0; i<m_alog_files.size(); i++) {
    SplitHandler handler(m_alog_files[i]);
    bool ok_alog_file = handler.handlePreCheckALogFile();
    if(!ok_alog_file) {
      cout << "Problem with alog file: " << m_alog_files[i] << endl;
      return(false);
    }
    m_handlers.push_back(handler);
  }
  return(true);
}

//--------------------------------------------------------
// Procedure: handle

bool SplitHandlerBundle::handle()
{
  for(unsigned int i=0; i<m_alog_files.size(); i++) {
    bool ok = handle(i);
    if(!ok) 
      return(false);
  }
  return(true);
}

//--------------------------------------------------------
// Procedure: handle

bool SplitHandlerBundle::handle(unsigned int ix)
{
  if((ix >= m_alog_files.size()) || (ix >= m_handlers.size()))
    return(false);
  
  bool result = m_handlers[ix].handle();

  return(result);
}


//--------------------------------------------------------
// Procedure: clear()

void SplitHandlerBundle::clear()
{
  m_alog_files.clear();
  m_handlers.clear();
}

