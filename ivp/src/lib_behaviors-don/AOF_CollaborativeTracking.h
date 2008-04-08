/***********************************************************/
/*    NAME: Don Eickstedt                                  */
/*    FILE: AOF_CollaborativeTracking.h                    */
/*    DATE: 23 July 2007                                   */
/***********************************************************/
 
#ifndef AOF_COLLABORATIVETRACKING_HEADER
#define AOF_COLLABORATIVETRACKING_HEADER

#include "AOF.h"

class IvPDomain;

class AOF_CollaborativeTracking: public AOF {
 public:
  AOF_CollaborativeTracking(IvPDomain);
  ~AOF_CollaborativeTracking() {};

public:    
  double evalBox(const IvPBox*) const;    // virtual defined
  double Uncertainty(double,double,double,double,double,double) const;
  bool   setParam(const std::string&, double);

protected:

  double osx,csx;
  double osy,csy;
  double targx,targy,sigmapos,sigmatheta;

  double crsBase;
  double crsDelta;

  int crs_ix;
};

#endif
