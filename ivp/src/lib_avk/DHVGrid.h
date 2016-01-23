/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: DHVGrid.h                                           */
/*    DATE: Mar 25th, 2015                                       */
/*****************************************************************/

#ifndef DHV_GRID_HEADER
#define DHV_GRID_HEADER

#include <iostream>
#include <iomanip>
#include <vector>
#include "MBUtils.h"

template <class CellType> class DHVGrid
{
 public:
  DHVGrid();
  ~DHVGrid() {};
  
 public: // Setters
  bool configure(double dh, double dv, double mv);
  bool reconfigure(double dh, double dv, double mv);
  void setAll(CellType value);
  bool setCell(double spd, double dh, CellType value);
  bool setAccessTolerance(double cell_pct);

 public: // Getters

  double  getHdgGap() const {return(m_hdg_gap);};
  double  getSpdGap() const {return(m_spd_gap);};
  double  getSpdMax() const {return(m_spd_max);};
  unsigned int getRows() const {return(m_rows);};
  unsigned int getCols() const {return(m_cols);};
  unsigned int cellsSet() const;

  bool getCellVal(double spd, double dh, CellType& cell) const;
  bool getCellVal(unsigned int spd_ix, unsigned int dh_ix, CellType& cell) const;
  bool cellSet(double spd, double dh) const;
  bool cellSet(unsigned int spd_ix, unsigned int dh_ix) const;
  
 public: // Analyzers
  bool  configured() const {return(m_configured);};
  void  print() const;

 public: // Utilities
  bool  getSpdIX(double, unsigned int&) const;
  bool  getHdgIX(double, unsigned int&) const;
  bool  getSpdVal(unsigned int, double&) const;
  bool  getHdgVal(unsigned int, double&) const;

  std::vector<CellType>& operator[] (const int ix);

 protected: // Config variables
  double  m_hdg_gap;
  double  m_spd_gap;
  double  m_spd_max;

 protected: // State variables  
  std::vector<std::vector<CellType> > m_grid;
  std::vector<std::vector<bool> >     m_cset;
  std::vector<std::vector<double> >   m_spd;
  std::vector<std::vector<double> >   m_dh;

  unsigned int m_rows;
  unsigned int m_cols;

  double m_tolerance;

  bool  m_configured;
};

template <class CellType>
std::vector<CellType>& DHVGrid<CellType>::operator[] (const int ix)
{
  return(m_grid[ix]);
}


//----------------------------------------------------------------
// Constructor

template <class CellType> 
DHVGrid<CellType>::DHVGrid()
{
  m_configured   = false;

  // Initialize State Variables
  m_rows  = 0;
  m_cols  = 0;

  m_tolerance = 1.0;   // Acceptable range (0,1]

}

//----------------------------------------------------------------
// Proecedure: setAccessTolerance()

template <class CellType> 
bool DHVGrid<CellType>::setAccessTolerance(double tolerance)
{
  if(tolerance <= 0)
    return(false);

  if(tolerance > 1.0)
    m_tolerance = 1.0;   // Acceptable range (0,1.0]
  else
    m_tolerance = tolerance;
  return(true);
}


//----------------------------------------------------------------
// Procedure: configure()
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

template <class CellType> 
bool DHVGrid<CellType>::configure(double hdg_gap, double spd_gap, double spd_max)
{
  // Some Sanity checks (probably could be stricter)
  if((hdg_gap < 0) || (hdg_gap > 90))
    return(false);
  if((spd_gap < 0) || (spd_gap >= spd_max) || (spd_max < 0))
    return(false);


  // Part 1A: Determine the number of speed rows
  unsigned int rows = (unsigned int) snapToStep((spd_max / spd_gap), .01) + 1;
  // Part 1B: Sanity check the speed init vars
  double v_product  = (double)(rows-1) * spd_gap;
  double v_mismatch = v_product - spd_max;
  if(v_mismatch < 0)
    v_mismatch *= -1;
  if(v_mismatch > (0.001 * spd_gap)) {
    std::cout << "spd_mismatch: " << v_mismatch << std::endl;
    return(false);
  }  

  // Part 2A: Determine the number of heading cols
  unsigned int cols = (unsigned int)(180.0 / hdg_gap) + 1;

  // Part 2B: Sanity check the heading init vars
  double h_product = (double)(cols-1) * hdg_gap;
  double h_mismatch = h_product - 180;
  if(h_mismatch < 0)
    h_mismatch *= -1;
  if(h_mismatch > (0.001 * hdg_gap)) {
    std::cout << "h_mismatch: " << h_mismatch << std::endl;
    return(false);
  }  

  // Part 3: Build the data grid
  CellType                            new_cell;
  std::vector<CellType>               new_row(cols, new_cell);
  std::vector<std::vector<CellType> > new_grid(rows, new_row);
  m_grid = new_grid;

  // Part 4: Build the cell_set grid
  std::vector<bool>                 new_brow(cols, false);
  std::vector<std::vector<bool> >   new_bgrid(rows, new_brow);
  m_cset = new_bgrid;

  // Part 5: Buidl the dh/spd grids and populate them.
  std::vector<double>               new_drow(cols, 0);
  std::vector<std::vector<double> > new_dgrid(rows, new_drow);
  m_spd = new_dgrid;
  m_dh  = new_dgrid;

  for(unsigned int i=0; i<rows; i++) {
    double rval = i * spd_gap;
    rval = snapToStep(rval, 0.1);
    for(unsigned int j=0; j<cols; j++) {
      double cval = (double)(j) * hdg_gap;
      cval = snapToStep(cval, 0.01);
      m_spd[i][j] = rval;
      m_dh[i][j] = cval;
    }
  }

  // Part 6: set the configuration params.
  m_hdg_gap = hdg_gap;
  m_spd_gap = spd_gap;
  m_spd_max = spd_max;
  m_rows    = rows;
  m_cols    = cols;
  m_configured = true;

  return(true);
}
  
//----------------------------------------------------------------
// Procedure: reconfigure()

template <class CellType> 
bool DHVGrid<CellType>::reconfigure(double hdg_gap, double spd_gap, double spd_max)
{
  if(!m_configured)
    return(configure(hdg_gap, spd_gap, spd_max));

  // Part 1: Store a copy of current information
  unsigned int c_rows = m_rows;
  unsigned int c_cols = m_cols;
  std::vector<std::vector<CellType> > c_grid = m_grid;
  std::vector<std::vector<bool> >     c_cset = m_cset;
  std::vector<std::vector<double> >   c_spd  = m_spd;
  std::vector<std::vector<double> >   c_dh   = m_dh;

  // Part2: Reconfigure the grid with the new values
  bool config_ok = configure(hdg_gap, spd_gap, spd_max);
  if(!config_ok)
    return(false);

  // Part 3: copy the older information into the new grid
  for(unsigned int i=0; i<c_rows; i++) {
    for(unsigned int j=0; j<c_cols; j++) {
      if(c_cset[i][j]) {
	double spd   = c_spd[i][j];
	double dh    = c_dh[i][j];
	CellType val = c_grid[i][j];
	setCell(spd, dh, val);
      }
    }
  }

  return(true);
}
  
//----------------------------------------------------------------
// Procedure: setAll

template <class CellType> 
void DHVGrid<CellType>::setAll(CellType value)
{
  for(unsigned int i=0; i<m_rows; i++) {
    for(unsigned int j=0; j<m_cols; j++) {
      m_grid[i][j] = value;
      m_cset[i][j] = true;
    }
  }
}

//----------------------------------------------------------------
// Procedure: getSpdIX()
//   Purpose: get the speed INDEX based on a given speed
//   Returns: true if given speed is in the range of the table      

template <class CellType> 
bool DHVGrid<CellType>::getSpdIX(double spd, unsigned int& index) const
{
  // Sanity Check 1 (speed must be in the range [min spd,max spd])
  if((spd < 0) || (spd > m_spd_max)) { 
    std::cout << "getSpdIX spd out of range, spd: " << spd;
    std::cout << ", m_spd_max: " << m_spd_max << std::endl;
    return(false);
  }
    
  //double d_ix_maybe = (spd / m_v_gap);
  double d_ix_maybe = snapToStep((spd / m_spd_gap), 0.01);
  unsigned int ix_maybe = (unsigned int)(d_ix_maybe+0.5);

  // Sanity Check 2 (index must be within bounds w.r.t. number of rows)
  if(ix_maybe >= m_rows) {
    std::cout << "row index out of bounds" << std::endl;
    return(false);
  }
  
  // Sanity check 3 (given spd must reasonably correspond to a table spd)
  double spd_in_table = m_spd[ix_maybe][0];
  double spd_mismatch = spd - spd_in_table;
  if(spd_mismatch < 0)
    spd_mismatch *= -1;

  double max_mismatch = m_tolerance * m_spd_gap;
  if(spd_mismatch > max_mismatch) {
    std::cout << "speed mismatch greater than tolerance" << std::endl;
    return(false);
  }
  
  index = ix_maybe;
  return(true);
  
}

//----------------------------------------------------------------
// Procedure: getHdgIX()
      
template <class CellType> 
bool DHVGrid<CellType>::getHdgIX(double hdg, unsigned int& index) const
{
  // Sanity Check 1: Make sure the table has been initialized
  if((m_rows == 0) || (m_cols == 0))
    return(false);

  // Sanity Check 2: Given hdg must be in range [m_h_min, m_h_max]
  if(hdg < 0)
    hdg = 0;
  if((hdg < 0) || (hdg > 180))
    return(false);

  unsigned int ix_maybe = (unsigned int)((hdg / m_hdg_gap) + 0.5);
  // Sanity Check 3: Index must be within bounds w.r.t. number of columns
  if(ix_maybe >= m_cols)
    return(false);
  
  // Sanity check 4: Given hdg must reasonably correspond to a table hdg
  double hdg_in_table = m_dh[0][ix_maybe];
  double hdg_mismatch = hdg - hdg_in_table;
  if(hdg_mismatch < 0)
    hdg_mismatch *= -1;

  double max_mismatch = m_tolerance * m_hdg_gap;
  if(hdg_mismatch > max_mismatch) 
    return(false);
  
  index = ix_maybe;
  return(true);  
}


//----------------------------------------------------------------
// Procedure: getSpdVal()
      
template <class CellType> 
bool DHVGrid<CellType>::getSpdVal(unsigned int index, double& val) const
{
  // Sanity Check 1: Make sure the table has been initialized
  if((m_rows == 0) || (m_cols == 0))
    return(false);

  // Sanity Check 2: Given index must be in range
  if(index >= m_rows)
    return(false);

  val = m_spd[index][0];
  return(true);  
}


//----------------------------------------------------------------
// Procedure: getHdgVal()
      
template <class CellType> 
bool DHVGrid<CellType>::getHdgVal(unsigned int index, double& val) const
{
  // Sanity Check 1: Make sure the table has been initialized
  if((m_rows == 0) || (m_cols == 0))
    return(false);

  // Sanity Check 2: Given index must be in range
  if(index >= m_cols)
    return(false);

  val = m_dh[0][index];
  return(true);  
}


//----------------------------------------------------------------
// Procedure: setCell

template <class CellType> 
bool DHVGrid<CellType>::setCell(double spd, double dh, CellType cell_val)
{
  unsigned int spd_ix, hdg_ix;
  
  bool ok_spd_ix = getSpdIX(spd, spd_ix);
  bool ok_hdg_ix = getHdgIX(dh, hdg_ix);
  
  //std::cout << "ok_spd_ix:" << ok_spd_ix << std::endl;
  //std::cout << "ok_hdg_ix:" << ok_hdg_ix << std::endl;

  if(!ok_spd_ix || !ok_hdg_ix)
    return(false);
  
  m_grid[spd_ix][hdg_ix] = cell_val;
  m_cset[spd_ix][hdg_ix] = true;
  
  return(true);
}

//----------------------------------------------------------------
// Procedure: cellsSet

template <class CellType> 
unsigned int DHVGrid<CellType>::cellsSet() const
{
  unsigned int count = 0;
  for(unsigned int i=0; i<m_rows; i++) {
    for(unsigned int j=0; j<m_cols; j++) {
      if(cellSet(i, j)) {
	count++;
      }
    }
  }
  return(count);
}

//----------------------------------------------------------------
// Procedure: getCellVal

template <class CellType> 
bool DHVGrid<CellType>::getCellVal(double spd, double dh, CellType& cell) const
{
  unsigned int spd_ix, hdg_ix;
  
  bool ok_spd_ix = getSpdIX(spd, spd_ix);
  bool ok_hdg_ix = getHdgIX(dh, hdg_ix);
  
  //std::cout << "ok_spd_ix:" << ok_spd_ix << std::endl;
  //std::cout << "ok_hdg_ix:" << ok_hdg_ix << std::endl;

  if(!ok_spd_ix || !ok_hdg_ix)
    return(false);
  
  cell = m_grid[spd_ix][hdg_ix];
  return(true);
}

//----------------------------------------------------------------
// Procedure: getCellVal

template <class CellType> 
bool DHVGrid<CellType>::getCellVal(unsigned int spd_ix, unsigned int dh_ix, 
				   CellType& cell) const
{
  if((spd_ix >= m_rows) || (dh_ix >= m_cols))
    return(false);
  
  cell = m_grid[spd_ix][dh_ix];
  return(true);
}

//----------------------------------------------------------------
// Procedure: cellSet

template <class CellType> 
bool DHVGrid<CellType>::cellSet(double spd, double dh) const
{
  unsigned int spd_ix, hdg_ix;
  
  bool ok_spd_ix = getSpdIX(spd, spd_ix);
  bool ok_hdg_ix = getHdgIX(dh, hdg_ix);
  
  if(!ok_spd_ix || !ok_hdg_ix)
    return(false);
  
  return(m_cset[spd_ix][hdg_ix]);
}

//----------------------------------------------------------------
// Procedure: cellSet

template <class CellType> 
bool DHVGrid<CellType>::cellSet(unsigned int spd_ix, unsigned int dh_ix) const
{
  if((spd_ix >= m_rows) || (dh_ix >= m_cols))
    return(false);
  
  return(m_cset[spd_ix][dh_ix]);
}

//----------------------------------------------------------------
// Procedure: print

template <class CellType> 
void DHVGrid<CellType>::print() const
{
  std::string table_hdr = "     ";
  for(unsigned int j=0; j<m_cols; j++) {
    std::string hdr = "[" + uintToString(j) + "]";
    hdr = padString(hdr, 10, true);
    table_hdr += hdr;
  }

#if 0
  // Only valid if the CellType can be sent to cout
  std::cout << "GRID:" << std::endl;
  std::cout << table_hdr << std::endl;
  for(unsigned int i=0; i<m_rows; i++) {
    std::string row = "[" + uintToString(i) + "]";
    row = padString(row, 5, false);
    std::cout << row;
    for(unsigned int j=0; j<m_cols; j++) {
      std::string cell = doubleToString(m_grid[i][j],1);
      cell = padString(cell, 10, true);
      std::cout << cell;
    }
    std::cout << std::endl;
  }
#endif

  std::cout << "DH/SPD:" << std::endl;
  std::cout << table_hdr << std::endl;
  for(unsigned int i=0; i<m_rows; i++) {
    std::string row = "[" + uintToString(i) + "]";
    row = padString(row, 5, false);
    std::cout << row;
    for(unsigned int j=0; j<m_cols; j++) {
      std::string cell = doubleToString(m_spd[i][j],1) + ":" + doubleToString(m_dh[i][j],0);
      cell = padString(cell, 10, true);
      std::cout << cell;
    }
    std::cout << std::endl;
  }

  std::cout << "CellSet:" << std::endl;
  std::cout << table_hdr << std::endl;
  for(unsigned int i=0; i<m_rows; i++) {
    std::string row = "[" + uintToString(i) + "]";
    row = padString(row, 5, false);
    std::cout << row;
    for(unsigned int j=0; j<m_cols; j++) {
      std::string cell = "-";
      if(m_cset[i][j]) {
	cell = doubleToString(m_spd[i][j],1) + ":";
	cell += doubleToString(m_dh[i][j],0);
      }
      cell = padString(cell, 10, true);
      std::cout << cell;

      //cell = "SET";
      //cell = padString(cell, 10, true);
      //std::cout << cell;
    }
    std::cout << std::endl;
  }
}

#endif 
