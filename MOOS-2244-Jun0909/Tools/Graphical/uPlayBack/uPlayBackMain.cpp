///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman at MIT 2001-2002 and Oxford 
//   University 2003-2005. email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Utility Component. 
//        
//   This program is free software; you can redistribute it and/or 
//   modify it under the terms of the GNU General Public License as 
//   published by the Free Software Foundation; either version 2 of the 
//   License, or (at your option) any later version. 
//          
//   This program is distributed in the hope that it will be useful, 
//   but WITHOUT ANY WARRANTY; without even the implied warranty of 
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
//   General Public License for more details. 
//            
//   You should have received a copy of the GNU General Public License 
//   along with this program; if not, write to the Free Software 
//   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 
//   02111-1307, USA. 
//
//////////////////////////    END_GPL    //////////////////////////////////
#if (_MSC_VER == 1200)
#pragma warning(disable: 4786)
#pragma warning(disable: 4503)
#endif

#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Tabs.H>
#include <FLTKVW/Flv_Table.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include "PlaybackWindow.h"


int main(int argc, char* argv[])
{
    Fl_Double_Window *w = new Fl_Double_Window( 340, 160, "uPlayBack" );
    Fl_Group *pG = new CPlaybackWindow(0,0,340,160,"");
    w->end();


    w->resizable(pG);
    w->show(argc, argv);
    return Fl::run();
}
