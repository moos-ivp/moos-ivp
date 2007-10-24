/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: AOF_ArrayTurn.h                                */
/*    DATE: 23 July 05                                     */
/***********************************************************/
 
#ifndef AOF_ARRAYTURN_HEADER
#define AOF_ARRAYTURN_HEADER

#include "AOF.h"

#define LR  1
#define OPT 2


class IvPDomain;
class AOF_ArrayTurn: public AOF {
 public:
  AOF_ArrayTurn(IvPDomain, double,double);
  ~AOF_ArrayTurn() {};

public:    
  double evalBox(const IvPBox*) const;    // virtual defined
  double lrmetric(double) const;
 

protected:

  
  double leftabs,rightabs,hwidth,osCourse;

  double crsBase;
  double crsDelta;
};

#endif
