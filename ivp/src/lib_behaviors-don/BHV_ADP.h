#ifndef ADP_H
#define ADP_H

#include "tnt/vec.h"
#include "tnt/fmat.h"
#include "tnt/inv.h"

#include "IvPBehavior.h"

class BHV_ADP : public IvPBehavior {
public:
  BHV_ADP(IvPDomain);
  ~BHV_ADP() {};
  
  IvPFunction* onRunState();
  bool         setParam(std::string, std::string);

protected:
  double desired_heading;
  double basewidth;
  double peakwidth;

};

template <class T>
class ADP
{
 private:
  Vector<T> AUV_grid_r, AUV_grid_z, L, control_number, stage_info, state_part1, Sound_speed;	
  Fortran_matrix<T> samples_r_all, samples_z_all, samples_info, C, Q;
  Fortran_matrix<int> Path_all;
  int obs_n, u, control_combi_i, state_part2, sample_index, stage;
 public:
  ADP(){}
  void Initialize();
  void Set_measurement(const T);
  void Get_waypoint(Vector<T> &, Vector<T> &); 
};
#endif
