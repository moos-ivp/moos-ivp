/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AVK_Table.cpp                                        */
/*    DATE: Mar 25th, 2015                                       */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "SeglrUtils.h"
#include "AVK_Table.h"
#include "TurnPoly.h"
#include "CurveFitting.h"

using namespace std;

//----------------------------------------------------------------
// Constructor

AVK_Table::AVK_Table()
{
  m_configured   = false;
  m_interpolated = false;
}
  
//----------------------------------------------------------------
// Procedure: configure()

bool AVK_Table::configure(double hdg_gap, double spd_gap, double spd_max)
{
  bool ok1 = m_grid_coeffs.configure(hdg_gap, spd_gap, spd_max);
  bool ok2 = m_grid_xend.configure(hdg_gap, spd_gap, spd_max);
  bool ok3 = m_grid_time.configure(hdg_gap, spd_gap, spd_max);
  bool ok4 = m_grid_rotation.configure(hdg_gap, spd_gap, spd_max);
  bool ok5 = m_grid_seglrs.configure(hdg_gap, spd_gap, spd_max);
  bool ok6 = m_grid_interp.configure(hdg_gap, spd_gap, spd_max);

  if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || !ok6) 
    return(false);

  m_grid_interp.setAll(false);
  
  m_configured = true;
  return(true);
}

//----------------------------------------------------------------
// Procedure: reconfigure()

bool AVK_Table::reconfigure(double hdg_gap, double spd_gap, double spd_max)
{
  bool ok1 = m_grid_coeffs.reconfigure(hdg_gap, spd_gap, spd_max);
  bool ok2 = m_grid_xend.reconfigure(hdg_gap, spd_gap, spd_max);
  bool ok3 = m_grid_time.reconfigure(hdg_gap, spd_gap, spd_max);
  bool ok4 = m_grid_rotation.reconfigure(hdg_gap, spd_gap, spd_max);
  bool ok5 = m_grid_seglrs.reconfigure(hdg_gap, spd_gap, spd_max);
  bool ok6 = m_grid_interp.reconfigure(hdg_gap, spd_gap, spd_max);

  if(!ok1 || !ok2 || !ok3 || !ok4 || !ok5 || !ok6) 
    return(false);

  m_grid_interp.setAll(false);

  return(true);
}

//----------------------------------------------------------------
// Procedure: setCellCoeffs

bool AVK_Table::setCellCoeffs(double spd, double dh, vector<double> coeffs)
{
  bool ok = m_grid_coeffs.setCell(spd, dh, coeffs);

  return(ok);
}

//----------------------------------------------------------------
// Procedure: setCellXend

bool AVK_Table::setCellXend(double spd, double dh, double xend)
{
  bool ok = m_grid_xend.setCell(spd, dh, xend);

  return(ok);
}

//----------------------------------------------------------------
// Procedure: setCellInterp

bool AVK_Table::setCellInterp(double spd, double dh, bool interp)
{
  bool ok = m_grid_interp.setCell(spd, dh, interp);

  return(ok);
}

//----------------------------------------------------------------
// Procedure: setCellTime

bool AVK_Table::setCellTime(double spd, double dh, double time)
{
  bool ok = m_grid_time.setCell(spd, dh, time);

  return(ok);
}

//----------------------------------------------------------------
// Procedure: setCellRotation

bool AVK_Table::setCellRotation(double spd, double dh, double rotation)
{
  bool ok = m_grid_rotation.setCell(spd, dh, rotation);

  return(ok);
}

//----------------------------------------------------------------
// Procedure: interpolateAll()

bool AVK_Table::interpolateAll()
{
  unsigned int rows = m_grid_coeffs.getRows();
  unsigned int cols = m_grid_coeffs.getCols();

  bool all_interps_ok = true;
  for(unsigned int i=0; i<rows; i++) {
    for(unsigned int j=0; j<cols; j++) {
      if(m_grid_coeffs.cellSet(i, j) == false) {
	bool ok = cellInterpolate(i, j);
	if(!ok)
	  all_interps_ok = false;
      }
    }
  }
  return(all_interps_ok);

}


//----------------------------------------------------------------
// Procedure: cellInterpolate()
//   Returns: true if the cell polynomial is set, regardless of whether 
//            it is set here, or already previously set.      

bool AVK_Table::cellInterpolate(unsigned int spd_ix, unsigned int dh_ix)
{
  // Part 1 - Sanity Check
  if(m_grid_coeffs.cellSet(spd_ix, dh_ix)) 
    return(true);
  
  // Part 2 - Do the interpolation if possible
  unsigned int spd1_ix = 0;
  unsigned int dh1_ix  = 0;
  unsigned int spd2_ix = 0;
  unsigned int dh2_ix  = 0;
  double pct = 0;
  
  bool able_to_interp = findInterpNeigh(spd_ix, dh_ix, spd1_ix, dh1_ix, 
					spd2_ix, dh2_ix, pct);
  if(!able_to_interp)
    return(false);

  // Part 3 - Use the results of the interpolation to get the two
  //          polynomials and coefficents to interp between
  TurnPoly turn_poly_a;
  double spd1, dh1, rota1, xend1, time1;
  vector<double> coeffs1;
  bool ok1 = true;    
  ok1 = ok1 && m_grid_coeffs.getSpdVal(spd1_ix, spd1);
  ok1 = ok1 && m_grid_coeffs.getHdgVal(dh1_ix, dh1);
  ok1 = ok1 && m_grid_xend.getCellVal(spd1_ix, dh1_ix, xend1);
  ok1 = ok1 && m_grid_time.getCellVal(spd1_ix, dh1_ix, time1);
  ok1 = ok1 && m_grid_rotation.getCellVal(spd1_ix, dh1_ix, rota1);
  ok1 = ok1 && m_grid_coeffs.getCellVal(spd1_ix, dh1_ix, coeffs1);
  if(!ok1)
    return(false);

  turn_poly_a.setSpeed(spd1);
  turn_poly_a.setDeltaHeading(dh1);
  turn_poly_a.setPolyCoefficients(coeffs1);
  turn_poly_a.setRotationAngle(rota1);
  turn_poly_a.setXEnd(xend1);
  turn_poly_a.setTCutoff(time1);


  TurnPoly turn_poly_b;
  double spd2, dh2, rota2, xend2, time2;
  vector<double> coeffs2;
  bool ok2 = true;    
  ok2 = ok2 && m_grid_coeffs.getSpdVal(spd2_ix, spd2);
  ok2 = ok2 && m_grid_coeffs.getHdgVal(dh2_ix, dh2);
  ok2 = ok2 && m_grid_xend.getCellVal(spd2_ix, dh2_ix, xend2);
  ok2 = ok2 && m_grid_time.getCellVal(spd2_ix, dh2_ix, time2);
  ok2 = ok2 && m_grid_rotation.getCellVal(spd2_ix, dh2_ix, rota2);
  ok2 = ok2 && m_grid_coeffs.getCellVal(spd2_ix, dh2_ix, coeffs2);
  if(!ok2)
    return(false);

  turn_poly_b.setSpeed(spd2);
  turn_poly_b.setDeltaHeading(dh2);
  turn_poly_b.setPolyCoefficients(coeffs2);
  turn_poly_b.setRotationAngle(rota2);
  turn_poly_b.setXEnd(xend2);
  turn_poly_b.setTCutoff(time2);


  double new_hdg = 0;
  double new_spd = 0;

  bool ok_new_dh  = m_grid_coeffs.getHdgVal(dh_ix, new_hdg);
  bool ok_new_spd = m_grid_coeffs.getSpdVal(spd_ix, new_spd);
  if(!ok_new_dh || !ok_new_spd)
    return(false);

  TurnPoly new_poly;
  bool interp_ok = turnPolyInterpolator(turn_poly_a, turn_poly_b,
					new_hdg, new_spd, new_poly);
  if(!interp_ok)
    return(false);

  vector<double> coeffs = new_poly.getPolyCoefficients();
  double xend = new_poly.getXEnd();
  double time = new_poly.getTCutoff();
  double rota = new_poly.getRotationAngle();

  setCellCoeffs(new_spd, new_hdg, coeffs);
  setCellXend(new_spd, new_hdg, xend);
  setCellTime(new_spd, new_hdg, time);
  setCellRotation(new_spd, new_hdg, rota);
  setCellInterp(new_spd, new_hdg, true);

  return(true);
}



//----------------------------------------------------------------
// Procedure: findInterpNeigh
      
bool AVK_Table::findInterpNeigh(unsigned int spd_ix, unsigned int dh_ix,
				unsigned int& spd1_ix, unsigned int& dh1_ix,
				unsigned int& spd2_ix, unsigned int& dh2_ix,
				double& pct)
{
  unsigned int rows = m_grid_coeffs.getRows();
  unsigned int cols = m_grid_coeffs.getCols();

  // Part 1: First try to find two filled-in cells with higher and 
  // lower heading indices
  
  // Part 1A: First try to find a filled-in cell with a higher index
  unsigned int high_dh_ix = 0;
  bool found_high_dh_ix = false;
  for(unsigned int j=dh_ix+1; ((j<cols) && !found_high_dh_ix); j++) {
    if(m_grid_coeffs.cellSet(spd_ix, j)) {
      high_dh_ix = j;
      found_high_dh_ix = true;
    }
  }

  // Part 1B: Next try to find a filled-in cell with a lower index
  unsigned int low_dh_ix = 0;
  bool found_low_dh_ix = false;
  if(found_high_dh_ix && (dh_ix > 0)) {
    for(int j=dh_ix-1; ((j>=0) && !found_low_dh_ix); j--) {
      bool cell_already_set = m_grid_coeffs.cellSet(spd_ix, j);
      bool cell_interpolated = false;
      m_grid_interp.getCellVal(spd_ix, j, cell_interpolated);

      if(cell_already_set && !cell_interpolated) {
	low_dh_ix = (unsigned int)(j);
	found_low_dh_ix = true;
      }
    }
  }
  
  if(found_high_dh_ix && found_low_dh_ix) {
    spd1_ix = spd_ix;
    spd2_ix = spd_ix;
    dh1_ix  = low_dh_ix;
    dh2_ix  = high_dh_ix;
    pct = (double)(dh_ix-low_dh_ix) / (double)(high_dh_ix-low_dh_ix);
    return(true);
  }

  // Part 2: If two anchor cells cannot be found on the heading axis, 
  // try to find two filled-in cells with higher and lower speed indices
  
  // Part 2A: First try to find a filled-in cell with a higher spd index
  unsigned int high_spd_ix = 0;
  bool found_high_spd_ix = false;
  for(unsigned int i=spd_ix+1; ((i<rows) && !found_high_spd_ix); i++) {
    if(m_grid_coeffs.cellSet(i, dh_ix)) {
      high_spd_ix = i;
      found_high_spd_ix = true;
    }
  }

  // Part 2B: Next try to find a filled-in cell with a lower spd index
  unsigned int low_spd_ix = 0;
  bool found_low_spd_ix = false;
  if(found_high_spd_ix && (spd_ix > 0)) {
    for(int i=spd_ix-1; ((i>=0) && !found_low_spd_ix); i--) {
      bool cell_already_set = m_grid_coeffs.cellSet(i, dh_ix);
      bool cell_interpolated = false;
      m_grid_interp.getCellVal(i, dh_ix, cell_interpolated);

      if(cell_already_set && !cell_interpolated) {
	low_spd_ix = i;
	found_low_spd_ix = true;
      }
    }
  }
  
  if(found_high_spd_ix && found_low_spd_ix) {
    spd1_ix = low_spd_ix;
    spd2_ix = high_spd_ix;
    dh1_ix  = dh_ix;
    dh2_ix  = dh_ix;
    pct = (double)(spd_ix-low_spd_ix) / (double)(high_spd_ix-low_spd_ix);
    return(true);
  }

  return(false);
}



//----------------------------------------------------------------
// Procedure: pcwiseBuildAll()

bool AVK_Table::pcwiseBuildAll()
{
  unsigned int rows = m_grid_coeffs.getRows();
  unsigned int cols = m_grid_coeffs.getCols();

  for(unsigned int i=0; i<rows; i++) {
    for(unsigned int j=0; j<cols; j++) {

      double spd, dh;

      bool ok_spd = m_grid_coeffs.getSpdVal(i, spd);
      bool ok_hdg = m_grid_coeffs.getHdgVal(j, dh);
      if(!ok_spd || !ok_hdg)
	return(false);

      cout << "pcwisebuildall spd:" << spd << ", dh:" << dh << endl;
      bool set = m_grid_coeffs.cellSet(spd, dh);

      if(set) {
	cout << "[" << i << "][" << j << "]:" << endl;
	cout << "  spd=" << spd << ", dh=" << dh << endl;
	cout << "  set=" << set << endl;
	Seglr seglr = getSeglr(spd, dh);
	cout << "================================================" << endl;
	cout << "  " << seglrToString(seglr) << endl;
	cout << "================================================" << endl;
	m_grid_seglrs.setCell(spd, dh, seglr);
      }
      else {
	Seglr seglr(dh);
	seglr.addVertex(0,0);
	m_grid_seglrs.setCell(spd, dh, seglr);      
      }
    }
  }
  return(true);
}


//----------------------------------------------------------------
// Procedure: getAllSeglrs()

vector<Seglr> AVK_Table::getAllSeglrs() const
{
  vector<Seglr> seglrs;

  unsigned int rows = m_grid_seglrs.getRows();
  unsigned int cols = m_grid_seglrs.getCols();

  for(unsigned int i=0; i<rows; i++) {
    for(unsigned int j=0; j<cols; j++) {

      if(m_grid_seglrs.cellSet(i, j)) {
	cout << "Cell " << i << "," << j << " is set." << endl;
	Seglr seglr;
	bool ok = m_grid_seglrs.getCellVal(i, j, seglr);
	if(ok)
	  seglrs.push_back(seglr);
      }
      else
	cout << "Cell " << i << "," << j << " is NOT set." << endl;

    }
  }
  return(seglrs);
}

//----------------------------------------------------------------
// Procedure: getAllSpd()

vector<double> AVK_Table::getAllSpd() const
{
  vector<double> spd_vals;

  unsigned int rows = m_grid_seglrs.getRows();
  unsigned int cols = m_grid_seglrs.getCols();

  for(unsigned int i=0; i<rows; i++) {
    for(unsigned int j=0; j<cols; j++) {
      double spd;
      bool ok = m_grid_seglrs.getSpdVal(i, spd);
      if(ok)
	spd_vals.push_back(spd);
    }
  }
  return(spd_vals);
}

//----------------------------------------------------------------
// Procedure: getAllDH()

vector<double> AVK_Table::getAllDH() const
{
  vector<double> dh_vals;

  unsigned int rows = m_grid_seglrs.getRows();
  unsigned int cols = m_grid_seglrs.getCols();

  for(unsigned int i=0; i<rows; i++) {
    for(unsigned int j=0; j<cols; j++) {
      double dh;
      bool ok = m_grid_seglrs.getHdgVal(j, dh);
      if(ok)
	dh_vals.push_back(dh);
    }
  }
  return(dh_vals);
}

//----------------------------------------------------------------
// Procedure: getSeglrsSpd()

vector<Seglr> AVK_Table::getSeglrsSpd(double given_spd) const
{
  vector<Seglr> seglrs;

  unsigned int rows = m_grid_coeffs.getRows();
  unsigned int cols = m_grid_coeffs.getCols();

  unsigned int spd_ix=0;
  bool ok_ix = m_grid_seglrs.getSpdIX(given_spd, spd_ix);
  if(!ok_ix)
    return(seglrs);

  for(unsigned int i=0; i<rows; i++) {
    if(i == spd_ix) {
      for(unsigned int j=0; j<cols; j++) {
	if(m_grid_seglrs.cellSet(i, j)) {
	  Seglr seglr;
	  bool ok = m_grid_seglrs.getCellVal(i, j, seglr);
	  if(ok)
	    seglrs.push_back(seglr);
	}
      }
    }
  }
  return(seglrs);
}

//----------------------------------------------------------------
// Procedure: getSeglr

Seglr AVK_Table::getSeglr(double spd, double dh)
{
  Seglr seglr;
  vector<double> coeffs;
  double xend = 0;
  double time = 0;
  double rota = 0;

  // Get the coefficients corresponding to spd and dh
  bool ok1 = m_grid_coeffs.getCellVal(spd, dh, coeffs);
  bool ok2 = m_grid_xend.getCellVal(spd, dh, xend);
  bool ok3 = m_grid_time.getCellVal(spd, dh, time);
  bool ok4 = m_grid_rotation.getCellVal(spd, dh, rota);

  if(!ok1)
    cout << "Problem making a Seglr. Cannot retrieve coeffs." << endl;
  if(!ok2)
    cout << "Problem making a Seglr. Cannot retrieve xend." << endl;
  if(!ok3)
    cout << "Problem making a Seglr. Cannot retrieve turn time." << endl;
  if(!ok4)
    cout << "Problem making a Seglr. Cannot retrieve rotation." << endl;

  if(!ok1 || !ok1 || !ok1 || !ok1) 
    return(seglr);


  // Make a TurnPoly with the coefficients
  TurnPoly poly;
  poly.setDeltaHeading(dh);
  poly.setSpeed(spd);
  poly.setPolyCoefficients(coeffs);
  poly.setRotationAngle(rota);
  poly.setXEnd(xend);
  poly.setTCutoff(time);

  poly.getSimplifiedNorthFacingTurn(seglr);
  return(seglr);
}

//----------------------------------------------------------------
// Procedure: getSeglr

Seglr AVK_Table::getSeglr(unsigned int row, unsigned int col)
{
  Seglr null_seglr;
  if(row >= m_grid_seglrs.getRows())
    return(null_seglr);
  if(col >= m_grid_seglrs.getCols())
    return(null_seglr);
  
  Seglr seglr;
  bool ok = m_grid_seglrs.getCellVal(row, col, seglr);
  if(!ok)
    return(null_seglr);

  return(seglr);
}

//----------------------------------------------------------------
// Procedure: print()

void AVK_Table::print() const
{
  m_grid_coeffs.print();
  m_grid_xend.print();
  m_grid_time.print();
  m_grid_rotation.print();
}

void AVK_Table::getGridDHV(double spd, double dh, 
                            double& gridspd, double& griddh) const
{
  unsigned int spdix, dhix;
  m_grid_seglrs.getSpdIX(spd,spdix);
  m_grid_seglrs.getHdgIX(dh,dhix);

  m_grid_seglrs.getSpdVal(spdix,gridspd);
  m_grid_seglrs.getHdgVal(dhix,griddh);
}
