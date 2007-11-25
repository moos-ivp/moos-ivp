/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: MY_Output.h                                          */
/*    DATE: December 2004                                        */
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

#ifndef MY_OUTPUT_HEADER
#define MY_OUTPUT_HEADER

#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Output.H>


class MY_Output : public Fl_Output {
public:
  MY_Output(int x, int y, int w, int h, const char *l=0) :
    Fl_Output(x, y, w, h, l) {};

  int  handle(int event) {return(0);};
};
#endif








