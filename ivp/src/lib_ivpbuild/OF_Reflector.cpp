/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: OF_Reflector.cpp                                     */
/*    DATE: Aug 29th 2005 (derived from OFR_AOF built long ago)  */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#include <stdlib.h>
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "IvPFunction.h"
#include "Regressor.h"
#include "RT_Uniform.h"
#include "RT_Priority.h"
#include "RT_Focus.h"
#include "MBUtils.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: Constructor
//     Notes: g_degree=1 makes piecewise linear functions.
//            g_degree=0 makes piecewise constant functions.
//            g_aof is the underlying function to be approximated.

OF_Reflector::OF_Reflector(const AOF *g_aof, int g_degree) 
{
  m_aof         = g_aof;
  m_pdmap       = 0;
  m_regressor   = new Regressor(m_aof, g_degree);
  if(m_aof)
    m_domain    = m_aof->getDomain();

  m_rt_uniform  = new RT_Uniform(m_regressor);
  m_rt_priority = new RT_Priority(m_regressor);
  m_rt_focus    = new RT_Focus(m_regressor);
  
  m_uniform_amount = 1;
  m_smart_amount   = 0;
  m_smart_percent  = 0;
  m_smart_thresh   = 0;
  m_auto_peak      = true;
}

//-------------------------------------------------------------
// Procedure: Destructor

OF_Reflector::~OF_Reflector()
{
  // The PDMap is typically null in normal operation, after a 
  // call to extractOF() is made. But if for some reason the 
  // user used the reflector to create a PDMap, but then never
  // extracted it, the memory is free here.
  if(m_pdmap) 
    delete(m_pdmap);

  delete(m_regressor);
  delete(m_rt_uniform);
  delete(m_rt_priority);
  delete(m_rt_focus);
}

//-------------------------------------------------------------
// Procedure: extractOF
//   Purpose: Build and return for the caller an IvP objective
//            function built from the pdmap. Once this is done
//            the caller "owns" the PDMap. The reason for this is
//            that the pdmap is assumed to be too expensive to 
//            replicate for certain applications.

IvPFunction *OF_Reflector::extractOF(bool normalize)
{
  // Check for a NULL pdmap. This may be the case if the user made
  // an error in parameterizing the reflector, and a failed create
  // ensued and the user did not notice.
  if(!m_pdmap)
    return(0);

  // By default, the pdap is normalized before handing over
  if(normalize)
    m_pdmap->normalize(0.0, 100.0);

  // The pdmap now "belongs" to the IvPFunction, given to the caller.
  // The pdmap memory from the heap now belongs to the caller
  IvPFunction *new_ipf = new IvPFunction(m_pdmap);

  m_pdmap = 0;
  return(new_ipf);
}

//-------------------------------------------------------------
// Procedure: clearPDMap()

void OF_Reflector::clearPDMap()
{
  if(m_pdmap) {
    delete(m_pdmap);
    m_pdmap = 0;
  }
}
    
//-------------------------------------------------------------
// Procedure: create
//      Note: Returns the number of pieces made in the new PDMap.
//            A return of zero indicates an error.

int OF_Reflector::create(const string params)
{
  if(!setParam(params))
    return(0);
  else
    return(create());
}
    
//-------------------------------------------------------------
// Procedure: setParam

bool OF_Reflector::setParam(string str)
{
  bool ok = true;
  str = tolower(stripBlankEnds(str));

  vector<string> svector = parseString(str, '#');
  int vsize = svector.size();

  if(vsize == 0)
    return(false);
  
  for(int i=0; i<vsize; i++) {
    svector[i] = stripBlankEnds(svector[i]);
    vector<string> tvector = parseString(svector[i], '=');
    if(tvector.size() == 2) {
      string param = tvector[0];
      string value = tvector[1];
      ok = ok && setParam(param, value);
    }
  }
  return(ok);
}


//-------------------------------------------------------------
// Procedure: setParam
// Note: Care must be taken to add a refine_region and refine_piece
//       only in pairs. A refine_region must be added first, followed
//       by a refine_piece. A refine_region can only be added when 
//       the count of the two vectors are equal, and a refine_piece
//       can be added only if there is currently one more 
//       refine_region than the number of refine_pieces. If there is
//       a syntax error on adding a refine_region, false is simply
//       returned. If there is a syntax error on the refine-piece add, 
//       a single refine-region is popped from its vector.
//
// Note: The above strategy assumes that if the creation process is
//       commenced with one more refine_region than refine_piece, 
//       then the extra refine_region is simply disregarded.

bool OF_Reflector::setParam(string param, string value)
{
  param = tolower(stripBlankEnds(param));
  value = tolower(stripBlankEnds(value));

  if(param == "strict_range") {
    if((value != "true") && (value != "false"))
      return(false);
    if(m_regressor)
      m_regressor->setStrictRange(value=="true");
  }
  else if((param=="uniform_amount")||(param=="uniform_amt")) {
    int uniform_amount = atoi(value.c_str());
    if(!isNumber(value) || (uniform_amount < 1))
      return(false);
    m_uniform_amount = uniform_amount;
  }
  else if((param=="uniform_piece")||(param=="uniform_box")) {
    m_uniform_piece = stringToPointBox(value, m_domain, ',', ':');
    if(m_uniform_piece.null())
      return(false);
  }
  else if(param=="uniform_grid") {
    m_uniform_grid = stringToPointBox(value, m_domain, ',', ':');
    if(m_uniform_grid.null())
      return(false);
  }
  else if((param=="refine_region")||(param=="focus_region")) {
    if(m_refine_regions.size() != m_refine_pieces.size())
      return(false);
    IvPBox refine_region = stringFloatToRegionBox(value, m_domain, ',', ':');
    if(refine_region.null())
      return(false);
    else
      m_refine_regions.push_back(refine_region);
  }
  else if((param=="refine_piece")||(param=="focus_box")) {
    if((m_refine_regions.size() - m_refine_pieces.size()) != 1)
      return(false);
    IvPBox refine_piece = stringToPointBox(value, m_domain, ',', ':');
    if(refine_piece.null()) {
      m_refine_regions.pop_back();
      return(false);
    }
    m_refine_pieces.push_back(refine_piece);
  }
  else if(param == "refine_clear") {
    m_refine_regions.clear();
    m_refine_pieces.clear();
  }
  else if(param == "refine_point") {
    IvPBox refine_point = stringToPointBox(value, m_domain, ',', ':');
    if(refine_point.null() || !refine_point.isPtBox())
      return(false);
    m_refine_points.push_back(refine_point);
  }
  else if((param=="smart_amount")||(param=="priority_amt")) {
    int smart_amount = atoi(value.c_str());
    if(!isNumber(value) || (smart_amount < 0))
      return(false);
    m_smart_amount = smart_amount;
  }
  else if(param == "smart_percent") {
    int smart_percent = atoi(value.c_str());
    if(!isNumber(value) || (smart_percent < 0))
      return(false);
    m_smart_percent = smart_percent;
  }
  else if((param=="smart_thresh")||(param=="priority_thresh")) {
    double smart_thresh = atof(value.c_str());
    if(!isNumber(value) || (smart_thresh < 0))
      return(false);
    m_smart_thresh = smart_thresh;
  }
  else if(param == "auto_peak") {
    if((value != "true") && (value != "false"))
      return(false);
    m_auto_peak = (value == "true");
  }
  else
    return(false);
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: setParam

bool OF_Reflector::setParam(string param, int value)
{
  param = tolower(stripBlankEnds(param));
  
  if((param=="uniform_amount")||(param=="uniform_amt")) {
    if(value < 1)
      return(false);
    m_uniform_amount = value;
  }
  else if((param=="smart_amount")||(param=="priority_amt")) {
    if(value < 0)
      return(false);
    m_smart_amount = value;
  }
  else if(param == "smart_percent") {
    if(value < 0)
      return(false);
    m_smart_percent = value;
  }
  else
    return(false);
  
  return(true);
}

//-------------------------------------------------------------
// Procedure: create

int OF_Reflector::create(int unif_amt, int smart_amt, 
			 double smart_thresh)
{
  clearPDMap();
  if(!m_aof)
    return(0);
  
  if(unif_amt >= 0)
    m_uniform_amount = unif_amt;
  if(smart_amt >= 0)
    m_smart_amount = smart_amt;
  if(smart_thresh >= 0)
    m_smart_thresh = smart_thresh;

  // =============  Stage 1 - Uniform Pieces ======================
  // Make the initial uniform function based on the specified piece.
  // If no piece specified, base it on specified amount, default=1.
  int qlevels = 0;
  if((m_smart_amount > 0) || (m_smart_percent > 0))
    qlevels = 8;
  
  PQueue pqueue(qlevels);
  m_pqueue = pqueue;
  
  // Make a local copy of the uniform piece member variable
  IvPBox uniform_piece = m_uniform_piece;
  
  if(m_uniform_piece.null())
    uniform_piece = genUnifBox(m_domain, m_uniform_amount);

  // Now check that our local copy, however made, is not null
  if(!uniform_piece.null())
    m_pdmap = m_rt_uniform->create(&uniform_piece, 0, m_pqueue);
  
  m_uniform_piece_str = "count:";

  if(m_pdmap) 
    m_uniform_piece_str += intToString(m_pdmap->size());
  else
    m_uniform_piece_str += "0";
  
  if((int)(m_domain.size()) == uniform_piece.getDim()) {
    unsigned int dsize = m_domain.size();
    for(unsigned int i=0; i<dsize; i++) {
      int len = uniform_piece.pt(i,1) + 1;
      m_uniform_piece_str += ", ";
      m_uniform_piece_str += (m_domain.getVarName(i) + ":");
      m_uniform_piece_str += intToString(len);
    }
  }


  if(!m_pdmap)  // This should never happen, but check anyway.
    return(0);

  // =============  Stage 2 - Directed Refinement ================
  
  int pcs_before_dref = m_pdmap->size();
  
  int i; 
  int reg_size = m_refine_regions.size();
  int pce_size = m_refine_pieces.size();
  if(reg_size > pce_size)
    reg_size = pce_size;
  for(i=0; i<reg_size; i++) {
    IvPBox region = m_refine_regions[i];
    IvPBox resbox = m_refine_pieces[i];
    PDMap *new_pdmap = m_rt_focus->create(m_pdmap, region, 
					  resbox, m_pqueue);
    if(new_pdmap != 0)
      m_pdmap = new_pdmap;
  }
  
  int pcs_after_dref = m_pdmap->size();
  int pcs_made_dref = pcs_after_dref - pcs_before_dref;

  m_uniform_piece_str += ", dref_pcs:";
  m_uniform_piece_str += (intToString(pcs_made_dref) + ", ");

  if(!m_pdmap)  // This should never happen, but check anyway.
    return(0);

  // =============  Stage 3 - Smart Refinement ================

  int pcs_before_sref = m_pdmap->size();
  
  if(!m_pqueue.null()) {
    if((m_smart_amount > 0) || (m_smart_percent > 0)) {
      int  psize   = m_pdmap->size();
      int  use_amt = m_smart_amount;
      int  pct_amt = (psize * m_smart_percent) / 100;
      if(pct_amt > m_smart_amount)
	use_amt = pct_amt;    
      PDMap *new_pdmap = m_rt_priority->create(m_pdmap, m_pqueue, use_amt, 
					       m_smart_thresh);
      if(new_pdmap != 0)
	m_pdmap = new_pdmap;
    }
  }

  int pcs_after_sref = m_pdmap->size();
  int pcs_made_sref = pcs_after_sref - pcs_before_sref;

  m_uniform_piece_str += ", sref_pcs:";
  m_uniform_piece_str += (intToString(pcs_made_sref));
  
  if(m_pdmap)
    return(m_pdmap->size());
  else
    return(0);
}
