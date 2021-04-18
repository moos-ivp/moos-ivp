/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: MY_Menu_Button.h                                     */
/*    DATE: July 18th, 2009 (on the plane back from GLINT09)     */
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

#ifndef MY_FLTK_MENU_BUTTON_HEADER
#define MY_FLTK_MENU_BUTTON_HEADER

#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Menu_Button.H>


class MY_Menu_Button : public Fl_Menu_Button {
public:
  MY_Menu_Button(int x, int y, int w, int h, const char *l=0) :
  Fl_Menu_Button(x, y, w, h, l) {}
  
  int  handle(int event) {
    if((Fl::event_key()==FL_Up)   || (Fl::event_key()==FL_Down)  || 
       (Fl::event_key()==FL_Left) || (Fl::event_key()==FL_Right)) {
      return(0);
    }
    return(Fl_Menu_Button::handle(event));
  }
};

#endif

















