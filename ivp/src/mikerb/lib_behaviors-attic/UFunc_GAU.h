/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: UFunc_GAU.H                                          */
/*    DATE: June 14th, 2003                                      */
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

#ifndef UFUNC_GAU_HEADER
#define UFUNC_GAU_HEADER

#include "UFunc.h"

class UFunc_GAU: public UFunc {
public:
  UFunc_GAU(const char*  g_label,
	    double g_mean,
	    double g_stdDev);
  UFunc_GAU();
  ~UFunc_GAU() {};
  UFunc_GAU &operator=(const UFunc_GAU &rhs) {
    setLabel(rhs.label);
    mean = rhs.mean;
    stdDev = rhs.stdDev;
  }

public:
  double  eval(double) const; // virtual defined

  double  getMean() const   {return(mean);};
  double  getStdDev() const {return(stdDev);};

  void    setMean(double v)   {mean = v;};
  void    setStdDev(double v) {stdDev = v;};

protected:
  double mean;
  double stdDev;

private:
  double cachedVal;
};

#endif






