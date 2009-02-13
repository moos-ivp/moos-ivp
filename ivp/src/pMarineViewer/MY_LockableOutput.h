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

#ifndef MY_LOCKABLE_OUTPUT_HEADER
#define MY_LOCKABLE_OUTPUT_HEADER

#include <iostream>
#include <FL/Fl.H>
#include <FL/Fl_Output.H>


class MY_LockableOutput : public Fl_Output {
public:
  MY_LockableOutput(int x, int y, int w, int h, const char *l=0) :
  Fl_Output(x, y, w, h, l) {m_mutex=0;};

  int  handle(int event) {return(0);};

  void setMutex(CMOOSLock *mutex) {m_mutex = mutex;};

  void draw() {
    if(m_mutex)
      m_mutex->Lock();
    Fl_Output::draw();
    if(m_mutex)
      m_mutex->UnLock();
  }

  CMOOSLock *m_mutex;

};
#endif








