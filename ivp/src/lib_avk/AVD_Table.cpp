/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AVD_Table.cpp                                        */
/*    DATE: November 8th, 2014                                   */
/*****************************************************************/

#include <iostream>
#include "AVD_Table.h"
#include "CurveFitting.h"

using namespace std;

AVD_Table::AVD_Table()
{
  // Initialize Config Variables
  m_v_min = 0;
  m_v_max = 0;
  m_v_gap = 0;
  m_h_max = 0;
  m_h_gap = 0;

  m_verbose = true;

  // Initialize State Variables
  m_rows  = 0;
  m_cols  = 0;
}


//----------------------------------------------------------------
// Procedure: init()
//
// avd_table[i][j] --> ith speed, jth heading
// avd_table[i]    --> ith row of the table
//
//                           HEADING
//         |------|------|------|------|------|------|
//         |      |      |      |      |      |      | 
//         |      |      |      |      |      |      | 
//         |------|------|------|------|------|------|
//         |      |      |      |      |      |      | 
//         |      |      |      |      |      |      | 
//         |------|------|------|------|------|------|
//  SPEED  |      |      |      |      |      |      | 
//         |      |      |      |      |      |      | 
//         |------|------|------|------|------|------|
//         |      |      |      |      |      |      | 
//         |      |      |      |      |      |      | 
//         |------|------|------|------|------|------|
//         |      |      |      |      |      |      | 
//         |      |      |      |      |      |      | 
//         |------|------|------|------|------|------|

//  180:18  delta=10, num=18, prod=180
//  181:18  delta=10, num=18, prod=180


bool AVD_Table::init(double v_min, double v_max, double v_gap,
		     double h_min, double h_max, double h_gap)
{
  if(m_verbose) {
    cout << "Initializing with: " << endl;
    cout << " v_min: " << v_max << endl;
    cout << " v_max: " << v_max << endl;
    cout << " v_gap: " << v_gap << endl;
    cout << " h_min: " << h_min << endl;
    cout << " h_max: " << h_max << endl;
    cout << " h_gap: " << h_gap << endl;
  }

  // Part 1: Basic Sanity checks, all args positive, all maxs >= gaps
  if((v_min < 0 ) || (v_min > v_max))
    return(retmsg("sanity check v_min failed"));
  if((v_max <= 0) || (v_gap <= 0))
    return(retmsg("sanity check v_max, v_gap failed"));
  if((h_max <= 0) || (h_gap <= 0))
    return(retmsg("sanity check h_max, h_gap failed"));
  if((h_min < -180) || (h_min > h_max)) 
    return(retmsg("sanity check h_min failed"));
  if((v_max < v_gap) || (h_max < h_gap))
    return(retmsg("sanity check gap vs max failed"));
    
  // Part 1A: Determine the number of speed rows
  unsigned int rows = (unsigned int) 
    snapToStep((v_max-v_min) / v_gap, .01) + 1;
  // Part 1B: Sanity check the speed init vars
  double v_product = ((double)(rows-1) * v_gap) + v_min;
  double v_mismatch = v_product - v_max;
  if(v_mismatch < 0)
    v_mismatch *= -1;
  if(v_mismatch > (0.001 * v_gap)) {
    cout << "v_mismatch: " << v_mismatch << endl;
    return(false);
  }  
  
  // Part 2A: Determine the number of heading cols
  unsigned int cols = (unsigned int)((h_max-h_min) / h_gap) + 1;

  cout << "cols: " << cols << endl;
  cout << "hmax: " << h_max << endl;

  // Part 2B: Sanity check the heading init vars
  double h_product = ((double)(cols-1) * h_gap) + h_min;
  double h_mismatch = h_product - h_max;
  if(h_mismatch < 0)
    h_mismatch *= -1;
  if(h_mismatch > (0.001 * h_gap)) {
    cout << "h_mismatch: " << h_mismatch << endl;
    return(false);
  }  

  // Part 3: Initialize the table
  AVD_Entry new_entry;
  vector<AVD_Entry> new_column(cols, new_entry);
  vector<vector<AVD_Entry> > new_avd_table(rows, new_column);

  m_avd_table = new_avd_table;

  for(unsigned int i=0; i<m_avd_table.size(); i++) {
    for(unsigned int j=0; j<m_avd_table[i].size(); j++) {
      m_avd_table[i][j].setSpd(((double)(i) * v_gap) + v_min);
      m_avd_table[i][j].setDeltaHdg(((double)(j) * h_gap) + h_min);
    }
  }

  m_v_min = v_min;
  m_v_max = v_max;
  m_v_gap = v_gap;
  m_h_min = h_min;
  m_h_max = h_max;
  m_h_gap = h_gap;

  m_rows = rows;
  m_cols = cols;
  
  return(true);
}



//----------------------------------------------------------------
// Procedure: addTurnSequ
      
bool AVD_Table::addTurnSequ(TurnSequence tseq)
{
  unsigned int spdix, hdgix;
  return(addTurnSequ(tseq, spdix, hdgix));
}

//----------------------------------------------------------------
// Procedure: addTurnSequ
      
bool AVD_Table::addTurnSequ(TurnSequence tseq, unsigned int& spdix, unsigned int& hdgix)
{
  // Part 1: Sanity check on the incoming turn sequence
  if(tseq.size() == 0)
    return(retmsg("AddTurnSequ failed: empty turn_sequence"));

  double spd = tseq.getSpeed();
  if((spd < m_v_min) || (spd > m_v_max))
    return(retmsg("AddTurnSequ failed: turn_seqence w/ speed out of range"));

  unsigned int spd_ix;
  bool ok_spd_ix = getSpdIX(spd, spd_ix);
  if(!ok_spd_ix)
    return(retmsg("AddTurnSequ failed: unable to obtain speed index for " + doubleToString(spd,2)));
  
  double hdg = tseq.getDeltaHeading();
  if((hdg < m_h_min) || (hdg > m_h_max))
    return(retmsg("AddTurnSequ failed: dhdg " + doubleToString(hdg,0) + " out of range"));

  unsigned int hdg_ix;
  bool ok_hdg_ix = getHdgIX(hdg, hdg_ix);
  if(!ok_hdg_ix) {
    cout << "heading: " << hdg << endl;
    return(retmsg("AddTurnSequ failed: unable to obtain delta heading index"));
  }
  // Part 2: Valid Turn Sequence, so now just add
  bool ok_add = m_avd_table[spd_ix][hdg_ix].setTurnSequ(tseq);
  if(!ok_add)
    return(retmsg("AddTurnSequ failed: could not add tseq to AVD_Entry"));

  // Part 3: Try to fit TP and build PWL
  string shdg = doubleToString(hdg,0);
  string sspd = doubleToString(spd,1);
  if(!m_avd_table[spd_ix][hdg_ix].fitPolynomial())
    return(retmsg("Failed to fit TP at "+sspd+"/"+shdg));
  if(!m_avd_table[spd_ix][hdg_ix].buildPieceWise())
    return(retmsg("Failed to build PWL at "+sspd+"/"+shdg));

  spdix = spd_ix;
  hdgix = hdg_ix;
  return(true);
}


//----------------------------------------------------------------
// Procedure: getEntry()
      
AVD_Entry AVD_Table::getEntry(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols)) {
    AVD_Entry null_entry;
    return(null_entry);
  }
  
  return(m_avd_table[i][j]);  
}

//----------------------------------------------------------------
// Procedure: turnSequSet
      
bool AVD_Table::turnSequSet(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols))
    return(false);
  
  return(m_avd_table[i][j].turnSequSet());  
}

//----------------------------------------------------------------
// Procedure: turnPolySet
      
bool AVD_Table::turnPolySet(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols))
    return(false);
  
  return(m_avd_table[i][j].turnPolySet());  
}

//----------------------------------------------------------------
// Procedure: isInterpolated
      
bool AVD_Table::isInterpolated(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols))
    return(false);
  
  return(m_avd_table[i][j].isInterpolated());  
}


//----------------------------------------------------------------
// Procedure: getTurnSpd
      
double AVD_Table::getTurnSpd(unsigned int i) const
{
  if((i >= m_rows) || (m_cols == 0))
    return(false);
  
  return(m_avd_table[i][0].getSpd());  
}

//----------------------------------------------------------------
// Procedure: getTurnDur
      
double AVD_Table::getTurnDur(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols))
    return(0);
  
  return(m_avd_table[i][j].getTurnDur());  
}


//----------------------------------------------------------------
// Procedure: getDHGoal
      
double AVD_Table::getDHGoal(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols))
    return(0);

  return(m_avd_table[i][j].getDeltaHdgGoal());  
}

//----------------------------------------------------------------
// Procedure: getDHSeen
      
double AVD_Table::getDHSeen(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols))
    return(0);
  
  return(m_avd_table[i][j].getDeltaHdgSeen());  
}

//----------------------------------------------------------------
// Procedure: getCoeffs
      
string AVD_Table::getCoeffs(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols))
    return("");
  
  vector<double> coeffs = m_avd_table[i][j].getTurnPoly().getPolyCoefficients();

  string str_coeffs;
  for(unsigned int i=0; i<coeffs.size(); i++) {
    if(i > 0)
      str_coeffs += ", ";
    str_coeffs += doubleToString(coeffs[i], 2);
  }
  return(str_coeffs);
}

//----------------------------------------------------------------
// Procedure: getSegListRaw
      
XYSegList AVD_Table::getSegListRaw(unsigned int i, unsigned int j) const
{
  XYSegList null_segl;
  if((i >= m_rows) || (j >= m_cols))
    return(null_segl);
  
  return(m_avd_table[i][j].getTurnSeglRaw());  
}

//----------------------------------------------------------------
// Procedure: getSegListLSQ
      
XYSegList AVD_Table::getSegListLSQ(unsigned int i, unsigned int j) const
{
  XYSegList null_segl;
  if((i >= m_rows) || (j >= m_cols))
    return(null_segl);
  
  return(m_avd_table[i][j].getTurnSeglLSQ());  
}

//----------------------------------------------------------------
// Procedure: getSegListPCW
      
XYSegList AVD_Table::getSegListPWL(unsigned int i, unsigned int j) const
{
  XYSegList null_segl;
  if((i >= m_rows) || (j >= m_cols))
    return(null_segl);
  
  return(m_avd_table[i][j].getTurnSeglPWL());  
}
//----------------------------------------------------------------
// Procedure: getSegListRay
      
XYSegList AVD_Table::getSegListRay(unsigned int i, unsigned int j) const
{
  XYSegList null_segl;
  if((i >= m_rows) || (j >= m_cols))
    return(null_segl);
  
  return(m_avd_table[i][j].getTurnSeglRay());  
}

//----------------------------------------------------------------
// Procedure: getTurnSpdFromIX
      
double AVD_Table::getTurnSpdFromIX(unsigned int ix) const
{
  if(ix >= m_rows)
    return(0);
  
  return(m_v_min + (ix * m_v_gap));
}

//----------------------------------------------------------------
// Procedure: getDeltHdgFromIX
      
double AVD_Table::getDeltHdgFromIX(unsigned int ix) const
{
  if(ix >= m_cols)
    return(0);
  
  return(m_h_min + (ix * m_h_gap));
}


//----------------------------------------------------------------
// Procedure: getSpdIX

unsigned int AVD_Table::getSpdIX(double pct) const
{
  if(pct < 0)
    pct = 0;
  else if(pct > 1)
    pct = 1;
  
  double dval = pct * m_avd_table.size();
  unsigned int index = (unsigned int)(dval);
  return(index);
}

//----------------------------------------------------------------
// Procedure: getHdgIX

unsigned int AVD_Table::getHdgIX(double pct) const
{
  if(pct < 0)
    pct = 0;
  else if(pct > 1)
    pct = 1;
  
  if(m_avd_table.size() == 0)
    return(0);

  double dval = pct * m_avd_table[0].size();
  unsigned int index = (unsigned int)(dval);
  return(index);
}


//----------------------------------------------------------------
// Procedure: getGlobalMaxRawX()
      
double AVD_Table::getGlobalMaxRawX() const
{
  double max_so_far = 0;
  for(unsigned int i=0; i<m_rows; i++) {
    for(unsigned int j=0; j<m_rows; j++) {
      if(m_avd_table[i][j].turnSequSet()) {
	XYSegList turn_segl_raw = m_avd_table[i][j].getTurnSeglRaw();
	double max_x = turn_segl_raw.get_max_x();
	if(((i==0) && (j==0)) || (max_x > max_so_far))
	  max_so_far = max_x;
      }
    }
  }
  return(max_so_far);
}

//----------------------------------------------------------------
// Procedure: getGlobalMaxRawY()
      
double AVD_Table::getGlobalMaxRawY() const
{
  double max_so_far = 0;
  for(unsigned int i=0; i<m_rows; i++) {
    for(unsigned int j=0; j<m_rows; j++) {
      if(m_avd_table[i][j].turnSequSet()) {
	XYSegList turn_segl_raw = m_avd_table[i][j].getTurnSeglRaw();
	double max_y = turn_segl_raw.get_max_y();
	if(((i==0) && (j==0)) || (max_y > max_so_far))
	  max_so_far = max_y;
      }
    }
  }
  return(max_so_far);
}

//----------------------------------------------------------------
// Procedure: incEndIX()
      
void AVD_Table::incEndIX(unsigned int i, unsigned int j)
{
  if((i >= m_rows) || (j >= m_cols))
    return;
  m_avd_table[i][j].incEndIX();  
}

//----------------------------------------------------------------
// Procedure: decEndIX()
      
void AVD_Table::decEndIX(unsigned int i, unsigned int j)
{
  if((i >= m_rows) || (j >= m_cols))
    return;
  m_avd_table[i][j].decEndIX();  
}

//----------------------------------------------------------------
// Procedure: incLookIX()
      
void AVD_Table::incLookIX(unsigned int i, unsigned int j)
{
  if((i >= m_rows) || (j >= m_cols))
    return;
  m_avd_table[i][j].incLookIX();  
}

//----------------------------------------------------------------
// Procedure: decLookIX()
      
void AVD_Table::decLookIX(unsigned int i, unsigned int j)
{
  if((i >= m_rows) || (j >= m_cols))
    return;
  m_avd_table[i][j].decLookIX();  
}

//----------------------------------------------------------------
// Procedure: getEndIX()
      
unsigned int AVD_Table::getEndIX(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols))
    return(0);
  return(m_avd_table[i][j].getEndIX());  
}

//----------------------------------------------------------------
// Procedure: getLookIX()
      
unsigned int AVD_Table::getLookIX(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols))
    return(0);
  return(m_avd_table[i][j].getLookIX());
}

//----------------------------------------------------------------
// Procedure: getLookX()
      
double AVD_Table::getLookX(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols))
    return(0);
  return(m_avd_table[i][j].getLookX());
}

//----------------------------------------------------------------
// Procedure: getLookY()

double AVD_Table::getLookY(unsigned int i, unsigned int j) const
{
  if((i >= m_rows) || (j >= m_cols))
    return(0);
  return(m_avd_table[i][j].getLookY());
}


//----------------------------------------------------------------
// Procedure: print()
      
void AVD_Table::print(bool terse) const
{
  unsigned int total_segls = 0;
  unsigned int total_sequs = 0;
  
  cout << "AVD_Table: rows=" << m_rows << ", cols=" << m_cols << endl;
  cout << "v_max=" << m_v_max << "  v_gap:" << m_v_gap << endl;
  cout << "h_max=" << m_h_max << "  h_gap:" << m_h_gap << endl;

  if(!terse) {
    for(unsigned int i=0; i<m_avd_table.size(); i++) {
      cout << "  Row[" << i << "]:" << endl;
      for(unsigned int j=0; j<m_avd_table[i].size(); j++) 
	cout << m_avd_table[i][j].getSummary(18);
      cout << endl;
    }
    cout << endl;
  }

  //  for(unsigned int i=0; i<m_avd_table.size(); i++) {
  //  for(unsigned int j=0; j<m_avd_table[i].size(); j++) {
  for(unsigned int i=0; i<m_rows; i++) {
    for(unsigned int j=0; j<m_cols; j++) {
      if(m_avd_table[i][j].turnSequSet())
	total_sequs++;
      if(m_avd_table[i][j].turnSeglSet())
	total_segls++;
    }
  }
  
  cout << "Total TurnSequs: " << total_sequs << endl;
  cout << "Total TurnSegls: " << total_segls << endl;
}


//----------------------------------------------------------------
// Procedure: interpolateAll()
      
bool AVD_Table::interpolateAll()
{
  bool all_interps_ok = true;
  for(unsigned int i=0; i<m_rows; i++) {
    for(unsigned int j=0; j<m_cols; j++) {
      if(m_avd_table[i][j].turnPolySet() == false) {
	bool ok = cellInterpolate(i, j);
	if(!ok)
	  all_interps_ok = false;
      }
    }
  }
  return(all_interps_ok);
}

//----------------------------------------------------------------
// Procedure: pcwiseBuildAll()

bool AVD_Table::pcwiseBuildAll()
{
  bool all_pcwise_ok = true;
  for(unsigned int i=0; i<m_rows; i++) {
    for(unsigned int j=0; j<m_cols; j++) {
      bool ok = cellPWLBuild(i, j);
      if(!ok)
	all_pcwise_ok = false;
    }
  }
  return(all_pcwise_ok);
}

//----------------------------------------------------------------
// Procedure: cellInterpolate()
//   Returns: true if the cell polynomial is set, regardless of whether 
//            it is set here, or already previously set.      

bool AVD_Table::cellInterpolate(unsigned int spd_ix, unsigned int hdg_ix)
{
  // Part 1 - Sanity Check
  if(m_avd_table[spd_ix][hdg_ix].turnPolySet())
    return(true);

  // Part 2 - Do the interpolation if possible
  AVD_Entry entry_a;
  AVD_Entry entry_b;
  double pct;

  bool able_to_interp = findInterpEntries(spd_ix, hdg_ix, 
					  entry_a, entry_b, pct);
  if(!able_to_interp)
    return(false);

  // Part 3 - Use the results of the interpolation to get the two
  //          polynomials and coefficents to interp between
  TurnPoly turn_poly_a = entry_a.getTurnPoly();
  TurnPoly turn_poly_b = entry_b.getTurnPoly();

  double new_hdg = getDeltHdgFromIX(hdg_ix);
  double new_spd = getTurnSpdFromIX(spd_ix);

  TurnPoly new_poly;
  bool interp_ok = turnPolyInterpolator(turn_poly_a, turn_poly_b,
					new_hdg, new_spd,new_poly);
  if(!interp_ok)
    return(false);

  m_avd_table[spd_ix][hdg_ix].setInterpDirect();
  
  m_avd_table[spd_ix][hdg_ix].setTurnPoly(new_poly, "direct");

  return(true);
}

//----------------------------------------------------------------
// Procedure: cellPWLBuild()

bool AVD_Table::cellPWLBuild(unsigned int spd_ix, unsigned int hdg_ix)
{
  // Part 1 - Sanity Check 
  if(!m_avd_table[spd_ix][hdg_ix].turnPolySet())
    return(false);

  m_avd_table[spd_ix][hdg_ix].buildPieceWise();

  return(true);
}

//----------------------------------------------------------------
// Procedure: findInterpEntries()
      
bool AVD_Table::findInterpEntries(unsigned int spd_ix, unsigned int hdg_ix,
				  AVD_Entry& entry_a, AVD_Entry& entry_b, 
				  double& pct)
{
  // Part 1: First try to find two filled-in cells with higher and 
  // lower heading indices
  
  // Part 1A: First try to find a filled-in cell with a higher index
  unsigned int high_hdg_ix = 0;
  bool found_high_hdg_ix = false;
  for(unsigned int i=hdg_ix+1; ((i<m_cols) && !found_high_hdg_ix); i++) {
    if(m_avd_table[spd_ix][i].turnPolySet()) {
      high_hdg_ix = i;
      found_high_hdg_ix = true;
    }
  }

  // Part 1B: Next try to find a filled-in cell with a lower index
  unsigned int low_hdg_ix = 0;
  bool found_low_hdg_ix = false;
  if(found_high_hdg_ix && (hdg_ix > 0)) {
    for(int i=hdg_ix-1; ((i>=0) && !found_low_hdg_ix); i--) {
      if(m_avd_table[spd_ix][(unsigned int)(i)].turnPolySet() &&
	 !m_avd_table[spd_ix][(unsigned int)(i)].isInterpolated()) {
	low_hdg_ix = (unsigned int)(i);
	found_low_hdg_ix = true;
      }
    }
  }
  
  if(found_high_hdg_ix && found_low_hdg_ix) {
    entry_a = m_avd_table[spd_ix][low_hdg_ix];
    entry_b = m_avd_table[spd_ix][high_hdg_ix];
    pct = (double)(hdg_ix-low_hdg_ix) / (double)(high_hdg_ix-low_hdg_ix);
    return(true);
  }

#if 1

  // Part 2: If two anchor cells cannot be found on the heading axis, 
  // try to find two filled-in cells with higher and lower speed indices
  
  // Part 2A: First try to find a filled-in cell with a higher spd index
  unsigned int high_spd_ix = 0;
  bool found_high_spd_ix = false;
  for(unsigned int i=spd_ix+1; ((i<m_rows) && !found_high_spd_ix); i++) {
    if(m_avd_table[i][hdg_ix].turnPolySet()) {
      high_spd_ix = i;
      found_high_spd_ix = true;
    }
  }

  // Part 2B: Next try to find a filled-in cell with a lower spd index
  unsigned int low_spd_ix = 0;
  bool found_low_spd_ix = false;
  if(found_high_spd_ix && (spd_ix > 0)) {
    for(int i=spd_ix-1; ((i>=0) && !found_low_spd_ix); i--) {
      if(m_avd_table[(unsigned int)(i)][hdg_ix].turnPolySet()) {
	low_spd_ix = i;
	found_low_spd_ix = true;
      }
    }
  }
  
  if(found_high_spd_ix && found_low_spd_ix) {
    entry_a = m_avd_table[low_spd_ix][hdg_ix];
    entry_b = m_avd_table[high_spd_ix][hdg_ix];
    pct = (double)(spd_ix-low_spd_ix) / (double)(high_spd_ix-low_spd_ix);
    return(true);
  }
#endif  

  return(false);
}


//----------------------------------------------------------------
// Procedure: getSpdIX()
//   Purpose: get the speed INDEX based on a given speed
//   Returns: true if given speed is in the range of the table      

bool AVD_Table::getSpdIX(double spd, unsigned int& index) const
{
  // Sanity Check 1 (speed must be in the range [min spd,max spd])
  if((spd < m_v_min) || (spd > m_v_max)) { 
   return(false);
  }
    
  //double d_ix_maybe = (spd / m_v_gap);
  double d_ix_maybe = snapToStep(((spd-m_v_min) / m_v_gap), 0.01);
  unsigned int ix_maybe = (unsigned int)(d_ix_maybe+0.5);

  // Sanity Check 2 (index must be within bounds w.r.t. number of rows)
  if(ix_maybe >= m_rows) {
    return(false);
  }
  
  // Sanity check 3 (given spd must reasonably correspond to a table spd)
  double spd_in_table = m_avd_table[ix_maybe][0].getSpd();
  double spd_mismatch = spd - spd_in_table;
  if(spd_mismatch < 0)
    spd_mismatch *= -1;
  if(spd_mismatch > (0.5 * m_v_gap)) {
    return(false);
  }
  
  index = ix_maybe;
  return(true);
  
}

//----------------------------------------------------------------
// Procedure: getHdgIX()
      
bool AVD_Table::getHdgIX(double hdg, unsigned int& index) const
{
  // Sanity Check 1: Make sure the table has been initialized
  if((m_rows == 0) || (m_cols == 0))
    return(false);

  // Sanity Check 2: Given hdg must be in range [m_h_min, m_h_max]
  if((hdg < m_h_min) || (hdg > m_h_max))
    return(false);

  unsigned int ix_maybe = (unsigned int)((hdg - m_h_min) / m_h_gap + 0.5);
  // Sanity Check 3: Index must be within bounds w.r.t. number of columns
  if(ix_maybe >= m_cols)
    return(false);
  
  // Sanity check 4: Given hdg must reasonably correspond to a table hdg
  double hdg_in_table = m_avd_table[0][ix_maybe].getDeltaHdgGoal();
  double hdg_mismatch = hdg - hdg_in_table;
  if(hdg_mismatch < 0)
    hdg_mismatch *= -1;
  //  if(hdg_mismatch > (0.01 * m_h_gap))  //original
  if(hdg_mismatch > (0.5 * m_h_gap))  // kw edit 29 nov 2014
    return(false);
  
  index = ix_maybe;
  return(true);  
}

//----------------------------------------------------------------
// Procedure: retmsg()
      
bool AVD_Table::retmsg(string message) const
{
  if(m_verbose)
    cout << message << endl;
  return(false);
}
  
//----------------------------------------------------------------
// Procedure: getSpec()
      
string AVD_Table::getSpec() const
{
  string result = "spd_gap=" + doubleToString(m_v_gap,1);
  result += "#spd_max=" + doubleToString(m_v_max,1);
  result += "#hdg_gap=" + doubleToString(m_h_gap,1);
  for(unsigned int i=0; i<m_avd_table.size(); i++) {
    for(unsigned int j=0; j<m_avd_table[i].size(); j++) {
      if(m_avd_table[i][j].turnPolySet()) {
	TurnPoly turn_poly = m_avd_table[i][j].getTurnPoly();
	string spec = turn_poly.getSpec();
	if(result != "")
	  result += "#";
	result += spec;
      }
    }
  }

  return(result);
}
  

