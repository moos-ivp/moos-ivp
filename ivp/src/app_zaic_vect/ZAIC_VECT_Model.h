/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: ZAIC_VECT_Model.h                                    */
/*    DATE: May 16th, 2016                                       */
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

#ifndef ZAIC_VECT_MODEL_HEADER
#define ZAIC_VECT_MODEL_HEADER

#include <string>
#include "IvPDomain.h"
#include "ZAIC_Model.h"
#include "ZAIC_Vector.h"

class ZAIC_VECT_Model : public ZAIC_Model
{
 public:
  ZAIC_VECT_Model()  {m_zaic_vect=0; m_tolerance=0;}
  ~ZAIC_VECT_Model() {};

 public: // Virtual function overloaded
  IvPFunction *getIvPFunction();

  // Seters
  void setZAIC(ZAIC_Vector*);
  void setTolerance(double);
  void modTolerance(double);
  
  // Getters
  double getMinUtil();
  double getMaxUtil();
  double getTolerance();

  unsigned int getTotalPieces();
  
 protected:
  ZAIC_Vector* m_zaic_vect;

  double m_tolerance;
};
#endif 





