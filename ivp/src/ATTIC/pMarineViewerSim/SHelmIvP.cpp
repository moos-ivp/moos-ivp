/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SHelmIvP.cpp                                         */
/*    DATE: Mar 6th, 2005                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
 
#include <iostream>
#include "SHelmIvP.h"
#include "MBUtils.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

SHelmIvP::SHelmIvP(IvPDomain g_ivp_domain)
{
  ivp_domain  = g_ivp_domain;
  bhv_set     = 0;
  info_buffer = 0;
  hengine     = new HelmEngine(g_ivp_domain);
  hengine->setFLogFile("./", "log");
  verbose     = true;
}

//-----------------------------------------------------------
// Procedure: Destructor

SHelmIvP::~SHelmIvP()
{
  if(bhv_set) delete(bhv_set);
}

//-----------------------------------------------------------
// Procedure: click
//      Note:

bool SHelmIvP::click(double curr_time) 
{
  bool result = hengine->determineNextDecision(bhv_set, curr_time);
  postBehaviorMessages();
  if(verbose)
    hengine->printHelmSummary();
  hengine->logFunctionsToFile("verbose");
  return(result);
}

//------------------------------------------------------------
// Procedure: postBehaviorMessages()

void SHelmIvP::postBehaviorMessages()
{
  if(!bhv_set) return;

  int bhv_cnt = bhv_set->getCount();
  for(int i=0; i < bhv_cnt; i++) {
    vector<VarDataPair> mvector = bhv_set->getMessages(i);
    int msize = mvector.size();
    for(int j=0; j<msize; j++) {
      VarDataPair msg = mvector[j];
      
      string var   = stripBlankEnds(msg.get_var());
      string sdata = stripBlankEnds(msg.get_sdata());
      double ddata = msg.get_ddata();
      string ownship = hengine->getOwnship();

      if(sdata != "") {
	info_buffer->set_val(ownship, var, sdata);
	if(verbose)
	  cout << ownship << " " << var << " " << sdata << endl; 
     }
      else {
	info_buffer->set_val(ownship, var, ddata);
	if(verbose)
	  cout << ownship << " " << var << " " << ddata << endl; 
      }
    }
  }
}


//------------------------------------------------------------
// Procedure: postIntialMessages()

void SHelmIvP::postInitialMessages()
{
  cout << "Posting initial Messages() " << endl;
  if(!bhv_set) {
    cout << "Null BehaviorSet in SHhelmIvP::postInitialMessage()" << endl;
    return;
  }
  if(!info_buffer) {
    cout << "Null info_bufer in SHhelmIvP::postInitialMessage()" << endl;
    return;
  }
  
  string ownship = hengine->getOwnship();

  vector<VarDataPair> mvector = bhv_set->getInitialMessages();
  int msize = mvector.size();
  for(int j=0; j<msize; j++) {
    VarDataPair msg = mvector[j];
    
    string var   = stripBlankEnds(msg.get_var());
    string sdata = stripBlankEnds(msg.get_sdata());
    double ddata = msg.get_ddata();
    
    cout << "Posting Initial Message - var: " << var << " sdata:";
    cout << sdata << " ddata:" << ddata << endl;

    if(sdata != "") {
      info_buffer->set_val(ownship, var, sdata);
    }
    else {
      info_buffer->set_val(ownship, var, ddata);
    }
  }
}

//-----------------------------------------------------------
// Procedure: setInfoBuffer

void SHelmIvP::setInfoBuffer(IB_HelmIvP* ibuff)
{
  info_buffer = ibuff;
}

//-----------------------------------------------------------
// Procedure: setOwnship

void SHelmIvP::setOwnship(string os_name)
{
  hengine->setOwnship(os_name);
}

//-----------------------------------------------------------
// Procedure: setBehaviorSet

void SHelmIvP::setBehaviorSet(BehaviorSet *g_bhv_set)
{
  bhv_set = g_bhv_set;
}

//-----------------------------------------------------------
// Procedure: setPIDEngine

void SHelmIvP::setPIDEngine(PIDEngine pengine)
{
  hengine->setPIDEngine(pengine);
}

//-----------------------------------------------------------
// Procedure: headingDelta

void SHelmIvP::headingDelta(double delta)
{
  string ownship = hengine->getOwnship();
  double curr_heading = info_buffer->dQuery(ownship, "NAV_HEADING");
  double want_heading = curr_heading + delta;
  
  info_buffer->set_val(ownship, "NAV_HEADING", want_heading);
}

//-----------------------------------------------------------
// Procedure: speedDelta

void SHelmIvP::speedDelta(double delta)
{
  string ownship = hengine->getOwnship();
  double curr_speed = info_buffer->dQuery(ownship, "NAV_SPEED");
  double want_speed = curr_speed + delta;
  
  info_buffer->set_val(ownship, "NAV_SPEED", want_speed);
}

//-----------------------------------------------------------
// Procedure: getIMG
//      Note: The ofix (Objective Function  IndeX) argument
//            indicates which objective function to focus on,
//            or if the collective function is to be sampled.
//               0...n-1 indicates the index of the n OF's.
//              -1 indicates the collective function.

IMG* SHelmIvP::getIMG(int index)
{
  int ofnum = bhv_set->getCount();

  if((index<-1)||(index >= ofnum)) 
    return(0);

  if(index != -1)
    if(hengine->getOF(index) == 0)
      return(0);

  int spd_ix = ivp_domain.getIndex("speed");
  int ylim   = ivp_domain.get_dpoints(spd_ix)-1;
  
  int tol_ix = ivp_domain.getIndex("tol");
  int tlim   = ivp_domain.get_dpoints(tol_ix)-1;
  
  IMG* ret_img = new IMG(ylim);
  
  // Note the actual domain used in the helm may be only a subset 
  // of the full ivp_domain if the behaviors are not producing OF's
  // over one or more variables. In this case, the OF's will have
  // been defined over a subset.

  IvPDomain sub_domain = hengine->getSubDomain();
  IvPBox ptbox(sub_domain.get_dcount());

  bool scrs = (sub_domain.getIndex("heading") != -1);
  bool sspd = (sub_domain.getIndex("speed") != -1);
  bool stol = (sub_domain.getIndex("tol") != -1);

  for(int i=0; i<360; i++) {
    for(int j=0; j<=ylim; j++) {

      int d=0;
      if(scrs) {ptbox.setPTS(d, i, i); d++;}
      if(sspd) {ptbox.setPTS(d, j, j); d++;}
      if(stol) ptbox.setPTS(d, tlim, tlim);

      bool ok;
      double val = 0.0;
      if((index >= 0) && (index < ofnum))
        val = hengine->getOF(index)->getPDMap()->evalPoint(&ptbox, &ok);
      else
        for(int k=0; k<ofnum; k++) {
	  IvPFunction *of = hengine->getOF(k);
	  if(of) 
	    val += of->getPDMap()->evalPoint(&ptbox, &ok);
	}
#if 0
      if(!ok) {
	cout << "Faulty point: " << endl;
	ptbox.print();
      }
#endif
      ret_img->set_ival_ix(i, j, val);
    }
  }

  ret_img->set_vname(this->getOwnship());
  if(index == -1)
    ret_img->set_bname("collective");
  else
    ret_img->set_bname(bhv_set->getDescriptor(index));
    
  return(ret_img);
}

//-----------------------------------------------------------
// Procedure: getOF
//      Note: The index argument indicates which objective 
//            function to focus return.

IvPFunction* SHelmIvP::getOF(int index)
{
  return(hengine->getOF(index));
}





