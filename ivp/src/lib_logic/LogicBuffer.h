/*****************************************************************/
/*    NAME: Benjamin, Schmidt, Leonard                           */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: LogicBuffer.H                                        */
/*    DATE: Sep 19th 2010 On the flight to IPAM                  */
/*****************************************************************/

#ifndef LOGIC_BUFFER_HEADER
#define LOGIC_BUFFER_HEADER

#include <string>
#include <vector>
#include "InfoBuffer.h"
#include "LogicCondition.h"

class LogicBuffer {
public:
  LogicBuffer();
  ~LogicBuffer();

public:
  bool addNewCondition(const std::string&);

  bool updateInfoBuffer(const std::string& moosvar,
			const std::string& value);

  bool updateInfoBuffer(const std::string& moosvar,
			double value);

  bool checkConditions();

  unsigned int size() {return(m_logic_conditions.size());};

  std::vector<std::string> getAllVars();

protected:
  std::vector<LogicCondition> m_logic_conditions;
  
  InfoBuffer *m_info_buffer;
};
#endif
