/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: AOF_HArrayTurn.h                               */
/*    DATE: 23 July 05                                     */
/***********************************************************/
 
#ifndef AOF_HARRAYTURN_HEADER
#define AOF_HARRAYTURN_HEADER

#include "AOF.h"

#define LR  1
#define OPT 2


class IvPDomain;
class AOF_HArrayTurn: public AOF {
 public:
  AOF_HArrayTurn(IvPDomain, double,double);
  ~AOF_HArrayTurn() {};

public:    
  double evalBox(const IvPBox*) const;    // virtual defined
  double lrmetric(double) const;
 

protected:

  
  double leftabs,rightabs,hwidth,osCourse;

  double crsBase;
  double crsDelta;
};

#endif
