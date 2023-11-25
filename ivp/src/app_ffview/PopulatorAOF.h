/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PopulatorAOF.h                                       */
/*    DATE: Jan 14th 2006                                        */
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

#ifndef POPULATOR_AOF_HEADER
#define POPULATOR_AOF_HEADER

#include <string>
#include "IvPDomain.h"
#include "AOF.h"

class PopulatorAOF
{
public:
  PopulatorAOF()  {define_mode=0; aof=0;}
  ~PopulatorAOF() {}

  AOF* populate(std::string filename);

protected:
  bool handleLine(std::string);
  bool handleEntry(std::string, std::string, std::string);

  bool buildDomain(std::string);
  
protected:
  int        define_mode;
  AOF*       aof;
  IvPDomain  domain;
};
#endif















