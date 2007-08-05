#ifndef DingWang_Yoyo_CapFro_H
#define DingWang_Yoyo_CapFro_H

#include "tnt/vec.h"
#include "tnt/fmat.h"
#include "tnt/inv.h"
#include <math.h>

#include "IvPBehavior.h"

template <class T>
class DingWang_YoYo_CapFro
{
 private:
  T Near_bound, Far_bound, Upper_bound, Lower_bound, Temperature_threshold, Starting_azimuth;
  //Near_bound is the bound near the AUV starting location; Far_bound is the bound farther from the AUV starting location; Upper_bound and Lower_bound are the same as those in vertical yoyo control.
  Vector<T> Temperature, Mean_Tp; // latest measurement result
  Vector<int> Temperature_N;
  T W_azimuth;//the azimuth of W axis.
  Vector<T> W; // latest measurement location - W direction
  T near_far; // indicate AUV is located between the near bound and the fronts (-1) or between the fronts and the far bound (+1). if it's negaive, AUV should go to far; if it's positive, AUV should go to near
  T Current_W; // current position in W axis
  Vector<T> original_T; // original temperature
  T Current_going_up_down; // indicate AUV is going up or down. -1 down, 1 up.
  T original_going_up_down; 
  T Direction; // -1 go far; 1 go near
  int Segments, Segment_index;
 public:
  DingWang_YoYo_CapFro(){}
  void Initialize(const T, const T, const T,const T,const T, const int,const int, const T, const T);
  void Set_measurement(const T, const T, const T, const T, const T);
  T Get_waypoint(void); // returning back -1 means going down; 1 means going up 
};

class BHV_YoYoFrontCapture : public IvPBehavior {
public:
  BHV_YoYoFrontCapture(IvPDomain);
  ~BHV_YoYoFrontCapture() {};
  
  IvPFunction* produceOF();
  bool         setParam(std::string, std::string);

protected:
  double desired_heading;
  double basewidth;
  double peakwidth;

  int points,segments,segment_index;
  double w_azimuth,near,far,threshold,upper,lower;
  bool initialized;

  DingWang_YoYo_CapFro<double> CapFro;
  
  void initialize(double,double,double,double,double,int,int,double,double);
  void set_measurement(double,double,double,double,int);
  double get_azimuth();

};
#endif
