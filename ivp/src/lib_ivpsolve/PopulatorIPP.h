/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PopulatorIPP.h                                       */
/*    DATE: Nov 24th 2014                                        */
/*****************************************************************/

#ifndef POPULATOR_IPP_HEADER
#define POPULATOR_IPP_HEADER

#include <string>
#include "IvPProblem.h"
 
class PopulatorIPP
{
public:
  PopulatorIPP() {m_ivp_problem=0; m_grid_override_size=0;}
  ~PopulatorIPP() {}
  
  bool populate(std::string filename, int alg=0);

  void setVerbose(bool v)          {m_verbose=v;}
  IvPProblem* getIvPProblem()      {return(m_ivp_problem);}

  void setGridOverrideSize(int v)  {m_grid_override_size=v;}
  
protected:
  bool handleLine(std::string);
  void overrideGrid(IvPFunction*);

  
protected:
  IvPProblem* m_ivp_problem;
  bool        m_verbose;

  int         m_grid_override_size;

};
#endif
