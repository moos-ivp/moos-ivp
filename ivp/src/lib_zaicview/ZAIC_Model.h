/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ZAIC_Model.h                                         */
/*    DATE: May 9th, 2016                                        */
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

#ifndef ZAIC_MODEL_HEADER
#define ZAIC_MODEL_HEADER

#include "IvPFunction.h"

class ZAIC_Model 
{
 public:
  ZAIC_Model() {}
  virtual ~ZAIC_Model() {};

  // Virtual function to be overloaded
  virtual IvPFunction* getIvPFunction() {return(0);}
  virtual void setDomain(unsigned int)  {};
  virtual bool setParam(std::string, std::string) {return(false);}
  virtual void moveX(double amt) {};
  virtual void currMode(int) {};

  
  // Getters
  IvPDomain  getIvPDomain();
};
#endif 





