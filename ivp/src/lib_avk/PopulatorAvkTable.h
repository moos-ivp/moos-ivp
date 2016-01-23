/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PopulatorAvkTable.h                                  */
/*    DATE: Mar 26th, 2015                                       */
/*****************************************************************/

#ifndef POPULATOR_AVK_TABLE_HEADER
#define POPULATOR_AVK_TABLE_HEADER

#include <string>
#include "AVK_Table.h"

class PopulatorAvkTable
{
 public:
  PopulatorAvkTable() {m_verbose=false;};
  ~PopulatorAvkTable() {};
  
  bool setAvkFile(std::string);

  bool setVerbose(bool v) {m_verbose=v; return(true);};
  bool populate();

  AVK_Table  getAvkTable() const {return(m_avk_table);};

 protected:
  std::string m_avk_file;

  AVK_Table m_avk_table;

  bool m_verbose;
};

#endif 
