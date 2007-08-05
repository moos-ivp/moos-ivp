/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: SIM.cpp                                              */
/*    DATE: Mar 6th 2005                                         */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/
 
#include <iostream>
#include <assert.h>
#include "SIM.h"
#include "AngleUtils.h"
#include "MBUtils.h"
#include "VState.h"
#include "SimEngine.h"

#define M_PI 3.1415926535897

using namespace std;

//-----------------------------------------------------------
// Procedure: Constructor

SIM::SIM() 
{
  info_buffer = 0;
  click_time  = 1.0; // seconds
  curr_time   = 0;
}

//-------------------------------------------------------------
// Procedure: Destructor

SIM::~SIM()
{
  if(info_buffer) delete(info_buffer);

  for(int i=0; i<helms.size(); i++)
    if(helms[i]) {
      delete(helms[i]);
      helms[i] = 0;
    }
}

//-------------------------------------------------------------
// Procedure: setInfoBuffer

void SIM::setInfoBuffer(IB_HelmIvP *ib)
{
  info_buffer = ib;
  save_buffer = *ib;
}

//-------------------------------------------------------------
// Procedure: resetInfoBuffer

void SIM::resetInfoBuffer()
{
  if(info_buffer)
    info_buffer->reset(save_buffer);
}

//-------------------------------------------------------------
// Procedure: clickDecide

void SIM::clickDecide()
{
  // First figure out what each vehicle wants to to next, all based
  // on the current info_buffer state.
  for(int i=0; i<helms.size(); i++)
    helms[i]->click(curr_time);
}

//-------------------------------------------------------------
// Procedure: clickAct

void SIM::clickAct()
{
  int i;
  VState vstate;

  // Then get the messages that were produced by each behavior from
  // each helm during the process of producing an OF. These are applied 
  // to the info buffer AFTER all the helms have produced desired 
  // actions above, so that all helms/behaviors are looking at the 
  // same info buffer on each click
  for(i=0; i<helms.size(); i++)
    helms[i]->postBehaviorMessages();
  
  // Then execute that desired decision "simultaneously" for all 
  // vehicles
  for(int i=0; i<helms.size(); i++) {
    string h_nam = helms[i]->getOwnship();
    
    bool ok1, ok2, ok3, ok4; 
    vstate.m_dfX  = info_buffer->dQuery(h_nam, "NAV_X", &ok1);
    vstate.m_dfY  = info_buffer->dQuery(h_nam, "NAV_Y", &ok2);
    vstate.m_dfSpeed   = info_buffer->dQuery(h_nam, "NAV_SPEED", &ok4);
    vstate.m_dfHeading = degToRadians(info_buffer->dQuery(h_nam, "NAV_HEADING", &ok3));

    assert(ok1 && ok2 && ok3 && ok4);

    double desired_rudder = helms[i]->getDesiredRudder();
    double desired_thrust = helms[i]->getDesiredThrust();

    // Convert to Radians for SimEngine input
    double rudderAngle = desired_rudder * M_PI / 180.0;

    vstate.m_dfSpeed = desired_thrust / 20.0;

    SimEngine sim_eng;
    sim_eng.propagate(vstate, vstate.m_dfSpeed, rudderAngle, click_time);

    double new_crs = radToDegrees(radAngleWrap(vstate.m_dfHeading));

    ok1 = info_buffer->set_val(h_nam, "NAV_X", vstate.m_dfX);
    ok2 = info_buffer->set_val(h_nam, "NAV_Y", vstate.m_dfY);
    ok3 = info_buffer->set_val(h_nam, "NAV_HEADING", new_crs);
    ok4 = info_buffer->set_val(h_nam, "NAV_SPEED", vstate.m_dfSpeed);
    assert(ok1 && ok2 && ok3 && ok4);
  }

  curr_time += click_time;
  info_buffer->setCurrTime(curr_time);
}

//-------------------------------------------------------------
// Procedure: getInfoBHV

string SIM::getInfoBHV(int helm_ix)
{
  if((helm_ix >= 0) && (helm_ix < helms.size()))
    return(helms[helm_ix]->getInfoBHV());
  else
    return("");
}

//-----------------------------------------------------------
// Procedure: headingDelta

void SIM::headingDelta(int helm_ix, double val)
{
  if((helm_ix >= 0) && (helm_ix < helms.size()))
    helms[helm_ix]->headingDelta(val);
}

//-----------------------------------------------------------
// Procedure: speedDelta

void SIM::speedDelta(int helm_ix, double val)
{
  if((helm_ix >= 0) && (helm_ix < helms.size()))
    helms[helm_ix]->speedDelta(val);
}

//-------------------------------------------------------------
// Procedure: getOneImageSet

vector<IMG*> SIM::getOneImageSet(int index)
{
  vector<IMG*> rvector;

  if((index < 0) || (index >= helms.size()))
    return(rvector);
  
  IMG *collective_image = helms[index]->getIMG(-1);
  if(collective_image)
    rvector.push_back(collective_image);
  for(int j=0; j<helms[index]->getBehaviorCount(); j++) {
    IMG *img = helms[index]->getIMG(j);
    if(img)
      rvector.push_back(img);
  }
  return(rvector);
}

//-------------------------------------------------------------
// Procedure: getOF

IvPFunction *SIM::getOF(int index)
{
  if((index < 0) || (index >= helms.size()))
    return(0);
  else
    return(helms[index]->getOF(index));
}

//-------------------------------------------------------------
// Procedure: getAllImages

vector<IMG*> SIM::getAllImages()
{
  vector<IMG*> rvector;
  for(int i=0; i<helms.size(); i++) {
    IMG *collective_image = helms[i]->getIMG(-1);
    if(collective_image)
      rvector.push_back(collective_image);
    for(int j=0; j<helms[i]->getBehaviorCount(); j++) {
      IMG *img = helms[i]->getIMG(j);
      if(img)
	rvector.push_back(img);
    }
  }
  return(rvector);
}





