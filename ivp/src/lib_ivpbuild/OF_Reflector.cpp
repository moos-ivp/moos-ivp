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

#include <math.h>
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "IvPFunction.h"
#include "PQueue.h"
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
  
  m_uniform_amount = 0;
}

//-------------------------------------------------------------
// Procedure: Destructor
//      Note: the member variable "pdmap" is not freed since it 
//            it is common convention to use the pdmap as the 
//            product of the creation process, typically in a new
//            objective function - see OF_Reflector::pruneToOF()


OF_Reflector::~OF_Reflector()
{
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
// Procedure: createUniform
//   Purpose: Make a uniform IvP function based on the given amt
//            of max uniform boxes desired. First calculates the
//            largest uniform box possible given the domain. 
//            Further boxes will be created based on priority.

int OF_Reflector::createUniform(int max_amt, int qlevels)
{
  clearPDMap();
  if(!m_aof)
    return(0);

  IvPDomain domain = m_aof->getDomain();
  IvPBox unifbox = genUnifBox(domain, max_amt);
  
  m_pdmap = m_rt_uniform->create(&unifbox, &unifbox, qlevels);
  return(m_pdmap->size());
}

//-------------------------------------------------------------
// Procedure: createUniform
//   Purpose: Make a uniform IvP function based on the given box.
//            Each uniform piece will have the same size as the
//            given box.

int OF_Reflector::createUniform(const IvPBox *unifbox, 
				const IvPBox *gridbox, 
				int qlevels)
{
  clearPDMap();
  if(!m_aof)
    return(0);

  m_pdmap = m_rt_uniform->create(unifbox, gridbox, qlevels);
  return(m_pdmap->size());
}

//-------------------------------------------------------------
// Procedure: createPriority

int OF_Reflector::createPriority(int more_amt, double thresh)
{
  if(!m_pdmap)
    return(0);
  
  if(more_amt <= 0)
    return(m_pdmap->size());
  
  PQueue *pqueue = m_rt_uniform->getPQueue();
  if(!pqueue)
    return(m_pdmap->size());
  
  m_pdmap = m_rt_priority->create(m_pdmap, pqueue, more_amt, thresh);
  return(m_pdmap->size());
}

//-------------------------------------------------------------
// Procedure: createFocusRefine

int OF_Reflector::createFocusRefine(IvPBox region, IvPBox resbox)
{
  if(!m_pdmap)
    return(0);
  
  PDMap *new_pdmap = m_rt_focus->create(m_pdmap, region, resbox);
  if(new_pdmap != 0)
    m_pdmap = new_pdmap;
  
  return(m_pdmap->size());
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

int OF_Reflector::create(const string params)
{
  clearPDMap();
  if(!m_aof)
    return(0);

  // The possible paramaters - all initialized to "".
  string str_uniform_amt;
  string str_uniform_box; 
  string str_uniform_grid;
  string str_priority_amt;
  string str_priority_queue;
  string str_priority_thresh;
  string str_focus_region;
  string str_focus_refine;

  tokParse(params, "uniform_amt",  '#', '=', str_uniform_amt);
  tokParse(params, "uniform_box",  '#', '=', str_uniform_box);
  tokParse(params, "uniform_grid", '#', '=', str_uniform_grid);
  tokParse(params, "priority_amt", '#', '=', str_priority_amt);
  tokParse(params, "priority_queue",  '#', '=', str_priority_queue);
  tokParse(params, "priority_thresh", '#', '=', str_priority_thresh);
  tokParse(params, "focus_region", '#', '=', str_focus_region);
  tokParse(params, "focus_box",    '#', '=', str_focus_refine);

  str_uniform_amt  = stripBlankEnds(str_uniform_amt);
  str_uniform_box  = stripBlankEnds(str_uniform_box);
  str_uniform_grid = stripBlankEnds(str_uniform_grid);
  str_priority_amt = stripBlankEnds(str_priority_amt);
  str_priority_queue  = stripBlankEnds(str_priority_queue);
  str_priority_thresh = stripBlankEnds(str_priority_thresh);
  str_focus_region = stripBlankEnds(str_focus_region);
  str_focus_refine = stripBlankEnds(str_focus_refine);
  
  int unif_amt = 0;
  if(isNumber(str_uniform_amt))
    unif_amt = atoi(str_uniform_amt.c_str());

  int priority_amt = 0;
  if(isNumber(str_priority_amt))
    priority_amt = atoi(str_priority_amt.c_str());

  int qlevels = 8;
  if(isNumber(str_priority_queue)) {
    double pqueue_amt = atof(str_priority_queue.c_str());
    if(pqueue_amt < 2)
      qlevels = 0;
    else
      qlevels = (int)(log2(pqueue_amt));
  }

  double priority_thresh = 0.0;
  if(isNumber(str_priority_thresh)) {
     priority_thresh = atof(str_priority_thresh.c_str());
     if(priority_thresh < 0)
       priority_thresh = 0;
  }
  
  IvPDomain domain = m_aof->getDomain();

  IvPBox unif_box   = stringDiscreteToPointBox(str_uniform_box, domain, ',', ':');
  IvPBox grid_box   = stringDiscreteToPointBox(str_uniform_grid, domain, ',', ':');
  IvPBox region_box = stringFloatToPointBox(str_focus_region, domain, ',', ':');
  IvPBox refine_box = stringFloatToPointBox(str_focus_refine, domain, ',', ':');

  // Under this utility, a uniform PDMap must be create by either of the 
  // two methods, or else it will not proceed.
  if(unif_amt <= 0)
    if(unif_box.null())
      return(0);

  int pdmap_size = 0;
  
  if(unif_amt > 0)
    pdmap_size = createUniform(unif_amt, qlevels);
  else {
    // If the grid_box was not set, or if it doesn't subsume unif_box, then
    // proceed by just making the grid_box identical to the unif_box.
    if(grid_box.null() || (!containedWithinBox(unif_box, grid_box)))
      grid_box = unif_box;
    pdmap_size = createUniform(&unif_box, &grid_box, qlevels);
  }
  
  if(pdmap_size <= 0)
    return(0);

  if((pdmap_size > 0) && (priority_amt > 0))
    pdmap_size = createPriority(priority_amt, priority_thresh);
  
  if((pdmap_size > 0) && !region_box.null() && !refine_box.null())
    pdmap_size = createFocusRefine(region_box, refine_box);
  
  return(m_pdmap->size());
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
  else if(param == "uniform_amount") {
    int uniform_amount = atoi(value.c_str());
    if(!isNumber(value) || (uniform_amount < 1))
      return(false);
    m_uniform_amount = uniform_amount;
  }
  else if(param == "uniform_piece") {
    m_uniform_piece = stringToPointBox(value, m_domain, ',', ':');
    if(m_uniform_piece.null())
      return(false);
  }
  else if(param == "refine_region") {
    if(m_refine_regions.size() != m_refine_pieces.size())
      return(false);
    IvPBox refine_region = stringFloatToRegionBox(value, m_domain, ',', ':');
    if(refine_region.null())
      return(false);
  }
  else if(param == "refine_piece") {
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
  else if(param == "smart_amount") {
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
  else if(param == "smart_peak") {
    if((value != "true") && (value != "false"))
      return(false);
    m_smart_peak = (value == "true");
  }
  else
    return(false);
  
  return(true);
}


//"course:34.34:89.009, speed:all


