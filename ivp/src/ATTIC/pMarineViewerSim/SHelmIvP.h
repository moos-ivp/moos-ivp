/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SHelmIvP.h                                           */
/*    DATE: Mar 6th, 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef SHELMIVP_HEADER
#define SHELMIVP_HEADER

#include "HelmEngine.h"
#include "IvPDomain.h"
#include "IvPProblem.h"
#include "BehaviorSet.h"
#include "IB_HelmIvP.h"
#include "ScalarPID.h"
#include "IMG.h"
#include "PIDEngine.h"

class SHelmIvP {
public:
  SHelmIvP(IvPDomain);
  ~SHelmIvP();

  bool   click(double);
  void   setInfoBuffer(IB_HelmIvP*);
  void   setBehaviorSet(BehaviorSet*);
  void   setPIDEngine(PIDEngine);
  void   setOwnship(std::string os_name);
  void   postInitialMessages();
  void   postBehaviorMessages();
  void   headingDelta(double);
  void   speedDelta(double);
  IMG*   getIMG(int);
  IvPFunction* getOF(int);

  void   setVerboseFlag(bool v) {verbose = v;}
  int    getBehaviorCount()     {return(bhv_set->getCount());}
  double getDesiredRudder()     {return(hengine->getDesiredRudder());}
  double getDesiredThrust()     {return(hengine->getDesiredThrust());}
  std::string getOwnship()      {return(hengine->getOwnship());}
  std::string getInfoBHV()      {return(hengine->getInfoBHV());}


protected:
  IvPDomain    ivp_domain;
  BehaviorSet* bhv_set;
  HelmEngine*  hengine;
  IB_HelmIvP*  info_buffer;
  bool         verbose;
};

#endif





