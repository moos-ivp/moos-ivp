/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_HDG_Model.h                                     */
/*    DATE: May 10th, 2016                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
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
