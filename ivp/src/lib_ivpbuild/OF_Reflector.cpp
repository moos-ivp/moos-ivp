/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: OF_Reflector.cpp                                     */
/*    DATE: Aug 29th 2005 (derived from OFR_AOF built long ago)  */
/*                                                               */
/* This file is part of IvP Helm Core Libs                       */
/*                                                               */
/* IvP Helm Core Libs is free software: you can redistribute it  */
/* and/or modify it under the terms of the Lesser GNU General    */
/* Public License as published by the Free Software Foundation,  */
/* either version 3 of the License, or (at your option) any      */
/* later version.                                                */
/*                                                               */
/* IvP Helm Core Libs is distributed in the hope that it will    */
/* be useful but WITHOUT ANY WARRANTY; without even the implied  */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the Lesser GNU General Public License for more   */
/* details.                                                      */
/*                                                               */
/* You should have received a copy of the Lesser GNU General     */
/* Public License along with MOOS-IvP.  If not, see              */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include "OF_Reflector.h"
#include "BuildUtils.h"
#include "IvPFunction.h"
#include "Regressor.h"
#include "RT_Uniform.h"
#include "RT_UniformX.h"
#include "RT_Smart.h"
#include "RT_Directed.h"
#include "RT_Evaluator.h"
#include "RT_AutoPeak.h"
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
  m_rt_uniformx = new RT_UniformX(m_regressor);
  m_rt_directed = new RT_Directed;
  m_rt_evaluator = new RT_Evaluator(m_regressor);
  m_rt_smart    = new RT_Smart(m_regressor);
  m_rt_autopeak = new RT_AutoPeak(m_regressor);
  
  m_uniform_amount = 1;
  m_smart_amount   = 0;
  m_smart_percent  = 0;
  m_smart_thresh   = 0;
  m_auto_peak      = false;
  m_qlevels        = 8;

  m_pcheck_thresh  = 0.001;
  
  m_verbose = false;
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
  delete(m_rt_uniformx);
  delete(m_rt_smart);
  delete(m_rt_directed);
  delete(m_rt_autopeak);
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
// Procedure: checkPlateaus()
//   Purpose: Will check all plateau pieces by generating a point
//            box for each point in the piece, and evaluate that 
//            point to ensure it evaluates to the max value of
//            underlying function (aof).

double OF_Reflector::checkPlateaus(bool verbose) const
{
  // Part 1: Sanity Checks
  if(m_plateaus.size() == 0) 
    return(0);
  if(!m_aof || !m_aof->minMaxKnown())
    return(1);

  double maxval = m_aof->getKnownMax();

  double worst_fail = 0;

  // Part 2: Check all the point boxes for each plateau
  for(unsigned int i=0; i<m_plateaus.size(); i++) {
    vector<IvPBox> pboxes = getPointBoxes(m_plateaus[i]);
    for(unsigned int j=0; j<pboxes.size(); j++) {
      double pval = m_aof->evalBox(&pboxes[j]);
      double delta = pval - maxval;
      if(delta < 0)
	delta *= -1;
      if(delta > m_pcheck_thresh) {
	if(delta > worst_fail)
	  worst_fail = delta;
	if(verbose) {
	  pboxes[j].print();
	  double pval = m_aof->evalBox(&pboxes[j]);
	  cout << " --> max:" << maxval << ", ptval:" << pval << ", delta:"
	       << delta << endl;
	}
      }
    }
  }
  return(worst_fail);
}


//-------------------------------------------------------------
// Procedure: checkBasins()
//   Purpose: Will check all basin pieces by generating a point
//            box for each point in the piece, and evaluate that 
//            point to ensure it evaluates to the MIN value of
//            underlying function (aof).

double OF_Reflector::checkBasins(bool verbose) const
{
  // Part 1: Sanity checks
  if(m_basins.size() == 0) 
    return(0);
  if(!m_aof || !m_aof->minMaxKnown())
    return(1);

  double minval = m_aof->getKnownMax();

  double worst_fail = 0;

  // Part 2: Check all the point boxes for each basin
  for(unsigned int i=0; i<m_basins.size(); i++) {
    vector<IvPBox> pboxes = getPointBoxes(m_basins[i]);
    for(unsigned int j=0; j<pboxes.size(); j++) {
      double pval = m_aof->evalBox(&pboxes[j]);
      double delta = pval - minval;
      if(delta < 0)
	delta *= -1;
      if(delta > m_pcheck_thresh) {
	if(delta > worst_fail)
	  worst_fail = delta;
	if(verbose) {
	  pboxes[j].print();
	  double pval = m_aof->evalBox(&pboxes[j]);
	  cout << " --> min:" << minval << ", ptval:" << pval << ", delta:"
	       << delta << endl;
	}
      }
    }
  }
  return(worst_fail);
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
  if(params != "") {
    bool ok = setParam(params);
    if(!ok)
      return(0);
  }
  return(create());
}
    
//-------------------------------------------------------------
// Procedure: setParam

bool OF_Reflector::setParam(string str)
{
  if(m_verbose) {
    cout << "------------------------------------------------" << endl;
    cout << "Beginning OF_Reflector::setParam(string)        " << endl;
    cout << "param string: [" << str << "]" << endl;
  }
  
  bool ok = true;
  str = tolower(stripBlankEnds(str));

  vector<string> svector = parseString(str, '#');
  unsigned int vsize = svector.size();

  if(vsize == 0)
    return(addWarning("setParam(string) Empty list of param/value pairs"));
  
  for(unsigned int i=0; i<vsize; i++) {
    if(m_verbose)
      cout << "  Parameter line: [" << i << "]: " << svector[i] << endl;
    svector[i] = stripBlankEnds(svector[i]);
    vector<string> tvector = parseString(svector[i], '=');
    if(tvector.size() == 2) {
      string param = stripBlankEnds(tvector[0]);
      string value = stripBlankEnds(tvector[1]);

      ok = ok && setParam(param, value);
    }
  }

  if(m_verbose) {
    cout << "Total Refine Regions: " << m_refine_regions.size() << endl;
    cout << "Total Refine Pieces:  " << m_refine_pieces.size() << endl;
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
  if(m_verbose) {
    cout << "-------------------------------------------------" << endl;
    cout << "Beginning OF_Reflector::setParam(string, string) " << endl;
    cout << "param: [" << param << "], value:" << value << endl;
  }
  
  param = tolower(stripBlankEnds(param));
  value = tolower(stripBlankEnds(value));

  int ival = atoi(value.c_str());

  if((param == "strict_range") && isBoolean(value)) {
    if(m_regressor)
      m_regressor->setStrictRange(tolower(value) == "true");
  }
  else if((param=="uniform_amount") || (param=="uniform_amt")) {
    if(!isNumber(value) || (ival < 1))  
      return(addWarning(param + " value must be >= 1"));
    m_uniform_amount = ival;
  }
  else if((param=="queue_levels") && isNumber(value)) {
    if(ival < 1)
      return(addWarning(param + " value must be >= 1"));
    m_qlevels = ival;
  }
  else if((param=="uniform_piece")||(param=="uniform_box")) {
    IvPBox foo = stringToPointBox(value, m_domain, ',', ':');
    m_uniform_piece = foo;
    if(m_uniform_piece.null()) 
      return(addWarning(param + " value is ill-defined"));
  }
  else if(param=="uniform_grid") {
    m_uniform_grid = stringToPointBox(value, m_domain, ',', ':');
    if(m_uniform_grid.null()) 
      return(addWarning(param + " value is ill-defined"));
  }
  else if(param=="plateau_region") {
    IvPBox plateau_region = stringToRegionBox(value, m_domain, ',', ':');
    return(setParam("plateau_region", plateau_region));
  }
  else if(param=="basin_region") {
    IvPBox basin_region = stringToRegionBox(value, m_domain, ',', ':');
    return(setParam("basin_region", basin_region));
  }
  else if((param=="refine_region") || (param=="focus_region")) {
    IvPBox refine_region = stringToRegionBox(value, m_domain, ',', ':');
    return(setParam("refine_region", refine_region));
  }
  else if((param=="refine_piece") || (param=="focus_box")) {
    //IvPBox refine_piece = stringToRegionBox(value, m_domain, ',', ':');
    IvPBox refine_piece = stringToPointBox(value, m_domain, ',', ':');
    return(setParam("refine_piece", refine_piece));
  }
  else if(param == "refine_clear") {
    m_refine_regions.clear();
    m_refine_pieces.clear();
  }
  else if(param == "refine_point") {
    IvPBox refine_point = stringToPointBox(value, m_domain, ',', ':');
    if(refine_point.null() || !refine_point.isPtBox())
      return(addWarning(param + " value is ill-defined"));
    m_refine_points.push_back(refine_point);
  }
  else if((param=="smart_amount") && isNumber(value)) {
    if(ival < 0) 
      return(addWarning(param + " value must be >= 0"));
    m_smart_amount = ival;
  }
  else if((param == "smart_percent") && isNumber(value)) {
    if(ival < 0) 
      return(addWarning("smart_percent value must be >= 0"));
    m_smart_percent = ival;
  }
  else if((param=="smart_thresh") && isNumber(value)) {
    double smart_thresh = atof(value.c_str());
    if(smart_thresh < 0) 
      return(addWarning(param + " value must be >= 0"));
    m_smart_thresh = smart_thresh;
  }
  else if(param == "auto_peak") {
    if((value != "true") && (value != "false")) 
      return(addWarning("auto_peak value must be true/false"));
    m_auto_peak = (value == "true");
  }
  else if((param == "auto_peak_max_pcs") && isNumber(value)) {
    if(ival <= 0) 
      return(addWarning("auto_peak_max_pcs value must be > 0"));
    m_auto_peak_max_pcs = ival;
  }
  else 
    return(addWarning(param + ": unhandled parameter"));

  return(true);
}

//-------------------------------------------------------------
// Procedure: setParam

bool OF_Reflector::setParam(string param, double value)
{
  if(m_verbose) {
    cout << "---------------------------------------------------" << endl;
    cout << "Beginning OF_Reflector::setParam(string, double)   " << endl;
    cout << "param: [" << param << "], value:" << value << endl;
  }
  
  param = tolower(stripBlankEnds(param));
  
  if((param=="uniform_amount") || (param=="uniform_amt")) {
    if(value < 1) 
      return(addWarning(param + " value must be >= 1"));
    m_uniform_amount = (int)(value);
  }
  else if((param=="smart_amount") || (param=="priority_amt")) {
    if(value < 0) 
      return(addWarning(param + " value must be >= 0"));
    m_smart_amount = (int)(value);
  }
  else if(param == "smart_percent") {
    if(value < 0) 
      return(addWarning(param + " value must be >= 0"));
    m_smart_percent = (int)(value);
  }
  else if(param=="smart_thresh") {
    if(value < 0) 
      return(addWarning(param + " value must be >= 0"));
    m_smart_thresh = value;
  }
  else if(param=="pcheck_thresh") {
    if((value < 0) || (value > 1)) 
      return(addWarning(param + " value must be in range [0,1]"));
    m_pcheck_thresh = value;
  }
  else 
    return(addWarning(param + ": undefined parameter"));
  
  return(true);
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

bool OF_Reflector::setParam(string param, IvPBox gbox)
{
  if(m_verbose) {
    cout << "-------------------------------------------------" << endl;
    cout << "Beginning OF_Reflector::setParam(string, IvPBox) " << endl;
    cout << "param: [" << param << endl;
    cout << "Box :" << endl;
    gbox.print();
  }

  param = tolower(stripBlankEnds(param));

  if((param=="uniform_piece")||(param=="uniform_box")) {
    m_uniform_piece = gbox;
    if(m_uniform_piece.null()) 
      return(addWarning(param + " box value is ill-defined"));
  }
  else if(param=="uniform_grid") {
    m_uniform_grid = gbox;
    if(m_uniform_grid.null())
      return(addWarning(param + " box value is ill-defined"));
  }
  else if(param=="plateau_region") {
    if(gbox.null()) 
      return(addWarning(param + " plateau_region value ill-defined"));
    m_plateaus.push_back(gbox);
  }
  else if(param=="basin_region") {
    if(gbox.null()) 
      return(addWarning(param + " basin_region value ill-defined"));
    m_basins.push_back(gbox);
  }
  else if((param=="refine_region") || (param=="focus_region")) {
    // Num refine regions must be equal or one less than num refine pieces
    if((m_refine_regions.size() != m_refine_pieces.size()) &&
       ((m_refine_regions.size() + 1) != m_refine_pieces.size())) {
      return(addWarning(param + " and refine_piece must be added in pairs"));
    }
    if(gbox.null()) 
      return(addWarning(param + " refine_region box value is ill-defined"));
    m_refine_regions.push_back(gbox);
  }
  else if((param=="refine_piece")||(param=="focus_box")) {
    // Num refine piecess must be equal or one less than num refine regions
    if((m_refine_regions.size() != m_refine_pieces.size()) &&
       ((m_refine_pieces.size() + 1) != m_refine_regions.size()))
      return(addWarning(param + " and refine_region must be added in pairs"));
    if(gbox.null())
      return(addWarning(param + " refine_piece box value is null/ill-defined"));
    m_refine_pieces.push_back(gbox);
  }
  else if(param == "refine_point") {
    IvPBox refine_point = gbox;
    if(refine_point.null() || !refine_point.isPtBox()) 
      return(addWarning(param + " box value is ill-defined"));
    m_refine_points.push_back(refine_point);
  }
  else 
    return(addWarning(param + ": undefined parameter"));

  return(true);
}

//-------------------------------------------------------------
// Procedure: create

int OF_Reflector::create(int unif_amt, int smart_amt, double smart_thresh)
{
  if(m_verbose) 
    cout << "========== Begin OF_Reflector::create() ==============" << endl;
    
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
  
  if(m_uniform_piece.null())
    m_uniform_piece = genUnifBox(m_domain, m_uniform_amount);
  if(m_uniform_grid.null())
    m_uniform_grid = m_uniform_piece;

  if(m_verbose)
    m_rt_uniformx->setVerbose();
  m_rt_uniformx->setPlateaus(m_plateaus);
  m_pdmap = m_rt_uniformx->create(m_uniform_piece, m_uniform_grid);

  if(!m_pdmap)  // This should never happen, but check anyway.
    return(0);

  // =============  Stage 2 - Directed Refinement ================

  int reg_size = m_refine_regions.size();
  int pce_size = m_refine_pieces.size();

  if(m_verbose) {
    cout << "Stage2: DR: reg_size: " << reg_size << endl;
    cout << "Stage2: DR: pce_size: " << pce_size << endl;
  }

  if(reg_size > pce_size)
    reg_size = pce_size;
  for(int i=0; i<reg_size; i++) {
    IvPBox region = m_refine_regions[i];
    IvPBox resbox = m_refine_pieces[i];
    PDMap *new_pdmap = m_rt_directed->create(m_pdmap, region, resbox);
    if(new_pdmap != 0)
      m_pdmap = new_pdmap;
  }
  
  if(!m_pdmap)  // This should never happen, but check anyway.
    return(0);

  // =============  Stage 3 - Evaluation ================

  int qlevels = 0;
  if((m_smart_amount > 0) || (m_smart_percent > 0))
    qlevels = m_qlevels;

  if(m_verbose) {
    cout << "Smart amount: " << m_smart_amount << endl; 
    cout << "Smart percent: " << m_smart_percent << endl; 
  }
  
  PQueue pqueue(qlevels);
  m_pqueue = pqueue;

  m_rt_evaluator->evaluate(m_pdmap, m_pqueue);  
  
  // =============  Stage 4 - Smart Refinement ================

  if(m_verbose) {
    cout << "m_pqueue.null(): " << m_pqueue.null() << endl;
  }
  
  if(!m_pqueue.null()) {
    if((m_smart_amount > 0) || (m_smart_percent > 0)) {
      int  psize   = m_pdmap->size();

      int  use_amt = m_smart_amount;
      int  pct_amt = (psize * m_smart_percent) / 100;
      if(pct_amt > m_smart_amount)
	use_amt = pct_amt;    

      if(m_verbose) {
	cout << "Amt prior to smart stage: " << psize << endl;
	cout << "Use Amount: " << use_amt << endl;
      }
	
      PDMap *new_pdmap = m_rt_smart->create(m_pdmap, m_pqueue, use_amt, 
					    m_smart_thresh);

      if(new_pdmap != 0)
	m_pdmap = new_pdmap;
    }
  }

  if(!m_pdmap)  // This should never happen, but check anyway.
    return(0);

  // =============  Stage 4 - AutoPeak Refinement ================

  if(m_auto_peak) {
    PDMap *new_pdmap = m_rt_autopeak->create(m_pdmap);
    if(new_pdmap != 0) 
      m_pdmap = new_pdmap;
  }

  if(m_verbose) {
    cout << "Total SetWts: " << m_regressor->getTotalSetWts() << endl;
    cout << "Total Evals:  " << m_regressor->getTotalEvals() << endl;
  }
  
  if(m_pdmap)
    return(m_pdmap->size());
  else
    return(0);
}


//-------------------------------------------------------------
// Procedure: addWarning

bool OF_Reflector::addWarning(string new_warning)
{
  if(m_warnings != "")
    m_warnings += " # ";
  m_warnings += new_warning;

  return(false);
}

//-------------------------------------------------------------
// Procedure: getMessage()

string OF_Reflector::getMessage(unsigned int ix) const
{
  if(m_regressor)
    return(m_regressor->getMessage(ix));
  return("");
}

//-------------------------------------------------------------
// Procedure: getMessageCnt()

unsigned int OF_Reflector::getMessageCnt() const
{
  if(m_regressor)
    return(m_regressor->getMessageCnt());
  return(0);
}

//-------------------------------------------------------------
// Procedure: getTotalEvals()

unsigned int OF_Reflector::getTotalEvals() const
{
  if(m_regressor)
    return(m_regressor->getTotalEvals());
  return(0);
}


//-------------------------------------------------------------
// Procedure: makeUniform()

void OF_Reflector::makeUniform()
{
  IvPDomain domain = m_regressor->getAOF()->getDomain();

  IvPBox universe = domainToBox(domain);

  int degree = m_regressor->getDegree();
  
  BoxSet *boxset = makeUniformDistro(universe, m_uniform_piece, degree);
  int vsize = boxset->size();
  if(vsize == 0)
    return;
  
  m_pdmap = new PDMap(vsize, domain, degree);
  BoxSetNode *bsn = boxset->retBSN(FIRST);
  int index = 0;
  while(bsn) {
    m_pdmap->bx(index) = bsn->getBox();
    index++;
    bsn = bsn->getNext();
  }
  delete(boxset);

  m_pdmap->setGelBox(m_uniform_grid);

  // Do later, after directed refinement done?
  // pdmap->updateGrid(1,1);
}

