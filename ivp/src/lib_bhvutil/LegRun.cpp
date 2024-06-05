/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: LegRun.cpp                                      */
/*    DATE: May 26th, 2023                                  */
/************************************************************/

#include <iterator>
#include "GeomUtils.h"
#include "AngleUtils.h"
#include "XYFormatUtilsPoint.h"
#include "MBUtils.h"
#include "TurnGenWilliamson.h"
#include "LegRun.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

LegRun::LegRun()
{
  // Init Config vars
  m_turn_pt_gap  = 20;
  m_turn1_rad    = -1;
  m_turn2_rad    = -1;
  m_turn_rad_min = 2;
  m_turn1_bias   = 0;
  m_turn2_bias   = 0;
  m_turn1_ext    = 0;
  m_turn2_ext    = 0;
  m_lane_ix      = -5;
  
  m_turn1_dir = "port";
  m_turn2_dir = "port";

  // Init State vars
  m_lane_ix_set   = false;
  m_leg_modified  = true;
  m_turn_modified = true;

  // Init State vars (turn info cache)
  m_turn1_len = -1;
  m_turn2_len = -1;
}

//---------------------------------------------------------
// Procedure: valid()

bool LegRun::valid() const
{
  if((m_turn1_rad < 0) || (m_turn2_rad < 0))
   return(false);

  if(!m_p1.valid() || !m_p2.valid())
    return(false);

  return(true);
}

//---------------------------------------------------------
// Procedure: getTotalLen()

double LegRun::getTotalLen()
{
  double len = 2 * getLegLen();

  // Note: leg extents are accounted for in getTurnXLen()
  len += getTurn1Len();
  len += getTurn2Len();
  
  return(len);
}

//---------------------------------------------------------
// Procedure: setParams()

bool LegRun::setParams(string str)
{
  bool ok = true;
  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    ok = ok && setParam(param, value);
  }
  return(ok);
}

//---------------------------------------------------------
// Procedure: setParam()
//   Example: p1=4:5
//            turn1_rad=50

bool LegRun::setParam(string param, string val)
{
  setLegModified();

  if(param == "p1")
    return(setPoint1(val));
  else if(param == "vx1") // deprecated
    return(setPoint1(val));
  else if(param == "p2")
    return(setPoint2(val));
  else if(param == "vx2") // deprecated
    return(setPoint2(val));
  else if((param == "full_leg") || (param == "leg"))
    return(setLegByCtr(val));
  else if(param == "shift_point") // deprecated
    return(modCenterPt(val));
  else if(param == "shift_pt") // deprecated
    return(modCenterPt(val));
  else if((param == "leg_len") && isNumber(val))
    return(setLegLen(atof(val.c_str())));
  else if((param == "leg_length") && isNumber(val)) // deprecated
    return(setLegLen(atof(val.c_str())));
  else if((param == "leg_len_mod") && isNumber(val))
    return(modLegLen(atof(val.c_str())));
  else if((param == "leg_length_mod") && isNumber(val)) // deprecated
    return(modLegLen(atof(val.c_str())));
  else if((param == "leg_ang") && isNumber(val))
    return(setLegAng(atof(val.c_str())));
  else if((param == "leg_angle") && isNumber(val)) // deprecated
    return(setLegAng(atof(val.c_str())));
  else if((param == "leg_ang_mod") && isNumber(val))
    return(modLegAng(atof(val.c_str())));
  else if((param == "leg_angle_mod") && isNumber(val)) // deprecated
    return(modLegAng(atof(val.c_str())));

  else if((param == "turn1_rad") && isNumber(val))
    return(setTurn1Rad(atof(val.c_str())));
  else if((param == "turn2_rad") && isNumber(val))
    return(setTurn2Rad(atof(val.c_str())));
  else if((param == "turn_rad") && isNumber(val))
    return(setTurnRad(atof(val.c_str())));
  else if((param == "turn1_rad_mod") && isNumber(val))
    return(modTurn1Rad(atof(val.c_str())));
  else if((param == "turn2_rad_mod") && isNumber(val))
    return(modTurn2Rad(atof(val.c_str())));
  else if((param == "turn_rad_mod") && isNumber(val))
    return(modTurnRad(atof(val.c_str())));
  else if((param == "turn_rad_min") && isNumber(val))
    return(setTurnRadMin(atof(val.c_str())));

  else if((param == "turn1_ext") && isNumber(val))
    return(setTurn1Ext(atof(val.c_str())));
  else if((param == "turn2_ext") && isNumber(val))
    return(setTurn2Ext(atof(val.c_str())));
  else if((param == "turn_ext") && isNumber(val))
    return(setTurnExt(atof(val.c_str())));
  else if((param == "turn1_ext_mod") && isNumber(val))
    return(modTurn1Ext(atof(val.c_str())));
  else if((param == "turn2_ext_mod") && isNumber(val))
    return(modTurn2Ext(atof(val.c_str())));
  else if((param == "turn_ext_mod") && isNumber(val))
    return(modTurnExt(atof(val.c_str())));

  else if((param == "turn1_bias") && isNumber(val))
    return(setTurn1Bias(atof(val.c_str())));
  else if((param == "turn2_bias") && isNumber(val))
    return(setTurn2Bias(atof(val.c_str())));
  else if((param == "turn_bias") && isNumber(val))
    return(setTurnBias(atof(val.c_str())));
  else if((param == "turn1_bias_mod") && isNumber(val))
    return(modTurn1Bias(atof(val.c_str())));
  else if((param == "turn2_bias_mod") && isNumber(val))
    return(modTurn2Bias(atof(val.c_str())));
  else if((param == "turn_bias_mod") && isNumber(val))
    return(modTurnBias(atof(val.c_str())));

  else if((param == "turn_pt_gap") && isNumber(val))
    return(setTurnPtGap(atof(val.c_str())));
  else if(param == "adjust_turn")
    return(setBooleanOnString(m_adjust_turn, val));

  else if((param == "lane") && isNumber(val))
    return(setLaneIX(atoi(val.c_str())));

  else if(param == "turn1_dir") 
    return(setTurn1Dir(val));
  else if(param == "turn2_dir")
    return(setTurn2Dir(val));
  else if(param == "turn_dir")
    return(setTurnDir(val));
  else if(param == "id") {
    m_leg_id = val;
    return(true);
  }
  else
    return(false);
  
  return(true);
}

//---------------------------------------------------------
// Procedure: setPoint1()
//   Example: 4:6

bool LegRun::setPoint1(string str)
{
  XYPoint newpt = string2Point(str);
  if(!newpt.valid())
    return(false);
  m_p1.set_vertex(newpt.x(), newpt.y());
  return(true);

#if 0
  string xstr = biteStringX(str, ':');
  string ystr = str;
  if(!isNumber(xstr) || !isNumber(ystr))
    return(false);
  double xval = atof(xstr.c_str());
  double yval = atof(ystr.c_str());
  m_p1.set_vertex(xval, yval);

  clearTurnSegls();
  return(true);
#endif
}

//---------------------------------------------------------
// Procedure: setPoint2()
//   Example: 4:6

bool LegRun::setPoint2(string str)
{
  XYPoint newpt = string2Point(str);
  if(!newpt.valid())
    return(false);
  m_p2.set_vertex(newpt.x(), newpt.y());
  return(true);

#if 0
    
  string xstr = biteStringX(str, ':');
  string ystr = str;
  if(!isNumber(xstr) || !isNumber(ystr))
    return(false);
  double xval = atof(xstr.c_str());
  double yval = atof(ystr.c_str());
  m_p2.set_vertex(xval, yval);

  clearTurnSegls();
  return(true);
#endif
  
}

//---------------------------------------------------------
// Procedure: setLegByCtr()
//   Example: 4:6:64:80
//        Or: x=4,y=6,ang=64,len=80

bool LegRun::setLegByCtr(string str)
{
  if(strContains(str, "x="))
    return(setLegByCtr2(str));
  return(setLegByCtr1(str));  
}

//---------------------------------------------------------
// Procedure: setLegByCtr1()
//    Format: x:y:ang:len
//   Example: 4:6:64:50

bool LegRun::setLegByCtr1(string str)
{
  vector<string> svector = parseString(str, ':');
  if(svector.size() != 4)
    return(false);

  string xs = svector[0];
  string ys = svector[1];
  string langs = svector[2];
  string llens = svector[3];

  if(!isNumber(xs) || !isNumber(ys) ||
     !isNumber(langs) || !isNumber(llens))
    return(false);

  double cx = atof(xs.c_str());
  double cy = atof(ys.c_str());
  double lang = atof(langs.c_str());
  double llen = atof(llens.c_str());

  return(setLegAux(cx,cy,lang,llen));
}

//---------------------------------------------------------
// Procedure: setLegByCtr2()
//    Format: leg = x=4,y=5,ang=64,len=40

bool LegRun::setLegByCtr2(string str)
{
  str = tolower(str);
  vector<string> svector = parseString(str, ',');

  string xs,ys,langs,llens;
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    if(param == "x")
      xs = value;
    else if(param == "y")
      ys = value;
    else if(param == "ang")
      langs = value;
    else if(param == "len")
      llens = value;
    else
      return(false);
  }

  if(!isNumber(xs) || !isNumber(ys) ||
     !isNumber(langs) || !isNumber(llens))
    return(false);

  double cx = atof(xs.c_str());
  double cy = atof(ys.c_str());
  double lang = atof(langs.c_str());
  double llen = atof(llens.c_str());

  return(setLegAux(cx,cy,lang,llen));
}


//---------------------------------------------------------
// Procedure: modCenterPt()
//    Format: x:y or x,y, or x=1,y=2

bool LegRun::modCenterPt(string str)
{
  // Sanity check: Cannot shift/set the center point
  // unless the two vertices have been set earlier
  if(!m_p1.valid() || !m_p2.valid())
    return(false);

  double llen = getLegLen();
  double lang = getLegAng();

  // Part 1: Check and get the X/Y center value
  string xstr = tokStringParse(str, "x");
  string ystr = tokStringParse(str, "y");

  double cx = 0;
  double cy = 0;
  if(isNumber(xstr) && isNumber(ystr)) {
    cx = atof(xstr.c_str());
    cy = atof(ystr.c_str());
  }
  else { // handle "x,y" or "x:y" format
    str = findReplace(str, ',', ':');
    vector<string> svector = parseString(str, ':');
    if(svector.size() != 2)
      return(false);
    string xstr = svector[0];
    string ystr = svector[1];
    if(!isNumber(xstr) || !isNumber(ystr))
      return(false);
    cx = atof(xstr.c_str());
    cy = atof(ystr.c_str());
  }

  return(setLegAux(cx,cy,lang,llen));
}

//---------------------------------------------------------
// Procedure: modCenterPt(XYPoint)

bool LegRun::modCenterPt(XYPoint cpt)
{
  double cx   = cpt.x();
  double cy   = cpt.y();
  double llen = getLegLen();
  double lang = getLegAng();

  return(setLegAux(cx,cy,lang,llen));
}

//---------------------------------------------------------
// Procedure: setLegLen()

bool LegRun::setLegLen(double llen)
{
  if(llen <= 0)
    return(false);

  XYPoint cpt  = getCenterPt();
  double  lang = getLegAng();

  double  cx = cpt.x();
  double  cy = cpt.y();

  return(setLegAux(cx,cy,lang,llen));
}

//---------------------------------------------------------
// Procedure: modLegLen()

bool LegRun::modLegLen(double llen_mod)
{
  double curr_llen = getLegLen();
  
  return(setLegLen(curr_llen + llen_mod));
}

//---------------------------------------------------------
// Procedure: setLegAng()

bool LegRun::setLegAng(double lang)
{
  XYPoint cpt  = getCenterPt();
  double  llen = getLegLen();

  double cx = cpt.x();
  double cy = cpt.y();

  return(setLegAux(cx,cy,lang,llen));
}

//---------------------------------------------------------
// Procedure: modLegAng()

bool LegRun::modLegAng(double lang_mod)
{
  double curr_lang = getLegAng();
  
  return(setLegAng(curr_lang + lang_mod));
}

//---------------------------------------------------------
// Procedure: setLegAux()

bool LegRun::setLegAux(double cx, double cy,
		       double lang, double llen)
{
  if(llen <= 0)
    return(false);

  double x1,y1,x2,y2;
  projectPoint(lang-180, llen/2, cx,cy, x1,y1);
  projectPoint(lang, llen/2, cx,cy, x2,y2);

  m_p1.set_vertex(x1,y1);
  m_p2.set_vertex(x2,y2);
  
  clearTurnSegls();
  return(true);
}


//---------------------------------------------------------
// Procedure: setTurn1Rad()

bool LegRun::setTurn1Rad(double dval)
{
  if(dval <= 0)
    return(false);

  m_turn1_rad = dval;

  // If radius is legal positive number but less than configured min
  // limit, just clip and return true.
  if(m_turn1_rad < m_turn_rad_min)
    m_turn1_rad = m_turn_rad_min;

  clearTurnSegls();
  return(true);  
}

//---------------------------------------------------------
// Procedure: setTurn2Rad()

bool LegRun::setTurn2Rad(double dval)
{
  if(dval <= 0)
    return(false);

  m_turn2_rad = dval;

  // If radius is legal positive number but less than configured min
  // limit, just clip and return true.
  if(m_turn2_rad < m_turn_rad_min)
    m_turn2_rad = m_turn_rad_min;

  clearTurnSegls();
  return(true);  
}
  
//---------------------------------------------------------
// Procedure: setTurnRad()

bool LegRun::setTurnRad(double dval)
{
  if(dval <= 0)
    return(false);

  m_turn1_rad = dval;
  m_turn2_rad = dval;

  // If radius is legal positive number but less than configured min
  // limit, just clip and return true.
  if(m_turn1_rad < m_turn_rad_min)
    m_turn1_rad = m_turn_rad_min;
  if(m_turn2_rad < m_turn_rad_min)
    m_turn2_rad = m_turn_rad_min;

  clearTurnSegls();
  return(true);  
}
  
//---------------------------------------------------------
// Procedure: setTurnRadMin()

bool LegRun::setTurnRadMin(double dval)
{
  if(dval <= 0)
    return(false);

  m_turn_rad_min = dval;
  return(true);  
}
  
//---------------------------------------------------------
// Procedure: modTurn1Rad()

bool LegRun::modTurn1Rad(double dval)
{
  return(setTurn1Rad(m_turn1_rad + dval));  
}

//---------------------------------------------------------
// Procedure: modTurn2Rad()

bool LegRun::modTurn2Rad(double dval)
{
  return(setTurn2Rad(m_turn2_rad + dval));  
}

//---------------------------------------------------------
// Procedure: modTurnRad()

bool LegRun::modTurnRad(double dval)
{
  return(setTurn1Rad(m_turn1_rad + dval) &&
	 setTurn2Rad(m_turn2_rad + dval));
}
  
//---------------------------------------------------------
// Procedure: setTurn1Ext()

bool LegRun::setTurn1Ext(double dval)
{
  if(dval < 0)
    return(false);

  m_turn1_ext = dval;
  clearTurnSegls();
  return(true);  
}

//---------------------------------------------------------
// Procedure: setTurn2Ext()

bool LegRun::setTurn2Ext(double dval)
{
  if(dval < 0)
    return(false);

  m_turn2_ext = dval;
  clearTurnSegls();
  return(true);  
}
  
//---------------------------------------------------------
// Procedure: setTurnExt()

bool LegRun::setTurnExt(double dval)
{
  if(dval < 0)
    return(false);

  m_turn1_ext = dval;
  m_turn2_ext = dval;
  clearTurnSegls();
  return(true);  
}
  
//---------------------------------------------------------
// Procedure: modTurn1Ext()

bool LegRun::modTurn1Ext(double dval)
{
  clearTurnSegls();
  return(setTurn1Ext(m_turn1_ext + dval));  
}

//---------------------------------------------------------
// Procedure: modTurn2Ext()

bool LegRun::modTurn2Ext(double dval)
{
  clearTurnSegls();
  return(setTurn2Ext(m_turn2_ext + dval));  
}
  
//---------------------------------------------------------
// Procedure: modTurnExt()

bool LegRun::modTurnExt(double dval)
{
  clearTurnSegls();
  return(setTurn1Ext(m_turn1_ext + dval) &&
	 setTurn2Ext(m_turn2_ext + dval));
}
  
//---------------------------------------------------------
// Procedure: setTurn1Bias()

bool LegRun::setTurn1Bias(double dval)
{
  if(dval < -100)
    dval = -100;
  if(dval > 100)
    dval = 100;
  
  m_turn1_bias = dval;

  clearTurnSegls();
  return(true);  
}

//---------------------------------------------------------
// Procedure: setTurn2Bias()

bool LegRun::setTurn2Bias(double dval)
{
  if(dval < -100)
    dval = -100;
  if(dval > 100)
    dval = 100;
  
  m_turn2_bias = dval;

  clearTurnSegls();
  return(true);  
}
  
//---------------------------------------------------------
// Procedure: setTurnBias()

bool LegRun::setTurnBias(double dval)
{
  if(dval < -100)
    dval = -100;
  if(dval > 100)
    dval = 100;
  
  m_turn1_bias = dval;
  m_turn2_bias = dval;

  clearTurnSegls();
  return(true);  
}

//---------------------------------------------------------
// Procedure: modTurn1Bias()

bool LegRun::modTurn1Bias(double dval)
{
  return(setTurn1Bias(m_turn1_bias + dval));
}

//---------------------------------------------------------
// Procedure: modTurn2Bias()

bool LegRun::modTurn2Bias(double dval)
{
  return(setTurn2Bias(m_turn2_bias + dval));
}
  
//---------------------------------------------------------
// Procedure: modTurnBias()

bool LegRun::modTurnBias(double dval)
{
  return(setTurn1Bias(m_turn1_bias + dval) &&
	 setTurn2Bias(m_turn2_bias + dval));
}

//---------------------------------------------------------
// Procedure: setTurn1Dir()

bool LegRun::setTurn1Dir(string str)
{
  clearTurnSegls();
  return(setPortStarOnString(m_turn1_dir, str));  
}
  
//---------------------------------------------------------
// Procedure: setTurn2Dir()

bool LegRun::setTurn2Dir(string str)
{
  clearTurnSegls();
  return(setPortStarOnString(m_turn2_dir, str));  
}

//---------------------------------------------------------
// Procedure: setTurnDir()

bool LegRun::setTurnDir(string str)
{
  clearTurnSegls();
  setPortStarOnString(m_turn1_dir, str);  
  return(setPortStarOnString(m_turn2_dir, str));  
}

//---------------------------------------------------------
// Procedure: setTurnPtGap()

bool LegRun::setTurnPtGap(double dval)
{
  m_turn_pt_gap = dval;

  if(m_turn_pt_gap < 0.5)
    m_turn_pt_gap = 0.5;

  clearTurnSegls();
  return(true);
}

//---------------------------------------------------------
// Procedure: modTurnPtGap()

bool LegRun::modTurnPtGap(double dval)
{
  m_turn_pt_gap += dval;

  if(m_turn_pt_gap < 0.5)
    m_turn_pt_gap = 0.5;

  clearTurnSegls();
  return(true);
}

//---------------------------------------------------------
// Procedure: setLaneIX()

bool LegRun::setLaneIX(int ival)
{
  m_lane_ix = ival;
  m_lane_ix_set = true;
  return(true);
}

//---------------------------------------------------------
// Procedure: getLegLen()

double LegRun::getLegLen() const
{
  if(!m_p1.valid() || !m_p2.valid())
    return(-1);
  
  return(distPointToPoint(m_p1, m_p2));
}
  
//---------------------------------------------------------
// Procedure: getTurn1Len()

double LegRun::getTurn1Len() 
{
  // Use the cached value if it is valid
  if(m_turn1_len >= 0)
    return(m_turn1_len);

  // If cache needs updating, do so now
  initTurnPoints1();

  // Sanity check. Lowest return value is zero 
  if(m_turn1_len >= 0)
    return(m_turn1_len);

  return(0);
}
  
//---------------------------------------------------------
// Procedure: getTurn2Len()

double LegRun::getTurn2Len()
{
  // Use the cached value if it is valid
  if(m_turn2_len >= 0)
    return(m_turn2_len);

  // If cache needs updating, do so now
  initTurnPoints2();

  // Sanity check. Lowest return value is zero 
  if(m_turn2_len >= 0)
    return(m_turn2_len);

  return(0);
}
  
//---------------------------------------------------------
// Procedure: getLegAng()

double LegRun::getLegAng() const
{
  if(!m_p1.valid() || !m_p2.valid())
    return(-1);
  
  return(relAng(m_p1, m_p2));
}

//---------------------------------------------------------
// Procedure: getCenterPt()

XYPoint LegRun::getCenterPt() const
{
  return(getSegCenter(m_p1, m_p2));
}

//---------------------------------------------------------
// Procedure: getDistPt()
//
//---------------------------------------------------------
//                      <--mode=leg1                      |
//                                                        |
// mode=    /--------o----------------o---------\    ^    |
// turn1    | t1  |  p1              p2   | t2  |    |    |
//   |      \    /                         \    /  mode=  |
//   V       ---/       mode=leg2-->        \---   turn2  |
//----------------------------------------------------------

XYPoint LegRun::getDistPt(double dist_from_beg)
{
  XYPoint rpt = m_p1;

  //cout << "----" << endl;
  //cout << "getDistPt()" << endl;
  //cout << "LegRun: dp: total_len: " << getTotalLen() << endl;   
  //cout << "LegRun: dp: dist_from_beg: " << dist_from_beg << endl;   

  if(dist_from_beg <= 0)
    return(rpt);

  double lang = relAng(m_p1, m_p2);  
  double leglen = getLegLen();
  //cout << "LegRun: dp: lang: " << lang << ", leglen: " << leglen << endl;

  if(dist_from_beg <= 0)
    return(m_p1);
  if(dist_from_beg == leglen)
    return(m_p2);

  // Case A
  if(dist_from_beg < leglen) {
    rpt = projectPoint(lang, dist_from_beg, m_p1);
    return(rpt);
  }
  dist_from_beg -= leglen;

  // Case B
  double turn2_len = getTurn2Len();
  if(dist_from_beg < turn2_len) {
    rpt = m_turn2_segl.get_dist_point(dist_from_beg);
    return(rpt);
  }
  dist_from_beg -= turn2_len;

  // Case C
  if(dist_from_beg < leglen) {
    rpt = projectPoint(lang-180, dist_from_beg, m_p2);
    return(rpt);
  }
  dist_from_beg -= leglen;

  // Case D
  double turn1_len = getTurn1Len();
  if(dist_from_beg < turn1_len) {
    rpt = m_turn1_segl.get_dist_point(dist_from_beg);
    return(rpt);
  }

  return(rpt);
}


//---------------------------------------------------------
// Procedure: stringToLegRun


LegRun stringToLegRun(string str)
{
  LegRun null_legrun;
  LegRun good_legrun;

  vector<string> svector = parseString(str, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    bool ok = good_legrun.setParam(param, value);
    if(!ok)
      return(null_legrun);
  }
  return(good_legrun);
}

//-----------------------------------------------------------
// Procedure: initTurnPoints1()

XYSegList LegRun::initTurnPoints1(bool adjust, double osx,
				  double osy)
{
  XYSegList turn_segl;

  // Sanity checks
  if(!m_p1.valid() || !m_p2.valid())
    return(turn_segl);

  // If turn1_segl is built/cached then use it
  if(m_turn1_segl.size() != 0)
    return(m_turn1_segl);

  // Part 1: Determine the target position and heading
  double px1 = m_p1.x();
  double py1 = m_p1.y();
  double px2 = m_p2.x();
  double py2 = m_p2.y();
  double th = relAng(px1, py1, px2, py2);

  double lane_gap_thresh = 5;  
  double lane_gap = 0.1;
  
  if(adjust) {
    double sp_lane_gap = distPointToLine(osx,osy, px1,py1, px2,py2);
    if(sp_lane_gap > lane_gap_thresh) {
      lane_gap = sp_lane_gap;
      px1 = osx;
      py1 = osy;
    }
  }

  if(m_turn1_ext > 0) {
    turn_segl.add_vertex(px1,py1);
    projectPoint(th-180, m_turn1_ext, px1,py1, px1,py1);
  }    
    
  TurnGenWilliamson turngen;
  turngen.setStartPos(px1, py1, th+180);
  turngen.setEndPos(m_p1.x(), m_p1.y());
  turngen.setEndHeading(th);
  turngen.setTurnRadius(m_turn1_rad);
  turngen.setPointGap(m_turn_pt_gap);
  turngen.setLaneGap(lane_gap);
  turngen.setBiasPct(m_turn1_bias);
  turngen.setPortTurn(m_turn1_dir == "port");
  turngen.setAutoTurnDir(-1);
  turngen.generate();

  vector<XYPoint> pts = turngen.getPts();
  for(unsigned int i=0; i<pts.size(); i++)
    turn_segl.add_vertex(pts[i]);

  m_turn1_segl = turn_segl;
  m_turn1_len = turn_segl.length();
  return(m_turn1_segl);
}

//-----------------------------------------------------------
// Procedure: initTurnPoints2()

XYSegList LegRun::initTurnPoints2(bool adjust, double sx,
				  double sy)
{
  XYSegList turn_segl;

  // Sanity checks
  if(!m_p1.valid() || !m_p2.valid())
    return(turn_segl);
  
  // If turn_segl2 is built/cached then use it
  if(m_turn2_segl.size() != 0)
    return(m_turn2_segl);

  // Part 1: Determine the target position and heading
  double px1 = m_p1.x();
  double py1 = m_p1.y();
  double px2 = m_p2.x();
  double py2 = m_p2.y();
  double th = relAng(px2, py2, px1, py1);

  double lane_gap_thresh = 5;  
  double lane_gap = 0.1;
  
  if(adjust) {
    double sp_lane_gap = distPointToLine(sx,sy, px1,py1, px2,py2);
    if(sp_lane_gap > lane_gap_thresh) {
      lane_gap = sp_lane_gap;
      px2 = sx;
      py2 = sy;
    }
  }
  
  if(m_turn2_ext > 0) {
    turn_segl.add_vertex(px2,py2);
    projectPoint(th-180, m_turn2_ext, px2,py2, px2,py2);
  }
  
  TurnGenWilliamson turngen;
  turngen.setStartPos(px2, py2, th+180);
  turngen.setEndPos(m_p2.x(), m_p2.y());
  turngen.setEndHeading(th);
  turngen.setTurnRadius(m_turn2_rad);
  turngen.setPointGap(m_turn_pt_gap);
  turngen.setLaneGap(lane_gap);
  turngen.setBiasPct(m_turn2_bias);
  turngen.setPortTurn(m_turn2_dir == "port");
  turngen.setAutoTurnDir(-1);
  
  turngen.generate();
  vector<XYPoint> pts = turngen.getPts();
  for(unsigned int i=0; i<pts.size(); i++)
    turn_segl.add_vertex(pts[i]);

  m_turn2_segl = turn_segl;  
  m_turn2_len = turn_segl.length();
  return(m_turn2_segl);
}


//-----------------------------------------------------------
// Procedure: getSpec()

string LegRun::getSpec(char sepchar) const
{
  string s = "p1=" + m_p1.get_spec_xy(':');
  s += sepchar;
  s = "p2=" + m_p2.get_spec_xy(':');
  s += sepchar;

  s = "turn1_rad=" + doubleToStringX(m_turn1_rad,1);
  s += sepchar;
  s = "turn2_rad=" + doubleToStringX(m_turn2_rad,1);
  s += sepchar;
  
  s = "turn1_bias=" + doubleToStringX(m_turn1_bias,1);
  s += sepchar;
  s = "turn2_bias=" + doubleToStringX(m_turn2_bias,1);
  s += sepchar;

  s = "turn1_ext=" + doubleToStringX(m_turn1_ext,1);
  s += sepchar;
  s = "turn2_ext=" + doubleToStringX(m_turn2_ext,1);
  s += sepchar;
  
  s = "turn1_dir=" + m_turn1_dir;
  s += sepchar;
  s = "turn2_dir=" + m_turn2_dir;

  return(s);
}


//-----------------------------------------------------------
// Procedure: clearTurnSegls()

void LegRun::clearTurnSegls()
{
  m_turn1_segl = XYSegList();
  m_turn2_segl = XYSegList();
  m_turn1_len = -1;
  m_turn2_len = -1;
}
