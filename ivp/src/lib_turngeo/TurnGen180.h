/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: TurnGen180.h                                         */
/*    DATE: July 27th, 2020                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef TURN_GEN_180_HEADER
#define TURN_GEN_180_HEADER

#include <vector>
#include <string>
#include "TurnGenerator.h"

class TurnGen180 : public TurnGenerator
{
public:
  TurnGen180();
  ~TurnGen180() {};
  
  void generate();
  
private:

};

#endif 
