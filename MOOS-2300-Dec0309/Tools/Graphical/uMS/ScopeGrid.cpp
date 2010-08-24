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
// ScopeGrid.cpp: implementation of the CScopeGrid class.


#ifdef _WIN32
#pragma warning(disable : 4786)
#endif

#include <string>
#include "ScopeGrid.h"
#include <FL/fl_draw.H>
#include <MOOSGenLib/MOOSGenLibGlobalHelper.h>
#include <FLTKVW/MOOSFLTKUI.h>
#include <FL/Fl_Input.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Round_Button.H>
#include <FL/fl_ask.H>


//this is a file scope utitlity class - used for poking the MOOS
class CPokeDlg : public CMOOSFLTKUI
{
    typedef CMOOSFLTKUI BASE;
public:

    enum{OK,CANCEL,STRING,NUMERIC,POKE};
    std::string m_sVal,m_sType,m_sName;
    double m_dfVal;
    bool m_bGood;
    bool IsGood(){return m_bGood;};
    bool IsDouble(){return m_sType=="D";};
    bool IsString(){return m_sType=="$";};
    std::string GetName(){return m_sName;};
    std::string GetString(){return m_sVal;};
    double GetDouble(){return m_dfVal;};


    CPokeDlg(int x,int y, int w, int h, const char * Name=NULL):BASE(x,y,w,h,Name)
    {
        m_bGood = false;
        Fl_Input* pValue = new  Fl_Input(10,20,w-20,20);
        SetID(pValue,POKE);
        pValue->align(FL_ALIGN_TOP);
        Fl_Button* pOK = new Fl_Button(w-70,h-30,60,20,"OK");
        Fl_Button* pQuit = new Fl_Button(10,h-30,60,20,"Cancel");
        SetID(pOK,OK);
        SetID(pQuit,CANCEL);
    }

    void Configure(std::string sType, std::string sVal,std::string sName)
    {
        m_sVal = sVal;
        m_sName =sName;

        ((Fl_Input*)GetByID(POKE))->value(m_sVal.c_str());

        if(sType=="*" || sType=="?")
        {
            if(sType=="*")
            {
                //this is a new variable - better ask for a name
                const char * pStr = fl_input("Enter name for new variable");
                if(pStr==NULL)
                {
                    return;
                }
                m_sName = std::string(pStr);
            }

            int nChoice = fl_choice("Variable is ","cancel","numeric","string");

            switch(nChoice)
            {
            case 1: m_sType = "D"; break;
            case 2: m_sType = "$"; break;
            default : return;
            }

        }
        else
        {
            m_sType = sType;
        }

        static char Name[1024];
        sprintf(Name,"new value for %s is....",m_sName.c_str());
        ((Fl_Input*)GetByID(POKE))->label(Name);

    }

    void OnControlWidget(Fl_Widget * pW,int nID)
    {
        switch(nID)
        {
        case OK:
            {
                std::string sVal = ((Fl_Input*)GetByID(POKE))->value();

                if(m_sType=="$"  )
                {
                    if(!MOOSIsNumeric(sVal) || sVal.empty())
                        m_sVal = sVal;
                    else
                    {
                        fl_alert("must be string - this looks like a number");
                        return;
                    }
                }
                else if(m_sType=="D")
                {
                    if(MOOSIsNumeric(sVal) && !sVal.empty() )
                        m_dfVal = atof(sVal.c_str());
                    else
                    {
                        fl_alert("must be double - this looks like a string");
                        return;
                    }
                }

                //and we are all done!
                m_bGood = true;
                GetRootWindow()->hide();
            }
            break;
        case CANCEL:
            GetRootWindow()->hide();
            break;

        }
    }

};



/////////////////////////////////////////////////////////////////////////


CScopeGrid::CScopeGrid( int X, int Y, int W, int H, const char *l ) :        Flv_Table(X,Y,W,H,l)
{
    m_pDBImage = NULL;
    m_nCount = 0;
    m_pComms = NULL;
    //default size
    rows(500);
    cols(7);
    feature(FLVF_HEADERS|FLVF_DIVIDERS|FLVF_MULTI_SELECT|FLVF_FULL_RESIZE);
    feature_remove(FLVF_MULTI_SELECT | FLVF_COL_HEADER  );

    //fonts etc for headers
    Flv_Style s;
    Flv_Table::get_style(s,-1,0);
    row_style[-1].align(FL_ALIGN_CLIP);
    row_style[-1].font( (Fl_Font)(s.font()+FL_BOLD));
    col_style[1].align(FL_ALIGN_CENTER);
    col_style[2].align(FL_ALIGN_CENTER);
    col_style[3].align(FL_ALIGN_CENTER);
    col_style[4].align(FL_ALIGN_CENTER);
    col_style[5].align(FL_ALIGN_CENTER);
    col_style[6].align(FL_ALIGN_TOP_LEFT);

    global_style.font_size(12);
    //set up callbacks
    callback_when( FLVEcb_CLICKED );
    callback((Fl_Callback*)GridCallback);
    max_clicks(2);

    //set up hint window for veiwing data
    Fl_Group *save = Fl_Group::current();   // save current widget..
    m_pTW = new CTipWindow();                  // ..because this trashes it
    m_pTW->hide();
    Fl_Group::current(save);                // ..then back to previous.

    strcpy(m_csTitle,"Not connected");
    label(m_csTitle);
};



void CScopeGrid::SetTitle(std::string sTitle)
{
    strncpy(m_csTitle,sTitle.c_str(),sizeof(m_csTitle));
}


std::string CScopeGrid::GetDataValue(int R,int C)
{
    CDBImage::CVar Var;
    if(m_pDBImage!=NULL)
    {
        if(m_pDBImage->Get(Var,R))
        {
            switch(C)
            {
            case 0:
                return Var.GetName();
                break;
            case 1:
                return Var.GetTime();
                break;
            case 2:
                return Var.GetType();
                break;
            case 3:
                return Var.GetFrequency();
            case 4:
                return Var.GetSource();
            case 5:
                return Var.GetCommunity();
            case 6:
                return Var.GetValue();
                break;
            }
        }
    }
    return "";
}


bool CScopeGrid::PokeMOOS(CDBImage::CVar & Var,bool bNew)
{

    Fl_Window W(20,20,400,100,"Poke the MOOS");
    CPokeDlg D(0,0,400,100,"Poke the MOOS");

    if(bNew)
    {
        //user has clicked on empty cell - wanting to make a new variable
        D.Configure("*","","");
    }
    else
    {
        D.Configure(Var.GetType(),Var.GetValue(),Var.GetName());
    }

    W.show();
    while(W.shown())
        Fl::wait();

    if(D.IsGood())
    {
        if(m_pComms  && m_pComms->IsConnected())
        {
            if(D.IsDouble())
            {
                m_pComms->Notify(D.GetName(),D.GetDouble());
            }
            if(D.IsString())
            {
                m_pComms->Notify(D.GetName(),D.GetString());
            }
        }
    }

    return true;


}

void CScopeGrid::OnGridCallBack()
{
    if(why_event()==FLVE_CLICKED)
    {
        //this is a double click
        int c = select_start_col();
        int r = select_start_row();
        std::string sVal = GetDataValue(r,c);

        if(c==6)
        {
            m_pTW->value(sVal);
        }
        else
        {
            m_pTW->hide();
            if(c==0)
            {
                //hold control down to poke...
                if((Fl::event_key(FL_Control_L)||Fl::event_key(FL_Control_R)))
                {
                    CDBImage::CVar Var;
                    if(m_pDBImage->Get(Var,r))
                    {
                        PokeMOOS(Var,false);
                    }
                    else
                    {
                        PokeMOOS(Var,true);
                    }
                }
            }
        }
    }
}





void CScopeGrid::redraw()
{
    int c = select_start_col();
    int r = select_start_row();
    if(c==6)
    {
        std::string sVal = GetDataValue(r,c);
        m_pTW->value(sVal);
    }

    BASE::redraw();
}

int CScopeGrid::handle(int e)
{
    switch(e)
    {
    case FL_PUSH:
        // XXX: if offscreen, move tip ABOVE mouse instead
        m_pTW->position(Fl::event_x_root(), Fl::event_y_root());
        m_pTW->show();
        break;
    case FL_HIDE:       // valuator goes away
    case FL_RELEASE:    // release mouse
    case FL_LEAVE:      // leave focus
        // Make sure tipwin closes when app closes
        m_pTW->hide();
        break;
    }
    return(BASE::handle(e));
}


void CScopeGrid::draw_cell( int Offset, int &X, int &Y, int &W, int &H, int R, int C )
{
    Flv_Style s;
    get_style(s, R, C);


    Flv_Table::draw_cell(Offset,X,Y,W,H,R,C);

    if(R==-1)
    {
        //this is the header:
        const char * ColumnNames[] = {"Name","Time","Type","Freq","Source","Community","Value"};
        const char * pStr = C<sizeof(ColumnNames) ? ColumnNames[C] : "";
        fl_draw(pStr, X-Offset, Y, W, H, s.align() );
    }
    else
    {
        //FIX for FLTK escaping of "@" PMN May 2005
        std::string sStr = GetDataValue(R,C).c_str();
        std::string sFLTK;
        while(!sStr.empty())
        {
            sFLTK += MOOSChomp(sStr,"@");
            if(!sStr.empty())
                sFLTK+="@@";
        }
        fl_draw(sFLTK.c_str(), X-Offset, Y, W, H, s.align() );
    }
}


void CScopeGrid::get_style( Flv_Style &s, int R, int C )
{
    Flv_Table::get_style(s,R,C);                    //    Get standard style
    if(R%2==0)
    {
        s.background( (Fl_Color)(FL_GRAY_RAMP+22) );
    }
    if(C==6 && m_pDBImage->HasChanged(R))
    {
        s.foreground( (Fl_Color)(FL_RED) );
    }
}



int CScopeGrid::col_width( int C )
{
    static int LW=-1;
    static int cw[7];
    int scrollbar_width = (scrollbar.visible()?scrollbar.w():0);
    int W = w()- scrollbar_width-1;
    int ww, t;

    //    Either always calculate or be sure to check that width
    //    hasn't changed.
    if (W!=LW)                            //    Width change, recalculate
    {
        //char * ColumnNames[] = {"Name","Time","Type","Freq","Source","Community","Value"};
        cw[0] = (W*20)/100;        //    30        Name
        cw[1] = (W*10)/100;        //    10        Time
        cw[2] = (W*4)/100;        //    15%        Type
        cw[3] = (W*6)/100;        //     8%        Freq
        cw[4] = (W*10)/100;        //    15%        Src
        cw[5] = (W*10)/100;        //    15%        Comm
        cw[6] = (W*30)/100;        //    15%        Val


        for (ww=0, t=0;    t<6;    t++ )
        {
            ww += cw[t];
        }
        cw[6] = W-ww-1;                     //    ~30% +/- previous rounding errors
        LW = W;
    }
    return cw[C];

}

void CScopeGrid::SetDBImage(CDBImage *pDBImage)
{
    m_pDBImage = pDBImage;
}
