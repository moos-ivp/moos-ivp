/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: SplitHandlerBundle.h                                 */
/*    DATE: Feb 5th, 2015                                        */
/*****************************************************************/

#ifndef ALOG_SPLIT_HANDLER_BUNDLE_HEADER
#define ALOG_SPLIT_HANDLER_BUNDLE_HEADER

#include <vector>
#include <string>
#include "SplitHandler.h"

class SplitHandlerBundle
{
 public:
  SplitHandlerBundle() {};
  ~SplitHandlerBundle() {};

  void addALogFile(std::string);
  bool checkALogFiles();

  bool handle();
  void clear();

 protected:
  
  bool handle(unsigned int);

  std::vector<std::string>  m_alog_files;
  std::vector<SplitHandler> m_handlers;
};

#endif
