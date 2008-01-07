/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Timutif.h                                            */
/*    DATE: Dec 25th, 2006                                       */
/*****************************************************************/
 
#ifndef AOF_TIMUTIF_HEADER
#define AOF_TIMUTIF_HEADER

#include <string>

class Timutif {
 public:
  Timutif(std::string config="");
  ~Timutif() {};

public:
  double evalUtility(double time) const;
  bool   isConfigured() const {return(configured);};
  
  std::string  toString();

protected:
  bool   configured;
  int    util_type;

  double time_low;
  double time_med;
  double time_high;
  double util_med;
};

#endif

