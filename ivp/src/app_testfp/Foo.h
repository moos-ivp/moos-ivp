/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Foo.h                                                */
/*    DATE:                                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef FOO_HEADER
#define FOO_HEADER

#include "Bar.h"

class Foo
{
 public:
  Foo() {
    bar2 = new Bar;
    bar2->owner = this;
    bar2->bar_add = &Foo::add;
  }
  virtual ~Foo() {};

  void doit() {bar2->reallydoit(10, 3);};

  int  add(int a, int b) {return(4*a+b);};

  Bar *bar2;
};

#endif 


