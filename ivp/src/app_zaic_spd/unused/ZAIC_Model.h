/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ZAIC_Model.h                                         */
/*    DATE: May 9th, 2016                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef ZAIC_MODEL_HEADER
#define ZAIC_MODEL_HEADER

#include "IvPFunction.h"

class ZAIC_Model 
{
 public:
  ZAIC_Model() {}
  ~ZAIC_Model() {};

  // Virtual function to be overloaded
  virtual IvPFunction* getIvPFunction() {return(0);}

  // Getters
  IvPDomain  getIvPDomain();
};
#endif 
