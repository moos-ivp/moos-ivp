/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_NavPlot.h                                  */
/*    DATE: June 5th, 2005 (Sun in Kenmorre)                     */
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

#ifndef POPULATOR_NAVPLOT_HEADER
#define POPULATOR_NAVPLOT_HEADER

#include <string>
#include <vector>
#include "NavPlot.h"

class Populator_NavPlot 
{
public:
  Populator_NavPlot();
  ~Populator_NavPlot() {};

  bool    set_file(std::string);
  bool    populate();
  NavPlot get_navplot() {return(navplot);};
  void    set_skew(double amt)      {skew = amt;};

 protected:
  void scan_columns();
  void populate_navplot(int, int, int ix=-1);

protected:
  NavPlot navplot;
  std::string file;
  std::vector<std::string> columns;
  std::vector<std::string> lines;
  double skew;

  std::vector<std::vector<std::string> > plines; // parsed lines

};
#endif 








