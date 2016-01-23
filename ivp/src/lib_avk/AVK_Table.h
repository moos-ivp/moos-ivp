/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AVK_Table.h                                          */
/*    DATE: Mar 25th, 2015                                       */
/*****************************************************************/

#ifndef AVK_TABLE_HEADER
#define AVK_TABLE_HEADER

#include <vector>
#include "DHVGrid.h"
#include "Seglr.h"

class AVK_Table 
{
 public:
  AVK_Table();
  ~AVK_Table() {};
  
 public: // Setters
  bool   configure(double dh, double dv, double mv);
  bool   reconfigure(double dh, double dv, double mv);
  bool   setCellCoeffs(double spd, double dh, std::vector<double> coeffs);
  bool   setCellXend(double spd, double dh, double xend);
  bool   setCellTime(double spd, double dh, double time);
  bool   setCellRotation(double spd, double dh, double rotation);
  bool   setCellInterp(double spd, double dh, bool interp);

 public: // Getters
  Seglr  getSeglr(double spd, double dh);
  Seglr  getSeglr(unsigned int row, unsigned int col);

  double getHdgGap() const {return(m_grid_coeffs.getHdgGap());};
  double getSpdGap() const {return(m_grid_coeffs.getSpdGap());};
  double getSpdMax() const {return(m_grid_coeffs.getSpdMax());};
  void   getGridDHV(double spd, double dh, 
                    double& gridspd, double& griddh) const;

  unsigned int getRows() const {return(m_grid_coeffs.getRows());};
  unsigned int getCols() const {return(m_grid_coeffs.getCols());};

  std::vector<Seglr>  getAllSeglrs() const;
  std::vector<double> getAllDH() const;
  std::vector<double> getAllSpd() const;

  std::vector<Seglr> getSeglrsSpd(double) const;
  std::vector<Seglr> getSeglrsDH() const;

 public: // Analyzers
  bool  configured() const   {return(m_configured);};
  bool  interpolated() const {return(m_interpolated);};
  void  print() const;

 public: // Modifiers
  bool  interpolateAll();
  bool  pcwiseBuildAll();

 protected: // Utilities
  bool cellInterpolate(unsigned int spd_ix, unsigned int dh_ix);
  bool findInterpNeigh(unsigned int spd_ix, unsigned int dh_ix, 
		       unsigned int& spd1_ix, unsigned int& dh1_ix,
		       unsigned int& spd2_ix, unsigned int& dh2_ix,
		       double& pct);

 protected: // State variables
  
  DHVGrid<std::vector<double> > m_grid_coeffs;
  DHVGrid<double>               m_grid_xend;
  DHVGrid<double>               m_grid_time;
  DHVGrid<double>               m_grid_rotation;
  DHVGrid<Seglr>                m_grid_seglrs;
  DHVGrid<bool>                 m_grid_interp;

  bool  m_configured;
  bool  m_interpolated;
};

#endif 
