/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_HDG_Model.h                                     */
/*    DATE: May 10th, 2016                                       */
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

#ifndef ZAIC_HDG_MODEL_HEADER
#define ZAIC_HDG_MODEL_HEADER

#include <string>
#include "IvPDomain.h"
#include "ZAIC_Model.h"
#include "ZAIC_HDG.h"

class ZAIC_HDG_Model : public ZAIC_Model
{
 public:
  ZAIC_HDG_Model();
  ~ZAIC_HDG_Model() {};

 public: // Virtual functions overloaded
  IvPFunction *getIvPFunction();
  void  setDomain(unsigned int);
  void  moveX(double amt);
  void  currMode(int);

 public:
  int getCurrMode() const {return(m_curr_mode);}
  
  // Getters
  double getSummit();
  double getLowDelta();
  double getHighDelta();
  double getLowDeltaUtil();
  double getHighDeltaUtil();

  double getLowMinUtil();
  double getHighMinUtil();
  double getMaxUtil();

 protected:
  ZAIC_HDG* m_zaic_hdg;
  int       m_curr_mode;
};
#endif 




