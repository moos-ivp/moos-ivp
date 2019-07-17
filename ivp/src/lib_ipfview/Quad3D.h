/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Quad3D.h                                             */
/*    DATE: Jan 12th 2006                                        */
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

#ifndef QUAD3D_HEADER
#define QUAD3D_HEADER

class Quad3D {
public:
  Quad3D();
  ~Quad3D() {}

  double getAvgVal() {return((ll_hgt + hl_hgt + hh_hgt + lh_hgt)/4);}

  // Setters ------------------------------------------------
  void setLLX(double v)   {ll_xval = v;}
  void setHLX(double v)   {hl_xval = v;}
  void setHHX(double v)   {hh_xval = v;}
  void setLHX(double v)   {lh_xval = v;}

  void setLLY(double v)   {ll_yval = v;}
  void setHLY(double v)   {hl_yval = v;}
  void setHHY(double v)   {hh_yval = v;}
  void setLHY(double v)   {lh_yval = v;}
  
  void setLLZ(double v)   {ll_hgt = v;} // LLH = Low,Low's Height
  void setHLZ(double v)   {hl_hgt = v;}
  void setHHZ(double v)   {hh_hgt = v;}
  void setLHZ(double v)   {lh_hgt = v;}
  
  void setLLR(double v)   {ll_red = v;}
  void setHLR(double v)   {hl_red = v;}
  void setHHR(double v)   {hh_red = v;}
  void setLHR(double v)   {lh_red = v;}
  
  void setLLG(double v)   {ll_grn = v;}
  void setHLG(double v)   {hl_grn = v;}
  void setHHG(double v)   {hh_grn = v;}
  void setLHG(double v)   {lh_grn = v;}
  
  void setLLB(double v)   {ll_blu = v;}
  void setHLB(double v)   {hl_blu = v;}
  void setHHB(double v)   {hh_blu = v;}
  void setLHB(double v)   {lh_blu = v;}

  // Getters ------------------------------------------------
  double getLLX() const   {return(ll_xval);} // LLX = Low,Low's X val
  double getHLX() const   {return(hl_xval);}
  double getHHX() const   {return(hh_xval);}
  double getLHX() const   {return(lh_xval);}

  double getLLY() const   {return(ll_yval);} // LLX = Low,Low's Y val
  double getHLY() const   {return(hl_yval);}
  double getHHY() const   {return(hh_yval);}
  double getLHY() const   {return(lh_yval);}

  double getLLZ() const   {return(ll_hgt);} // LLZ = Low,Low's Height
  double getHLZ() const   {return(hl_hgt);}
  double getHHZ() const   {return(hh_hgt);}
  double getLHZ() const   {return(lh_hgt);}

  double getLLR() const   {return(ll_red);}
  double getHLR() const   {return(hl_red);}
  double getHHR() const   {return(hh_red);}
  double getLHR() const   {return(lh_red);}
  
  double getLLG() const   {return(ll_grn);}
  double getHLG() const   {return(hl_grn);}
  double getHHG() const   {return(hh_grn);}
  double getLHG() const   {return(lh_grn);}
  
  double getLLB() const   {return(ll_blu);}
  double getHLB() const   {return(hl_blu);}
  double getHHB() const   {return(hh_blu);}
  double getLHB() const   {return(lh_blu);}

  double getXinLOW(unsigned int) const;
  double getYinLOW(unsigned int) const;
  double getZinLOW(unsigned int) const;
  double getRinLOW(unsigned int) const;
  double getGinLOW(unsigned int) const;
  double getBinLOW(unsigned int) const;

  double getXinHGH(unsigned int) const;
  double getYinHGH(unsigned int) const;
  double getZinHGH(unsigned int) const;
  double getRinHGH(unsigned int) const;
  double getGinHGH(unsigned int) const;
  double getBinHGH(unsigned int) const;
  unsigned int getInPtsSize() const {return(m_xin_low.size());}
  
  // Modifiers -------------------------------------------
    
  void addLLZ(double v) {ll_hgt += v;}  // LLZ = Low,Low's Height
  void addHLZ(double v) {hl_hgt += v;}
  void addHHZ(double v) {hh_hgt += v;}
  void addLHZ(double v) {lh_hgt += v;}

  void expand(double v);
  
  void applyPolar(double radextra, int dim, int pts=0);
  void applyColorIntensity(double intensity);
  void applyScale(double scale);
  void applyBase(double value);
  void applyTranslation(double x, double y);

  void interpolate(double xdelta);
  
 protected:

  std::vector<double> m_xin_low;
  std::vector<double> m_yin_low;
  std::vector<double> m_zin_low;
  std::vector<double> m_rin_low;
  std::vector<double> m_gin_low;
  std::vector<double> m_bin_low;

  std::vector<double> m_xin_hgh;
  std::vector<double> m_yin_hgh;
  std::vector<double> m_zin_hgh;
  std::vector<double> m_rin_hgh;
  std::vector<double> m_gin_hgh;
  std::vector<double> m_bin_hgh;

 protected:
  double ll_xval;
  double hl_xval;
  double hh_xval;
  double lh_xval;

  double ll_yval;
  double hl_yval;
  double hh_yval;
  double lh_yval;  
  
  double ll_hgt;
  double hl_hgt;
  double hh_hgt;
  double lh_hgt;

  double ll_red;
  double hl_red;
  double hh_red;
  double lh_red;

  double ll_grn;
  double hl_grn;
  double hh_grn;
  double lh_grn;

  double ll_blu;
  double hl_blu;
  double hh_blu;
  double lh_blu;
};
#endif








