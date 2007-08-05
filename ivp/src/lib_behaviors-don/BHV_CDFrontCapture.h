#ifndef DingWang_CD_CapFro_H
#define DingWang_CD_CapFro_H

#include "tnt/vec.h"
#include "tnt/fmat.h"
#include "tnt/inv.h"
#include <math.h>

#include "IvPBehavior.h"

template <class T>
class DingWang_CD_CapFro
{
 private:
  T Near_bound, Far_bound, Temperature_threshold, Starting_azimuth;//Near_bound is the bound near the AUV starting location; Far_bound is the bound farther from the AUV starting location.
  int N_points;
  Vector<T> Temperature; // latest measurement result
  T W_azimuth;//the azimuth of W axis.
  Vector<T> W; // latest measurement location - W direction
  int sample_index; // from 1 to N_points
  T near_far; // indicate AUV is located between the near bound and the fronts (-1) or between the fronts and the far bound (+1). if it's negaive, AUV should go to far; if it's positive, AUV should go to near
  T Current_W; // current position in W direction
  T original_T; // original temperature
  T Direction; // -1 go to far; 1 go to near
 public:
  DingWang_CD_CapFro(){}
  void Initialize(const T, const T, const T, const int, const T, const T);
  void Set_measurement(const T, const T, const T);
  T Get_waypoint(void); // returning back -1 means going down; 1 means going up 
};

class BHV_CDFrontCapture : public IvPBehavior {
public:
  BHV_CDFrontCapture(IvPDomain);
  ~BHV_CDFrontCapture() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);

protected:
  double desired_heading;
  double basewidth;
  double peakwidth;

  int points;
  double w_azimuth,near,far,threshold;
  bool initialized;

  DingWang_CD_CapFro<double> CapFro;
  
  void initialize(double,double,double,int,double,double);
  void set_measurement(double,double,double);
  double get_azimuth();
};
#endif
