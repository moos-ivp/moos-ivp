/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: AVD_Regressor.h                                      */
/*    DATE: Feb 15th, 2009                                       */
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

#ifndef AVD_REGRESSOR_HEADER
#define AVD_REGRESSOR_HEADER

#include <vector>

class AVD_Regressor {
public:
  AVD_Regressor();
  ~AVD_Regressor() {};

  bool addMeasurement(double time, double hdelta);
  bool addMeasurementSet(std::string pairs);
  bool addMeasurementSet(std::vector<double>, std::vector<double>);

  double fit();

  void   clear();

  double getCoeff();
  double getScalar();

 protected:
  std::vector<double> m_times;
  std::vector<double> m_hdeltas;

  double m_coeff;
  double m_scalar;
};
#endif
