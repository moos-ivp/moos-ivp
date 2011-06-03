/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: PopulatorVZAIC.h                                     */
/*    DATE: May 5th, 2010                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
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

