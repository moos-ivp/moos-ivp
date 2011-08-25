/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_NavPlot.h                                  */
/*    DATE: May 14th, 2008                                       */
/*****************************************************************/

#ifndef POPULATOR_GRID_PLOT_HEADER
#define POPULATOR_GRID_PLOT_HEADER

#include <string>
#include "GridPlot.h"

class Populator_GridPlot 
{
 public:
  Populator_GridPlot() {};
  ~Populator_GridPlot() {};
  
  void  populate(std::string);
  void  setVname(std::string s)  {m_vname = s;};
  int   size()                   {return(m_gridplots.size());};

  GridPlot    getGridPlot(int ix=0);
  std::string getGridPlotLabel(int ix=0);
  
  void  print();

protected:
  void handleConfigEntry(double, const std::string&);
  void handleDeltaEntry(double, const std::string&);

protected:
  std::string              m_vname;

  // A GridPlot is created and maintained for each "GRID_CONFIG" 
  // entry found in the .alog file.
  std::vector<GridPlot>    m_gridplots;
};
#endif 








