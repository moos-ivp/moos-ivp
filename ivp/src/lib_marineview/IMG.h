/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: IMG.h                                                */
/*    DATE: Dec 13, 2003                                         */
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

#include <string>

#ifndef IMG_HEADER
#define IMG_HEADER

class IMG {
public:
  IMG(int g_ymax);
  IMG(const IMG&);
  ~IMG();

  const IMG &operator=(const IMG&);

  double get_ival_ix(int g_crs, int g_y) const;
  void   set_ival_ix(int g_crs, int g_y, double g_val);

  double get_hval() const  {return(hval);};
  double get_lval() const  {return(lval);};
  int    get_ymax() const  {return(ymax);};

  bool   isValid();

  void   set_vname(std::string str) {vname = str;};
  void   set_bname(std::string str) {bname = str;};
  std::string get_vname() const     {return(vname);};
  std::string get_bname() const     {return(bname);};

protected:
  double**  ival;    // The data representing OF information
  int       ymax;    // dimension 1 extent of the ival array

  double    hval;    // Highest value in the ival array
  double    lval;    // Lowest value in the ival array
  bool      lfresh;  // True when no high/lov vals applied
  bool      hfresh;  // True when no high/lov vals applied

  std::string vname; // vehicle name
  std::string bname; // behavior name
};

#endif








