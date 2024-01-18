/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: OpField.cpp                                     */
/*    DATE: September 25th, 2023                            */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "GeomUtils.h"
#include "OpField.h"
#include "XYFormatUtilsPoint.h"
#include "XYFormatUtilsPoly.h"

using namespace std;

//---------------------------------------------------------
// Constructor()

OpField::OpField()
{
}

//---------------------------------------------------------
// Procedure: getPtAliases()

vector<string> OpField::getPtAliases() const
{
  vector<string> svector;
  map<string,XYPoint>::const_iterator p;
  for(p=m_map_pts.begin(); p!= m_map_pts.end(); p++)
    svector.push_back(p->first);
  
  return(svector);
}

//---------------------------------------------------------
// Procedure: getPoint()

XYPoint OpField::getPoint(string alias) const
{
  XYPoint null_pt;
  
  map<string,XYPoint>::const_iterator p = m_map_pts.find(alias);

  if(p == m_map_pts.end())
    return(null_pt);

  return(p->second);
}

//---------------------------------------------------------
// Procedure: getColor()

string OpField::getColor(string alias) const
{
  map<string,string>::const_iterator p = m_map_colors.find(alias);
  if(p == m_map_colors.end())
    return("");

  return(p->second);
}

//---------------------------------------------------------
// Procedure: size()

unsigned int OpField::size() const
{
  return(ptSize() + polySize() + seglSize());
}

//---------------------------------------------------------
// Procedure: hasKey()

bool OpField::hasKey(string key) const
{
  if(hasKeyPt(key) || hasKeyPoly(key) || hasKeySegl(key))
    return(true);
  return(false);
}

//---------------------------------------------------------
// Procedure: hasKeyPt()

bool OpField::hasKeyPt(string key) const
{
  if(m_map_pts.count(key))
    return(true);
  return(false);
}

//---------------------------------------------------------
// Procedure: hasKeyPoly()

bool OpField::hasKeyPoly(string key) const
{
  if(m_map_polys.count(key))
    return(true);
  return(false);
}

//---------------------------------------------------------
// Procedure: hasKeySegl()

bool OpField::hasKeySegl(string key) const
{
  if(m_map_segls.count(key))
    return(true);
  return(false);
}

//---------------------------------------------------------
// Procedure: config()
// Examples:  sw=0,0 # se=100,0 # ne=100,100 # nw=0,100
//            redzone=pts={0,0:10,0:5,5} # nw=160,0

bool OpField::config(string str)
{
  bool all_ok = true;
  if(isQuoted(str))
    str = stripQuotes(str);
  
  vector<string> svector = parseString(str, '#');
  for(unsigned int i=0; i<svector.size(); i++) {
    string entry = svector[i];

    string alias = biteStringX(entry,'=');
    string value = entry;

    cout << "alias=" << alias << endl;
    cout << "value=" << value << endl;

    if(isColor(value)) 
      m_map_colors[alias] = value;
    else {
      bool ok = addPoint(alias, value);
      cout << "ok=" << boolToString(ok) << endl;
      if(!ok)
	all_ok = false;
    }
  }
  return(all_ok);
}


//---------------------------------------------------------
// Procedure: addPoint()
// Examples:  sw=0,0 
//            foo=2,3    

bool OpField::addPoint(string alias, string str)
{
  if(m_map_pts.count(alias))
    return(false);
  return(setPoint(alias, str));
}

//---------------------------------------------------------
// Procedure: addPoint()

bool OpField::addPoint(string alias, XYPoint pt)
{
  if(m_map_pts.count(alias))
    return(false);
  return(setPoint(alias, pt));
}

//---------------------------------------------------------
// Procedure: setPoint()
// Examples:  sw=0,0 
//            foo=2,3    

bool OpField::setPoint(string alias, string str)
{
  XYPoint pt = string2Point(str);
  return(setPoint(alias, pt));
}

//---------------------------------------------------------
// Procedure: setPoint()

bool OpField::setPoint(string alias, XYPoint pt)
{
  if(!pt.valid())
    return(false);

  m_map_pts[alias] = pt;
  return(true);
}


//---------------------------------------------------------
// Procedure: addPoly()

bool OpField::addPoly(string alias, string str)
{
  if(m_map_polys.count(alias))
    return(false);
  return(setPoly(alias, str));
}

//---------------------------------------------------------
// Procedure: addPoly()

bool OpField::addPoly(string alias, XYPolygon poly)
{
  if(m_map_polys.count(alias))
    return(false);
  return(setPoly(alias, poly));
}

//---------------------------------------------------------
// Procedure: setPoly()

bool OpField::setPoly(string alias, string polystr)
{
  XYPolygon poly = string2Poly(polystr);
  return(setPoly(alias, poly));
}

//---------------------------------------------------------
// Procedure: setPoly()

bool OpField::setPoly(string alias, XYPolygon poly)
{
  if(!poly.valid())
    return(false);

  m_map_polys[alias] = poly;
  return(true);
}

//---------------------------------------------------------
// Procedure: merge()

bool OpField::merge(OpField opf)
{
  map<string, XYPoint>::iterator p;
  for(p=opf.m_map_pts.begin(); p!=opf.m_map_pts.end(); p++) {
    string key = p->first;
    XYPoint pt = p->second;
    bool ok = addPoint(key, pt);
    if(!ok)
      return(false);
  }

  map<string, XYPolygon>::iterator q;
  for(q=opf.m_map_polys.begin(); q!=opf.m_map_polys.end(); q++) {
    string key = q->first;
    XYPolygon poly = q->second;
    bool ok = addPoly(key, poly);
    if(!ok)
      return(false);
  }

  return(true);
}

//---------------------------------------------------------
// Procedure: simplify()
//   Purpose: Remove duplicates of point elements that are
//            at the same location. This may occur if the opf 
//            file had lines like:
//                     AB=x=34,y=9, and
//                     ABT=34,9
//            This simplify() function will keep the element 
//            with the shorter key (AB in this case).
//            If keep_shorter is false, the element with the 
//            longer key is kept. If the keys have the same
//            size, neither element will be removed. 

void OpField::simplify(bool keep_shorter)
{
  set<string> keys_to_remove;

  map<string, XYPoint>::iterator p;
  for(p=m_map_pts.begin(); p!=m_map_pts.end(); p++) {
    string key1 = p->first;
    XYPoint pt1 = p->second;
    unsigned int len1 = key1.length();

    map<string, XYPoint>::iterator q;
    for(q=m_map_pts.begin(); q!=m_map_pts.end(); q++) {
      string key2 = q->first;
      XYPoint pt2 = q->second;
      unsigned int len2 = key2.length();

      if(len1 == len2)
	continue;
      if(distPointToPoint(pt1, pt2) < 0.001) {
	if((len1 < len2) && keep_shorter)
	  keys_to_remove.insert(key2);
	else
	  keys_to_remove.insert(key1);
      }
    }
  }
  
  set<string>::iterator q;
  for(q=keys_to_remove.begin(); q!=keys_to_remove.end(); q++) {
    string key = *q;
    m_map_pts.erase(key);
  }
}
