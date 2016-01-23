/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PopulatorTurnSeq.h                                   */
/*    DATE: Nov 11th, 2014                                       */
/*****************************************************************/

#ifndef POPULATOR_AVD_TABLE_HEADER
#define POPULATOR_AVD_TABLE_HEADER

#include <string>
#include "AVD_Table.h"

class PopulatorAvdTable
{
 public:
  PopulatorAvdTable();
  ~PopulatorAvdTable() {};
  
  bool setVerbose(bool v) {m_verbose=v; return(true);};
  bool addTSQFile(std::string);

  bool setVMin(std::string);
  bool setVMax(std::string);
  bool setVGap(std::string);
  bool setHMin(std::string);
  bool setHMax(std::string);
  bool setHGap(std::string);

  bool initialize();
  bool fill();
  bool populate();

  AVD_Table  getAvdTable() const {return(m_avd_table);};

 protected:
  AVD_Table m_avd_table;
  double    m_vmin;
  double    m_vmax;
  double    m_vgap;
  double    m_hmin;
  double    m_hmax;
  double    m_hgap;
  bool      m_verbose;

  std::vector<std::string> m_tsq_files;
};

#endif 
