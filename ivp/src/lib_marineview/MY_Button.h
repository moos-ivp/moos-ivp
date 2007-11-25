/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MY_Button.h                                          */
/*    DATE: September 23rd 2007                                  */
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

#ifndef MY_FLTK_BUTTON_HEADER
#define MY_FLTK_BUTTON_HEADER

#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Button.H>


class MY_Button : public Fl_Button {
public:
  MY_Button(int x, int y, int w, int h, const char *l=0) :
    Fl_Button(x, y, w, h, l) {};

  int  handle(int event) {
    if((Fl::event_key()==FL_Up)   || 
       (Fl::event_key()==FL_Down) || 
       (Fl::event_key()==FL_Left) || 
       (Fl::event_key()==FL_Right)) {
      return(0);
    }
    return(Fl_Button::handle(event));
  }
};
#endif








