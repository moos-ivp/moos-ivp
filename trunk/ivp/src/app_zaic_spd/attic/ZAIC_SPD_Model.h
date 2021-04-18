/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_SPD_Model.h                                     */
/*    DATE: May 9th, 2016                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ZAIC_SPD_MODEL_HEADER
#define ZAIC_SPD_MODEL_HEADER

#include <string>
#include "IvPDomain.h"
#include "ZAIC_Model.h"
#include "ZAIC_SPD.h"

class ZAIC_SPD_Model : public ZAIC_Model
{
 public:
  ZAIC_SPD_Model();
  ~ZAIC_SPD_Model() {};

 public: // Virtual function overloaded
  IvPFunction *getIvPFunction();
  
 public: // Editing functions
  void  moveX(double amt);
  void  setDomain(unsigned int);

  // Change the Edit Mode
  void  currMode(int);

  // Getters
  double getMedVal();
  double getLowVal();
  double getHghVal();
  double getLowValUtil();
  double getHghValUtil();

 protected:
  ZAIC_SPD*  m_zaic_spd;
  int        m_curr_mode;
};
#endif 
