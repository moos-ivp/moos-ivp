/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: RandomPairSet.cpp                                    */
/*    DATE: Nov 22nd 2016                                        */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "RandomPairSet.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Destructor

RandomPairSet::~RandomPairSet()
{
  for(unsigned int i=0; i<m_rpair_vector.size(); i++)
    delete(m_rpair_vector[i]);
}

//---------------------------------------------------------
// Procedure: contains

bool RandomPairSet::contains(string varname) const
{
  for(unsigned int i=0; i<m_rpair_vector.size(); i++) {
    if(m_rpair_vector[i]->getVarName1() == varname)
      return(true);
    if(m_rpair_vector[i]->getVarName2() == varname)
      return(true);
  }
  return(false);
}

//---------------------------------------------------------
// Procedure: addRandomPair()

string RandomPairSet::addRandomPair(string spec)
{
  string rptype = tokStringParse(spec, "type", ',', '=');
  
  cout << "rptype: [" << rptype << "]" << endl;

  if(rptype == "poly")
    return(addRandomPairPoly(spec));
  
  return("unknown random variable type: " + rptype);
}

//---------------------------------------------------------
// Procedure: addRandomPairPoly()
//   Example: var1=PX, var2=PY, type=poly,
//            poly=format=ellipse, x=10, y=-75, degs=65, pts=8, major=80, minor=20

string RandomPairSet::addRandomPairPoly(string spec)
{
  string var1;
  string var2;
  string keyname;
  string poly;

  vector<string> svector = parseStringQ(spec, ',');
  for(unsigned int i=0; i<svector.size(); i++) {
    string left  = biteStringX(svector[i], '=');
    string right = svector[i];

    if(isQuoted(right))
      right = stripQuotes(right);
    
    if(left == "var1")
      var1 = right;
    else if(left == "var2")
      var2 = right;
    else if(left == "key")
      keyname = tolower(right);
    else if(left == "poly")
      poly = right;
    else if(left != "type")
      return("Bad parameter=value: " + left + "=" + right);
  }
  
  if(keyname == "")
    return("key is not specified");
  
  if((keyname != "at_post") && (keyname != "at_start") && (keyname != "at_reset"))
    return("unknown random_pair key: " + keyname);
  
  if(var1 == "")
    return("Unset variable1 name");
  
  if(var2 == "")
    return("Unset variable2 name");
  
  if(poly == "")
    return("Minimum value greater than maximum value");
  
  if(contains(var1))
    return("Duplicate random pair variable:" + var1);
  
  if(contains(var2))
    return("Duplicate random pair variable:" + var2);

  
  RandPairPoly *rand_pair = new RandPairPoly();
  rand_pair->setVarName1(var1);
  rand_pair->setVarName2(var2);
  rand_pair->setKeyName(keyname);
  rand_pair->setType("poly");

  bool ok = rand_pair->setParam("poly", poly);
  if(ok)
    m_rpair_vector.push_back(rand_pair);
  return("bad poly");
}

//---------------------------------------------------------
// Procedure: reset

void RandomPairSet::reset(string key, double tstamp)
{
  for(unsigned int i=0; i<m_rpair_vector.size(); i++) {
    if(m_rpair_vector[i]->getKeyName() == key)
      m_rpair_vector[i]->reset();
  }
}

//---------------------------------------------------------
// Procedure: getVarName1(index)

string RandomPairSet::getVarName1(unsigned int ix) const
{
  if(ix < m_rpair_vector.size())
    return(m_rpair_vector[ix]->getVarName1());
  return("");
}

//---------------------------------------------------------
// Procedure: getVarName2(index)

string RandomPairSet::getVarName2(unsigned int ix) const
{
  if(ix < m_rpair_vector.size())
    return(m_rpair_vector[ix]->getVarName2());
  return("");
}

//---------------------------------------------------------
// Procedure: getKeyName(index)

string RandomPairSet::getKeyName(unsigned int ix) const
{
  if(ix < m_rpair_vector.size())
    return(m_rpair_vector[ix]->getKeyName());
  return("");
}

//---------------------------------------------------------
// Procedure: getType(index)

string RandomPairSet::getType(unsigned int ix) const
{
  if(ix < m_rpair_vector.size())
    return(m_rpair_vector[ix]->getType());
  return("");
}

//---------------------------------------------------------
// Procedure: getValue1(index)

double RandomPairSet::getValue1(unsigned int ix) const
{
  if(ix < m_rpair_vector.size())
    return(m_rpair_vector[ix]->getValue1());
  return(0);
}

//---------------------------------------------------------
// Procedure: getValue2(index)

double RandomPairSet::getValue2(unsigned int ix) const
{
  if(ix < m_rpair_vector.size())
    return(m_rpair_vector[ix]->getValue2());
  return(0);
}

//---------------------------------------------------------
// Procedure: getStringSummary(index)

string RandomPairSet::getStringSummary(unsigned int ix) const
{
  if(ix < m_rpair_vector.size())
    return(m_rpair_vector[ix]->getStringSummary());
  else
    return("");
}

//---------------------------------------------------------
// Procedure: getStringValue1(index)

string RandomPairSet::getStringValue1(unsigned int ix) const
{
  if(ix < m_rpair_vector.size())
    return(m_rpair_vector[ix]->getStringValue1());
  else
    return("");
}

//---------------------------------------------------------
// Procedure: getStringValue2(index)

string RandomPairSet::getStringValue2(unsigned int ix) const
{
  if(ix < m_rpair_vector.size())
    return(m_rpair_vector[ix]->getStringValue2());
  else
    return("");
}

//---------------------------------------------------------
// Procedure: getParams(index)

string RandomPairSet::getParams(unsigned int ix) const
{
  if(ix < m_rpair_vector.size())
    return(m_rpair_vector[ix]->getParams());
  else
    return("");
}

//---------------------------------------------------------
// Procedure: print() 

void RandomPairSet::print() const
{
  cout << "RandomPairSet: " << m_rpair_vector.size() << endl; 
  for(unsigned int i=0; i<m_rpair_vector.size(); i++) {
    cout << "[" << i << "]:" << m_rpair_vector[i]->getStringSummary() << endl;
  }
  cout << "done." << endl;
}






