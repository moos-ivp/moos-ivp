/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: UnitTester.h                                         */
/*    DATE: Nov 5th, 2018                                        */
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

#ifndef A_UNIT_TESTER_HEADER
#define A_UNIT_TESTER_HEADER

#include <vector>
#include <string>
#include <set>

class UnitTester
{
 public:
  UnitTester();
  ~UnitTester() {}

  bool addTestFile(std::string);
  bool addVerbosity() {m_verbosity++; return(true);}
  bool runUnitTests()
    ;
  void printReport();

protected:
  bool   handleUnitTestFile(std::string);
  std::string executeOneTest(std::string);
  
  bool   actualSatExpected(std::string, std::string);
  
protected:
  std::vector<std::string> m_test_files;
  
  unsigned int m_verbosity;
};

#endif









