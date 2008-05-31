/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: FColorMap.h                                          */
/*    DATE: Jan 13th 2006                                        */
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

#ifndef FCOLOR_MAP_HEADER
#define FCOLOR_MAP_HEADER

#include <vector>
#include <string>

class FColorMap
{
public:
  FColorMap() {setStandardMap();};
  ~FColorMap() {};

  void setType(const std::string&);

  double getRVal(unsigned int i) const
    {return((i>=0)&&(i<rval.size())?rval[i]:0);};

  double getGVal(unsigned int i) const
    {return((i>=0)&&(i<gval.size())?gval[i]:0);};

  double getBVal(unsigned int i) const
    {return((i>=0)&&(i<bval.size())?bval[i]:0);};

  double getIRVal(double) const;
  double getIGVal(double) const;
  double getIBVal(double) const;
  int    size() {return(rval.size());};
    
  void   addRGB(double r, double g, double b) 
    {rval.push_back(r); gval.push_back(g); bval.push_back(b);};

  void   setStandardMap();
  void   setCopperMap();
  void   setBoneMap();

  void   applyMidWhite(double, double);

protected:
  std::vector<double> rval;
  std::vector<double> gval;
  std::vector<double> bval;
};
#endif


