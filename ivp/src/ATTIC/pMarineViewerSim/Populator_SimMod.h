/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_SimMod.H                                   */
/*    DATE: Mar 6th 2005 Kath's Mom comes to Boston              */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
 
#ifndef POPULATOR_SIMMOD_HEADER
#define POPULATOR_SIMMOD_HEADER

#include "IB_HelmIvP.h"
#include "SIM.h"
#include "SSR.h"
#include "IvPBehavior.h"

class Populator_SimMod {
public:
  Populator_SimMod(SSR*);
  ~Populator_SimMod();  

  void  populate();

  SIM*        getSIM() {return(sim);};
  IB_HelmIvP* getIB()  {return(info_buffer);};

protected:
  void  handle_helms();

  IvPDomain  build_domain(std::string);
  void       build_pids(SHelmIvP*, std::string);

protected:
  SSR*        ssr;
  SIM*        sim;
  IB_HelmIvP* info_buffer;
};
#endif 





