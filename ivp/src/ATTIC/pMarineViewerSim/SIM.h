/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SIM.H                                                */
/*    DATE: Mar 6th 2005 Lanfen comes to Boston for 3 mo.s       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef SIM_HEADER
#define SIM_HEADER

#include <vector>
#include "SHelmIvP.h"
#include "IB_HelmIvP.h"

class SIM {
public:
  SIM();
  ~SIM();

  void   click() {clickDecide(); clickAct();};
  void   clickDecide();
  void   clickAct();

  void   setInfoBuffer(IB_HelmIvP *ib);
  void   resetInfoBuffer();
  void   addHelm(SHelmIvP* h)          {helms.push_back(h);};
  double getCurrTime()                 {return(curr_time);};
  int    getHelmCount()                {return(helms.size());};

  std::string       getInfoBHV(int helm_ix);
  std::vector<IMG*> getAllImages();
  std::vector<IMG*> getOneImageSet(int ix);
  IvPFunction*      getOF(int ix);

  void   courseDelta(int, double);
  void   speedDelta(int, double);

public:
  std::vector<SHelmIvP*> helms;

  IB_HelmIvP* info_buffer;
  IB_HelmIvP  save_buffer;
  double      click_time;
  double      curr_time;

};
#endif





