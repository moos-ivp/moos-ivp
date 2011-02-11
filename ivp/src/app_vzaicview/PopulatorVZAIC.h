/*****************************************************************/
/*    NAME: M.Benjamin, H.Schmidt, J.Leonard                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PopulatorVZAIC.h                                     */
/*    DATE: May 5th, 2010                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef POPULATOR_VZAIC_HEADER
#define POPULATOR_VZAIC_HEADER

#include <string>
#include <vector>
#include "ZAIC_Vector.h"
#include "IvPDomain.h"

class PopulatorVZAIC
{
 public:
  PopulatorVZAIC();
  ~PopulatorVZAIC() {};
  
  bool readFile(std::string filename);
  
  ZAIC_Vector *buildZAIC();

 protected:
  IvPDomain  m_domain;
  double     m_minutil;
  double     m_maxutil;

  std::vector<double> m_domain_vals;
  std::vector<double> m_range_vals;

};
#endif 

//--------------------------------------------
//  ivpdomain = foobar,0,100,101
//  domain = 10,20,30,40,50
//  range  = 2,4,6,8,10
//  minutil = 0
//  maxutil = 100
