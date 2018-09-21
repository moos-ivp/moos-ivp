/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MY_Fl_Hold_Browser.h                                 */
/*    DATE: Sep 10th, 2012                                       */
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

#ifndef MY_FLTK_HOLD_BROWSER_HEADER
#define MY_FLTK_HOLD_BROWSER_HEADER

#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Hold_Browser.H>

class MY_Fl_Hold_Browser : public Fl_Hold_Browser {
public:
 MY_Fl_Hold_Browser(int x, int y, int w, int h, const char *l=0) :
  Fl_Hold_Browser(x, y, w, h, l) 
    {
      textfont(FL_COURIER);
      box(FL_DOWN_BOX);
      textcolor(FL_BLACK);
    };
  
  int handle(int event) {
    if((event == 1) || (event == 2))
      return(Fl_Hold_Browser::handle(event));
    return(0);
  };
};
#endif

















