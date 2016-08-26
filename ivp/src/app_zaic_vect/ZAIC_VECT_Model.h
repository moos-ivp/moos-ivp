/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_VECT_Model.h                                    */
/*    DATE: May 16th, 2016                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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
