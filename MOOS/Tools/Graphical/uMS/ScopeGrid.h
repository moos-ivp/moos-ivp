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

// ScopeGrid.h: interface for the CScopeGrid class.


#if !defined(AFX_SCOPEGRID_H__E91D0ABA_84EC_4E7A_8AA8_FCC45E216208__INCLUDED_)
#define AFX_SCOPEGRID_H__E91D0ABA_84EC_4E7A_8AA8_FCC45E216208__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <cstring>
#include <FL/fl_draw.H>
#include <FLTKVW/Flv_Table.H>
#include <FL/Fl_Menu_Window.H>
#include "DBImage.h"
#include <MOOSLIB/MOOSCommClient.h>





// FLOATING TIP WINDOW


class CTipWindow : public Fl_Menu_Window {

//    std::string m_sText;
    char m_sText[4096];
public:
    CTipWindow():Fl_Menu_Window(1,1) {    // will autosize
        set_override();
        end();
    }
    void draw() {
        draw_box(FL_BORDER_BOX, 0, 0, w(), h(), Fl_Color(175));
        fl_color(FL_BLACK);
        fl_font(labelfont(), 10);
        fl_draw(m_sText, 3, 3, w()-6, h()-6, Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_TOP|FL_ALIGN_WRAP));
    }

    void value(const std::string & sStr) 
    {
        if(sStr.size()<sizeof(m_sText))
        {
            strncpy(m_sText,sStr.c_str(),sizeof(m_sText));
        }

        fl_font(labelfont(), labelsize());
        int W = w(), H = h();
        fl_measure(m_sText, W, H, Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_WRAP|FL_ALIGN_TOP));
        size(W+10, H+10);
        redraw();
    }
};

class CScopeGrid : public Flv_Table  
{
public:
    void SetDBImage(CDBImage * pDBImage);
    void SetComms(CMOOSCommClient* pComms){m_pComms = pComms;};
    void SetTitle(std::string sTitle);

    static void GridCallback(CScopeGrid *pMe, void * )
    {
        pMe->OnGridCallBack();
    }
    CScopeGrid( int X, int Y, int W, int H, const char *l=0 );
    virtual void draw_cell( int Offset, int &X, int &Y, int &W, int &H, int R, int C );
    virtual void get_style( Flv_Style &s, int R, int C );
    virtual void redraw();
    virtual int handle(int e);
    void OnGridCallBack();
    int col_width( int C );
    int m_nCount;

protected:
    std::string GetDataValue(int R,int C);
    bool PokeMOOS(CDBImage::CVar & Var, bool bNew);
    CTipWindow * m_pTW;
    CDBImage * m_pDBImage;
    char m_csTitle[256];

    CMOOSCommClient* m_pComms;

private :
    typedef Flv_Table BASE ;
};

#endif // !defined(AFX_SCOPEGRID_H__E91D0ABA_84EC_4E7A_8AA8_FCC45E216208__INCLUDED_)


