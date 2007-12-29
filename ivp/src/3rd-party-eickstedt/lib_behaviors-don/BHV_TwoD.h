#ifndef TwoD_H
#define TwoD_H

#include "tnt/vec.h"
#include "tnt/fmat.h"
#include "tnt/inv.h"
#include "tnt/lu.h"
#include <math.h>

#include "IvPBehavior.h"

template <class T>
class DingWang_2D_Gradient
{
 private:
  T Gradient_azimuth;
  Vector<T> Temperature; // latest measurement result
  Vector<T> X; // latest measurement location - East dirction
  Vector<T> Y; // latest measurement location - North dirction
  int sample_index; // how many samples we have
  T Azimuth; // current azimuth
 public:
  DingWang_2D_Gradient(){}
  void Initialize(const T);
  void Set_measurement(const T, const T, const T);
  T Get_gradient_azimuth(void); 
};



class BHV_TwoD : public IvPBehavior {
public:
  BHV_TwoD(IvPDomain);
  ~BHV_TwoD() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  double desired_heading,length,period,amplitude;
  double basewidth;
  double peakwidth;
  bool   initialized;

  DingWang_2D_Gradient<double> TwoD_Grad;
   
  void initialize(double);
  void set_measurement(double,double,double);
  double get_azimuth();

  double last_x,last_y, last_ss;
};
#endif
