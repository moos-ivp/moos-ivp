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
  SearchPassValue(std::string config="");
  ~SearchPassValue() {};

public:
  double evalValue(double time) const;
  bool   isConfigured() const {return(configured);};
  
  std::string  toString();

protected:
  bool   configured;

  double max_value;
  std::string config_string;

};

#endif
