/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: BHV_TestFailure.h                                    */
/*    DATE: October 21st 2011                                    */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef BHV_TEST_FAILURE_HEADER
#define BHV_TEST_FAILURE_HEADER

#include "IvPBehavior.h"

class BHV_TestFailure : public IvPBehavior {
public:
  BHV_TestFailure(IvPDomain);
  ~BHV_TestFailure() {}
  
  void  onCompleteState();
  bool  setParam(std::string, std::string);

 protected:
  bool   m_failure_crash;
  bool   m_failure_burn;
  double m_failure_burn_time;
};

#endif



















