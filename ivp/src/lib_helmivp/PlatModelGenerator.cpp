/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: PlatModelGenerator.cpp                               */
/*    DATE: Oct 24th, 2023                                       */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#include "MBUtils.h"
#include "AngleUtils.h"
#include "PlatModelGenerator.h"
#include "PMGen_Dubins.h"
#include "PMGen_Holonomic.h"

using namespace std;

//----------------------------------------------------------------
// Constructor()

PlatModelGenerator::PlatModelGenerator()
{
  m_hdg_hist_length = 5;
  
  m_pmgen = 0;
  m_count = 0;

  m_stale_pmodel = true;
  m_curr_utc = 0;
}

//----------------------------------------------------------------
// Procedure: generate()

PlatModel PlatModelGenerator::generate(double osx, double osy,
				       double osh, double osv)
{
  if(!m_pmgen) {
    PlatModel empty_pmodel;
    return(empty_pmodel);
  }    

  updateHdgHist(osh);

  m_pmgen->setHdgHist(m_recent_val_osh, m_recent_utc_osh);
  
  m_stale_pmodel = false;
  PlatModel pmodel = m_pmgen->generate(osx, osy, osh, osv);
  pmodel.setID(m_count);
  m_count++;
  return(pmodel);
}

//----------------------------------------------------------------
// Procedure: setParams()

bool PlatModelGenerator::setParams(string str)
{
  m_stale_pmodel = true;

  //=========================================================
  // Part 1: See if alg was specified, and handle if changed
  //=========================================================
  string alg = tolower(tokStringParse(str, "alg", '#', '='));
  if((alg != "holo") && (alg != "dubins"))
    return(false);

  // If this represents and algorithm switch, handle it
  PMGen* old_pmgen = m_pmgen;

  if((alg == "holo") && (m_alg != alg)) {
    m_pmgen = new PMGen_Holonomic;
    m_alg = alg;
  }
  else if((alg == "dubins") && (m_alg != alg)) {
    m_pmgen = new PMGen_Dubins;
    m_alg = alg;
  }

  if(!m_pmgen)
    return(false);
  
  //=========================================================
  // Part 2: Handle all other params except alg param
  //=========================================================
  bool all_pairs_ok = true;
  vector<string> svector = parseString(str, '#');
  for(unsigned int i=0; i<svector.size(); i++) {
    string param = biteStringX(svector[i], '=');
    string value = svector[i];
    if(param == "alg")
      continue;
    bool ok = m_pmgen->setParam(param, value);
    if(!ok) {
      all_pairs_ok = false;
      break;
    }
  }

  //=========================================================
  // Part 3: Handle when 1+ params bad. Revert to old pmgen
  //=========================================================
  if(!all_pairs_ok) {
    // If we had created a new pmgen, revert to the old one
    // and free the new one from memory.
    if(m_pmgen != old_pmgen) {
      delete(m_pmgen);
      m_pmgen = old_pmgen;	
    }
    return(false);
  }

  //=========================================================
  // Part 4: Handle when successful alg change was involved.
  //=========================================================
  // Free the former pmgen from memory
  if(old_pmgen != m_pmgen)
    delete(old_pmgen);
  
  return(true);
}
  
//----------------------------------------------------------------
// Procedure: getParamStr()

string PlatModelGenerator::getParamStr(string param) const
{
  if(!m_pmgen)
    return("");

  return(m_pmgen->getParamStr(param));
}
  
//----------------------------------------------------------------
// Procedure: getParamDbl()

double PlatModelGenerator::getParamDbl(string param) const
{
  if(!m_pmgen)
    return(0);

  return(m_pmgen->getParamDbl(param));
}
  
//----------------------------------------------------------------
// Procedure: getTurnRate()

double PlatModelGenerator::getTurnRate()
{
  if(!m_pmgen)
    return(0);

  return(m_pmgen->getHdgAvg(m_hdg_hist_length));
}
  
//----------------------------------------------------------------
// Procedure: updateHdgHist()

void PlatModelGenerator::updateHdgHist(double osh)
{
  cout << "updateHdgHist(): " << osh << endl;
  if(m_curr_utc == 0)
    return;

  m_recent_val_osh.push_front(osh);
  m_recent_utc_osh.push_front(m_curr_utc);

  cout << "size(1): " << m_recent_val_osh.size() << endl;
  bool pruned = false;
  while(!pruned) {
    if(m_recent_utc_osh.size() == 0)
      pruned = true;
    else {
      double elapsed = m_curr_utc - m_recent_utc_osh.back();
      cout << "elapsed: " << elapsed << endl;
      if(elapsed > m_hdg_hist_length) {
	m_recent_val_osh.pop_back();
	m_recent_utc_osh.pop_back();
      }
      else
	pruned = true;
    }
  }
  cout << "size(2): " << m_recent_val_osh.size() << endl;
}

