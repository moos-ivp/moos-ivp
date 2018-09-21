/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: RandomVariableSet.cpp                                */
/*    DATE: Dec 18th 2009                                        */
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
#include "RandomVariableSet.h"
#include "MBUtils.h"

using namespace std;

//---------------------------------------------------------
// Destructor

RandomVariableSet::~RandomVariableSet()
{
  unsigned int i, vsize = m_rvar_vector.size();
  for(i=0; i<vsize; i++)
    delete(m_rvar_vector[i]);
}

//---------------------------------------------------------
// Procedure: contains

bool RandomVariableSet::contains(const string& varname) const
{
  unsigned int i, vsize = m_rvar_vector.size();
  for(i=0; i<vsize; i++) {
    if(m_rvar_vector[i]->getVarName() == varname)
      return(true);
  }
  return(false);
}

//---------------------------------------------------------
// Procedure: addRandomVar()

string RandomVariableSet::addRandomVar(const string& spec)
{
  string rvtype = tokStringParse(spec, "type", ',', '=');
  
  cout << "rvtype: [" << rvtype << "]" << endl;

  if((rvtype == "uniform") || (rvtype == ""))
    return(addRandomVarUniform(spec));

  if((rvtype == "gaussian") || (rvtype == "normal"))
    return(addRandomVarGaussian(spec));

  return("unknown random variable type: " + rvtype);
}

//---------------------------------------------------------
// Procedure: addRandomVarUniform()

string RandomVariableSet::addRandomVarUniform(const string& spec)
{
  string varname;
  string keyname;
  double minval=0;
  double maxval=1;
  bool   minval_set = false;
  bool   maxval_set = false;

  vector<string> svector = parseString(spec, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = biteStringX(svector[i], '=');
    string right = svector[i];
    if(left == "varname")
      varname = right;
    else if(left == "key")
      keyname = tolower(right);
    else if((left == "min") && isNumber(right)) {
      minval = atof(right.c_str());
      minval_set = true;
    }
    else if((left == "max") && isNumber(right)) {
      maxval = atof(right.c_str());
      maxval_set = true;
    }
    else if(left != "type")
      return("Bad parametery=value: " + left + "=" + right);
  }
  
  if(keyname == "")
    return("key is not specified");

  if((keyname != "at_post") && (keyname != "at_start") && (keyname != "at_reset"))
    return("unknown random_var key: " + keyname);

  if(varname == "")
    return("Unset variable name");

  if(!minval_set)
    return("Lower value of the range not set");

  if(!maxval_set)
    return("Upper value of the range not set");
  
  if(minval > maxval)
    return("Minimum value greater than maximum value");


  if(contains(varname))
    return("Duplicate random variable");

  
  RandVarUniform *rand_var = new RandVarUniform();
  rand_var->setVarName(varname);
  if(keyname != "")
    rand_var->setKeyName(keyname);
  rand_var->setType("uniform");
  rand_var->setParam("min", minval);
  rand_var->setParam("max", maxval);
  
  m_rvar_vector.push_back(rand_var);
  return("");
}

//---------------------------------------------------------
// Procedure: addRandomVarGaussian()

string RandomVariableSet::addRandomVarGaussian(const string& spec)
{
  string varname;
  string keyname;
  double minval=0;
  double maxval=1;
  double mu=0;
  double sigma=1;
  bool   minval_set = false;
  bool   maxval_set = false;
  bool   mu_set     = false;
  bool   sigma_set  = false;

  vector<string> svector = parseString(spec, ',');
  unsigned int i, vsize = svector.size();
  for(i=0; i<vsize; i++) {
    string left  = stripBlankEnds(biteString(svector[i], '='));
    string right = stripBlankEnds(svector[i]);
    if(left == "varname")
      varname = right;
    else if(left == "key")
      keyname = tolower(right);
    else if((left == "min") && isNumber(right)) {
      minval = atof(right.c_str());
      minval_set = true;
    }
    else if((left == "max") && isNumber(right)) {
      maxval = atof(right.c_str());
      maxval_set = true;
    }
    else if((left == "mu") && isNumber(right)) {
      mu = atof(right.c_str());
      mu_set = true;
    }
    else if((left == "sigma") && isNumber(right)) {
      sigma = atof(right.c_str());
      sigma_set = true;
    }
    else if(left != "type")
      return("Bad parameterx=value: " + left + "=" + right);
  }
  
  if(keyname == "")
    return("key is not specified");
  
  if((keyname != "at_post") && (keyname != "at_start") && (keyname != "at_reset"))
    return("unknown random_var key: " + keyname);

  if(varname == "")
    return("Unset variable name");

  if(!minval_set)
    return("Lower value of the range not set");

  if(!maxval_set)
    return("Upper value of the range not set");

  if(!mu_set)
    return("Mu not set");

  if(!sigma_set)
    return("Sigma not set");
  
  if(minval > maxval)
    return("Minimum value greater than maximum value");

  if(contains(varname))
    return("Duplicate random variable");
  
  RandVarGaussian *rand_var = new RandVarGaussian();
  rand_var->setVarName(varname);
  if(keyname != "")
    rand_var->setKeyName(keyname);
  rand_var->setType("gaussian");
  rand_var->setParam("min", minval);
  rand_var->setParam("max", maxval);
  rand_var->setParam("mu",  mu);
  rand_var->setParam("sigma", sigma);
  
  m_rvar_vector.push_back(rand_var);
  return("");
}

//---------------------------------------------------------
// Procedure: reset

void RandomVariableSet::reset(const string& key, double tstamp)
{
  unsigned int i, vsize = m_rvar_vector.size();
  for(i=0; i<vsize; i++) {
    if(m_rvar_vector[i]->getKeyName() == key)
      m_rvar_vector[i]->reset();
  }
}

//---------------------------------------------------------
// Procedure: getVarName(index)

string RandomVariableSet::getVarName(unsigned int ix) const
{
  if(ix < m_rvar_vector.size())
    return(m_rvar_vector[ix]->getVarName());
  return("");
}

//---------------------------------------------------------
// Procedure: getKeyName(index)

string RandomVariableSet::getKeyName(unsigned int ix) const
{
  if(ix < m_rvar_vector.size())
    return(m_rvar_vector[ix]->getKeyName());
  return("");
}

//---------------------------------------------------------
// Procedure: getType(index)

string RandomVariableSet::getType(unsigned int ix) const
{
  if(ix < m_rvar_vector.size())
    return(m_rvar_vector[ix]->getType());
  return("");
}

//---------------------------------------------------------
// Procedure: getValue(index)

double RandomVariableSet::getValue(unsigned int ix) const
{
  if(ix < m_rvar_vector.size())
    return(m_rvar_vector[ix]->getValue());
  return(0);
}

//---------------------------------------------------------
// Procedure: getMinVal(index)

double RandomVariableSet::getMinVal(unsigned int ix) const
{
  if(ix < m_rvar_vector.size())
    return(m_rvar_vector[ix]->getMinVal());
  return(0);
}

//---------------------------------------------------------
// Procedure: getMaxVal(index)

double RandomVariableSet::getMaxVal(unsigned int ix) const
{
  if(ix < m_rvar_vector.size())
    return(m_rvar_vector[ix]->getMaxVal());
  return(0);
}

//---------------------------------------------------------
// Procedure: getStringSummary(index)

string RandomVariableSet::getStringSummary(unsigned int ix) const
{
  if(ix < m_rvar_vector.size())
    return(m_rvar_vector[ix]->getStringSummary());
  else
    return("");
}

//---------------------------------------------------------
// Procedure: getStringValue(index)

string RandomVariableSet::getStringValue(unsigned int ix) const
{
  if(ix < m_rvar_vector.size())
    return(m_rvar_vector[ix]->getStringValue());
  else
    return("");
}

//---------------------------------------------------------
// Procedure: getParams(index)

string RandomVariableSet::getParams(unsigned int ix) const
{
  if(ix < m_rvar_vector.size())
    return(m_rvar_vector[ix]->getParams());
  else
    return("");
}

//---------------------------------------------------------
// Procedure: print() 

void RandomVariableSet::print() const
{
  cout << "RandomVariableSet: " << m_rvar_vector.size() << endl; 
  for(unsigned int i=0; i<m_rvar_vector.size(); i++) {
    cout << "[" << i << "]:" << m_rvar_vector[i]->getStringSummary() << endl;
  }
  cout << "done." << endl;
}









