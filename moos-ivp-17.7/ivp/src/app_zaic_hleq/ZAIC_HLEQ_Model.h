/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_HLEQ_Model.h                                    */
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

#ifndef ZAIC_HLEQ_MODEL_HEADER
#define ZAIC_HLEQ_MODEL_HEADER

#include <string>
#include "IvPDomain.h"
#include "ZAIC_Model.h"
#include "ZAIC_LEQ.h"
#include "ZAIC_HEQ.h"

class ZAIC_HLEQ_Model : public ZAIC_Model
{
 public:
  ZAIC_HLEQ_Model();
  ~ZAIC_HLEQ_Model() {};

 public: // Virtual function overloaded
  IvPFunction *getIvPFunction();
  void  setDomain(unsigned int);
  
 public: // Editing functions
  void  moveX(double amt);

  // Change the Edit Mode
  void  currMode(int);
  void  toggleHLEQ() {m_leq_mode = !m_leq_mode;}

  // Getters
  double getSummit();
  double getSummitDelta();
  double getBaseWidth();
  double getMinUtil();
  double getMaxUtil();

 protected:
  ZAIC_LEQ*  m_zaic_leq;
  ZAIC_HEQ*  m_zaic_heq;
  int        m_curr_mode;
  bool       m_leq_mode;

};
#endif 




