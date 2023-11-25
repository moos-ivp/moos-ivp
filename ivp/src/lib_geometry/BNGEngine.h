/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BNGEngine.h                                          */
/*    DATE: Feb 29th 2016                                        */
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
 
#ifndef BNG_ENGINE_HEADER
#define BNG_ENGINE_HEADER

class BNGEngine {
public:
  BNGEngine();
  BNGEngine(double cnx, double cny, double cnh, double cnv, 
	    double osx, double osy);
  ~BNGEngine() {}

public:    
  double evalBNG(double osh, double osv) const;
  double evalBNGRate(double osh, double osv) const;

 protected:
  void   setStatic();

 protected: // Config parameters
  double m_cnx;   // Contact Lat position at time Tm.
  double m_cny;   // Contact Lon position at time Tm.
  double m_cnv;   // Contact Speed in kts.
  double m_cnh;   // Contact Course in degrees (0-359).
  double m_osx;   // Ownship Lat position at time Tm.
  double m_osy;   // Ownship Lon position at time Tm.

};

#endif




















