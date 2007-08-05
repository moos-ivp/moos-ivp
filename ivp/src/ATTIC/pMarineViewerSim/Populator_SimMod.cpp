/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_SimMod.h                                   */
/*    DATE: Nov 7th 2003 6 die in Iraq Copter crash              */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include <string>
#include <assert.h>
#include "Populator_SimMod.h"
#include "Populator_BehaviorSet.h"
#include "IvPDomain.h"
#include "SHelmIvP.h"
#include "MBUtils.h"
#include "ScalarPID.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

Populator_SimMod::Populator_SimMod(SSR *g_ssr)
{
  ssr      = g_ssr;
  sim      = 0;
  info_buffer = 0;
}

//-------------------------------------------------------------
// Procedure: Destructor

Populator_SimMod::~Populator_SimMod()
{
  if(info_buffer) delete(info_buffer);
  if(sim)      delete(sim);
}

//-------------------------------------------------------------
// Procedure: populate

void Populator_SimMod::populate()
{
  if(sim) delete(sim);
  if(info_buffer) delete(info_buffer);

  sim         = new SIM;
  info_buffer = new IB_HelmIvP;

  handle_helms();

  sim->setInfoBuffer(info_buffer);
}

//-------------------------------------------------------------
// Procedure: handle_helms

void Populator_SimMod::handle_helms()
{
  assert(info_buffer && sim);

  int i, j;
  int h_cnt = ssr->getCount();

  for(i=0; i<h_cnt; i++) {
    string h_nam = ssr->get_h_nam(i);
    string h_dom = ssr->get_h_dom(i);
    string h_bhv = ssr->get_h_bhv(i);
    string h_lon = ssr->get_h_lon(i);
    string h_lat = ssr->get_h_lat(i);
    string h_crs = ssr->get_h_crs(i);
    string h_spd = ssr->get_h_spd(i);
    string h_pid = ssr->get_h_pid(i);
    string h_ver = ssr->get_h_ver(i);

    info_buffer->set_val(h_nam, "NAV_X", atof(h_lon.c_str()));
    info_buffer->set_val(h_nam, "NAV_Y", atof(h_lat.c_str()));
    info_buffer->set_val(h_nam, "NAV_HEADING", atof(h_crs.c_str()));
    info_buffer->set_val(h_nam, "NAV_SPEED", atof(h_spd.c_str()));

    IvPDomain ivp_domain  = build_domain(h_dom);

    Populator_BehaviorSet bset_populator(ivp_domain, info_buffer);

    BehaviorSet* bset = bset_populator.populate(h_bhv);

    if(bset) {
      for(int i=0; i<bset->getCount(); i++)
	bset->getBehavior(i)->setParam("us", h_nam);
      bset->print();
    }
    if(!bset) { 
      cout << "Populator_SimMod: ";
      cout << "Exiting due to NULL Behavior Set" << endl;
      exit(0);
    }

    SHelmIvP* new_helm = new SHelmIvP(ivp_domain);

    new_helm->setBehaviorSet(bset);
    new_helm->setInfoBuffer(info_buffer);
    new_helm->setOwnship(h_nam);
    new_helm->setVerboseFlag(h_ver == "true");
    new_helm->postInitialMessages();
    build_pids(new_helm, h_pid);

    sim->addHelm(new_helm);
  }
}

//-------------------------------------------------------------
// Procedure: build_domain
//      Note: 

IvPDomain Populator_SimMod::build_domain(string domain_descriptor)
{
  IvPDomain domain;

  domain_descriptor = stripBlankEnds(domain_descriptor);

  vector<string> svector = parseString(domain_descriptor, ',');
  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
    string dstr = stripBlankEnds(svector[i]);
    vector<string> svector2 = parseString(dstr, ':');
    int vsize2 = svector2.size();
    assert(vsize2==4);
    string dname   = svector2[0];
    string dlow    = svector2[1];
    string dhigh   = svector2[2];
    string dpoints = svector2[3];
    double f_dlow    = atof(dlow.c_str());
    double f_dhigh   = atof(dhigh.c_str());
    int    i_dpoints = atoi(dpoints.c_str());

    domain.addDomain(dname.c_str(), f_dlow, f_dhigh, i_dpoints);
  }
  return(domain);
}

//-------------------------------------------------------------
// Procedure: build_pid
//      Note: 

void Populator_SimMod::build_pids(SHelmIvP *helm, string pid_descriptor)
{
  pid_descriptor = stripBlankEnds(pid_descriptor);

  PIDEngine pengine;
  pengine.setInfoBuffer(info_buffer);
  pengine.setOwnship(helm->getOwnship());

  vector<string> svector = parseString(pid_descriptor, ',');
  int vsize = svector.size();
  for(int i=0; i<vsize; i++) {
    string dstr = stripBlankEnds(svector[i]);
    vector<string> svector2 = parseString(dstr, ':');
    int vsize2 = svector2.size();
    assert(vsize2==5);
    string pname   = svector2[0];
    double f_kp    = atof(svector2[1].c_str());
    double f_kd    = atof(svector2[2].c_str());
    double f_ki    = atof(svector2[3].c_str());
    double f_lim   = atof(svector2[4].c_str());

    ScalarPID new_pid(f_kp, f_kd, f_ki, f_lim, 100);

    if(pname == "yaw")
      pengine.setPID(0, new_pid);
    if(pname == "speed")
      pengine.setPID(1, new_pid);
    if(pname == "depth")
      pengine.setPID(2, new_pid);
  }
  helm->setPIDEngine(pengine);
}





