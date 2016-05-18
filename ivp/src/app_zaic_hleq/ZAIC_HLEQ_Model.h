/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_HLEQ_Model.h                                    */
/*    DATE: May 9th, 2016                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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
