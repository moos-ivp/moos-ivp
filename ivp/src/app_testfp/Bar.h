/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Bar.h                                                */
/*    DATE:                                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef BAR_HEADER
#define BAR_HEADER

#include <iostream>
#include "Foo.h"

class Foo;

class Bar
{
 public:
  Bar() {};
  virtual ~Bar() {};

  int  (Foo::*bar_add)(int,int);

  void reallydoit(int x, int y) {
    std::cout << "x:" << x << " y:" << y << std::endl;
    int result = (owner->*bar_add)(x,y);
    std::cout << "result: " << result << std::endl;
  }

  Foo* owner;
};

#endif 


