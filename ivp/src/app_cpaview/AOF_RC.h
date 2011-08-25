/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOF_RC.h                                             */
/*    DATE: August 5th, 2007                                     */
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
 
#ifndef AOF_RC_RC_HEADER
#define AOF_RC_RC_HEADER

#include "AOF.h"

class IvPDomain;
class AOF_RC: public AOF {
public:
  AOF_RC(IvPDomain);
  ~AOF_RC() {};


public: // virtuals defined
  double evalBox(const IvPBox*) const;   
  bool   setParam(const std::string&, double);
  bool   initialize();

protected:
  double metric(double) const;

protected:
  double m_osx;   // Ownship X position
  double m_osy;   // Ownship Y position
  double m_cnx;   // Contact X position
  double m_cny;   // Contact Y position
  double m_cnh;   // Contact heading
  double m_cnv;   // Contact speed

  bool   m_osx_set;
  bool   m_osy_set;
  bool   m_cnx_set;
  bool   m_cny_set;
  bool   m_cnh_set;
  bool   m_cnv_set;

  int    m_crs_ix;  // Index of "course" variable in IvPDomain
  int    m_spd_ix;  // Index of "speed" variable in IvPDomain
};

#endif








