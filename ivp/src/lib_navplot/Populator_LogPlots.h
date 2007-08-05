/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: Populator_LogPlots.h                                 */
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

#ifndef POPULATOR_LOGPLOTS_HEADER
#define POPULATOR_LOGPLOTS_HEADER

#include <string>
#include <vector>
#include "LogPlot.h"

class Populator_LogPlots 
{
public:
  Populator_LogPlots();
  ~Populator_LogPlots() {};

  bool    set_file(std::string);
  void    set_vname(std::string s) {vname = s;};
  void    set_skew(double amt)     {skew = amt;};

  bool    populate();
  LogPlot get_logplot(int);
  LogPlot get_logplot(std::string);
  int     get_size() {return(logplots.size());};

  void    print();

protected:
  void scan_columns();
  void populate_logplot(int);

protected:
  std::string file;
  std::string vname;
  std::vector<LogPlot> logplots;
  std::vector<std::string> columns;
  std::vector<std::string> lines;
  std::vector<std::vector<std::string> > plines; // parsed lines

  double skew;
};
#endif 







