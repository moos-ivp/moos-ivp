/*****************************************************************/
/*    NAME: Andrew Shafer                                        */
/*    ORGN: MIT                                                  */
/*    FILE: SearchPassValue.h                                    */
/*    DATE: 23 Jan 2008                                          */
/*****************************************************************/
 
#ifndef AOF_SEARCHPASSVALUE_HEADER
#define AOF_SEARCHPASSVALUE_HEADER

#include <string>

class SearchPassValue {
 public:
  SearchPassValue();
  ~SearchPassValue() {};

public:
  double evalValue(double time) const;
  
protected:
  
};

#endif

