/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AVD_Entry.cpp                                        */
/*    DATE: November 9th, 2014                                   */
/*****************************************************************/

#include <iostream>
#include "AVD_Entry.h"
#include "MBUtils.h"
#include "CurveFitting.h"

using namespace std;

//-------------------------------------------------------------
// Constructor

AVD_Entry::AVD_Entry()
{
  m_dh = 0;
  m_spd = 0;

  m_turn_sequ_set = false;
  m_turn_poly_set = false;
  m_turn_segl_raw_set = false;
  m_turn_segl_lsq_set = false;
  m_turn_segl_pwl_set = false;

  m_interp_direct = false;
  m_interp_second = false;

  m_end_ix  = 0;
  m_look_ix = 0;
}


//-------------------------------------------------------------
// Procedure: setDeltaHdg()

bool AVD_Entry::setDeltaHdg(double dh)
{
  if(dh < -180)
    return(false);
  m_dh = dh;
  return(true);
}


//-------------------------------------------------------------
// Procedure: setSpd()

bool AVD_Entry::setSpd(double spd)
{
  if(spd <= 0)
    return(false);
  m_spd = spd;
  return(true);
}


//-------------------------------------------------------------
// Procedure: setTurnSequ()

bool AVD_Entry::setTurnSequ(TurnSequence tseq)
{
  if(tseq.size() == 0)
    return(false);

  m_turn_sequ = tseq;
  m_turn_segl_raw = tseq.getPositionData();

  m_turn_sequ_set = true;
  m_turn_segl_raw_set = true;

  if(m_turn_segl_raw.size() > 0)
    m_end_ix = m_turn_segl_raw.size() - 1;
  else
    m_end_ix = 0;

  // if(!fitPolynomial() || !buildPieceWise())
  //   return(false);

  return(true);
}


//-------------------------------------------------------------
// Procedure: setInterpStatus()

bool AVD_Entry::setInterpStatus(string status)
{
  if((status=="none")||(status=="pair")||(status=="diag")||(status=="fail")) {
    m_interp_status = status;
    return(true);
  }
  return(false);
}


//-------------------------------------------------------------
// Procedure: setTurnPoly()

bool AVD_Entry::setTurnPoly(TurnPoly poly, string source)
{
  m_turn_poly = poly;
  m_turn_poly_set = true;

  //m_turn_segl_lsq = m_turn_poly.getXYSLForConstantXSamplingInterval(0.5);
  //m_turn_segl_lsq = (m_turn_poly.getNorthFacingTurnSequence()).getPositionData();

  m_turn_segl_lsq = m_turn_poly.getNFaceDisc(0.1);

  if(source == "interp_direct")
    m_interp_direct = true;
  else if(source == "interp_second")
    m_interp_second = true;

  buildPieceWise();

  return(true);
}


//-------------------------------------------------------------
// Procedure: buildPieceWise

bool AVD_Entry::buildPieceWise()
{
  // Sanity Check
  if(!m_turn_poly_set) 
    return(false);

  m_turn_segl_pwl = m_turn_poly.getNorthFacingTurnSequence().getPositionData();

  // cout << "rotation angle=" << doubleToString(m_turn_poly.getRotationAngle(),2) << endl;
  // cout << "tangent  angle=" << doubleToString(m_turn_poly.getTangentAngle(),2) << endl;
  //  cout << "ray angle=" << doubleToString(m_turn_poly.get(),2) << endl;
  m_turn_segl_ray = m_turn_poly.getFinalRay();

  m_turn_segl_pwl_set = true;
  
  return(true);
}


//-------------------------------------------------------------
// Procedure: incEndIX

void AVD_Entry::incEndIX()
{
  if(!m_turn_segl_raw_set)
    return;
  if((m_end_ix + 1) >= m_turn_segl_raw.size())
    return;
  m_end_ix++;
}

//-------------------------------------------------------------
// Procedure: decEndIX

void AVD_Entry::decEndIX()
{
  if(!m_turn_segl_raw_set)
    return;
  if(m_end_ix > 0)
    m_end_ix--;
  if(m_look_ix > m_end_ix)
    m_look_ix = m_end_ix;

}

//-------------------------------------------------------------
// Procedure: resetEndIX

void AVD_Entry::resetEndIX()
{
  if(!m_turn_segl_raw_set)
    return;
  if(m_turn_segl_raw.size() > 0)
    m_end_ix = m_turn_segl_raw.size() - 1;
}


//-------------------------------------------------------------
// Procedure: incLookIX

void AVD_Entry::incLookIX()
{
  if(!m_turn_segl_raw_set)
    return;
  if((m_look_ix + 1) >= m_turn_segl_raw.size())
    return;
  if((m_look_ix + 1) > m_end_ix)
    return;
  m_look_ix++;
}

//-------------------------------------------------------------
// Procedure: decLookIX

void AVD_Entry::decLookIX()
{
  if(!m_turn_segl_raw_set)
    return;
  if(m_look_ix > 0)
    m_look_ix--;
}

//-------------------------------------------------------------
// Procedure: resetLookIX

void AVD_Entry::resetLookIX()
{
  if(!m_turn_segl_raw_set)
    return;
  if(m_turn_segl_raw.size() > 0)
    m_look_ix = m_turn_segl_raw.size() - 1;
  if(m_look_ix > m_end_ix)
    m_look_ix = m_end_ix;
}

//-------------------------------------------------------------
// Procedure: fitPolynomial

bool AVD_Entry::fitPolynomial() 
{
  if(!m_turn_sequ_set)
    return(false);

  TurnPoly tp;

  if(tp.generateTurnPolyFromTurnSequence(m_turn_sequ)){
    m_turn_poly = tp;
    m_turn_segl_lsq = m_turn_poly.getNFaceDisc(0.1);
    
    //  TurnSequence sequ = m_turn_poly.getNorthFacingTurnSequence();  
    //m_turn_segl_lsq = sequ.getPositionData();

    m_turn_poly_set = true;
    return(true);
  }
  else
    return false;  
}


//-------------------------------------------------------------
// Procedure: getTurnDur()

double AVD_Entry::getTurnDur() const
{
  if(!m_turn_sequ_set)
    return(0);

  return(m_turn_sequ.getTurnDuration());
}


//-------------------------------------------------------------
// Procedure: getLookX

double AVD_Entry::getLookX() const
{
  return(m_turn_segl_raw.get_vx(m_look_ix));
}

//-------------------------------------------------------------
// Procedure: getLookY

double AVD_Entry::getLookY() const
{
  return(m_turn_segl_raw.get_vy(m_look_ix));
}


//-------------------------------------------------------------
// Procedure: getSummary

string AVD_Entry::getSummary(unsigned int padlen) const
{
  string msg;

  string tseq_set = m_turn_sequ_set ? "1" : "0";

  //  string tseq_set = "0";
  string poly_set = "0";
  string segl_set = "0";
  if(m_turn_sequ_set)
    tseq_set = "1";
  if(m_turn_poly_set)
    poly_set = "1";
  if(m_turn_poly_set)
    segl_set = "1";
  
  msg += "(" + doubleToStringX(m_spd,2);
  msg += "," + doubleToStringX(m_dh,1) + ")";
  msg += "[" + tseq_set + poly_set + segl_set + "]";

  if(padlen != 0)
    msg = padString(msg, padlen, false);
  
  return(msg);
}

//-------------------------------------------------------------
// Procedure: pWiseExceedsGoal Check if any segments of the PWL
// representation exceed the DH Goal.

bool AVD_Entry::pWiseExceedsGoal() const
{
  if(!m_turn_segl_pwl_set) return(false);

  unsigned int segs = m_turn_segl_pwl.size() - 1;

  for(unsigned int i=0; i<segs;i++){
    double xa,ya,xb,yb, ang;
    xa=m_turn_segl_pwl.get_vx(i);
    ya=m_turn_segl_pwl.get_vy(i);
    xb=m_turn_segl_pwl.get_vx(i+1);
    yb=m_turn_segl_pwl.get_vy(i+1);
    ang=relAng(xa,ya,xb,yb);

    if(ang > m_dh) return(true);
  }
  
  return(false);
}

bool AVD_Entry::setTurnPWL(XYSegList pwl)
{
  if(pwl.size() == 0) return(false);

  if(m_turn_segl_pwl_set) cout << "Overwriting PWL" << endl;
  
  m_turn_segl_pwl = pwl;
  m_turn_segl_pwl_set = true;

  return(true);
}
