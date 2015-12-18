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
#include "IvPProblem_v3.h"
 
class PopulatorIPP
{
public:
  PopulatorIPP() {m_ivp_problem=0;}
  ~PopulatorIPP() {}
  
  bool populate(std::string filename, int alg=0);
  void setVerbose(bool v) {m_verbose=v;}

  IvPProblem* getIvPProblem() {return(m_ivp_problem);}

protected:
  bool handleLine(std::string);
  
protected:
  IvPProblem* m_ivp_problem;
  bool        m_verbose;
};
#endif
