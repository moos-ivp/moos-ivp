/***********************************************************/
/*    NAME: Henrik Schmidt                                  */
/*    FILE: AOF_MinimizeNoise.h                    */
/*    DATE: 8 May 2008                                   */
/***********************************************************/
 
#ifndef AOF_MINIMIZENOISE_HEADER
#define AOF_MINIMIZENOISE_HEADER

#include "AOF.h"

class IvPDomain;

class AOF_MinimizeNoise: public AOF {
 public:
  AOF_MinimizeNoise(IvPDomain);
  ~AOF_MinimizeNoise() {};

public:    
  double evalBox(const IvPBox*) const;    // virtual defined
  double Uncertainty(double,double,double,double,double,double) const;
  bool   setParam(const std::string&, double);
  int   setNoiseBin(double, double);
  void resetNoise();

protected:

  double osx, osy, targx, targy;

  std::vector<double> m_bearing;
  std::vector<double> m_noise;

  double getNoiseLevel(double);

  int crs_ix;
};

#endif
