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
// uMS.cpp : Defines the entry point for the console application.

// Paul Newman 2005





#include "ScopeGrid.h"
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Browser.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Preferences.H>
#include <sstream>
#include "ScopeTabPane.h"
#include <FLTKVW/Flv_Table.H>

#define DEFAULT_HEIGHT 460
#define DEFAULT_WIDTH 820


//called to produce a new pane
Fl_Group * MakeCommunityPane(int x ,int y, int w, int h,const std::string &  Name)
{
    char* nName = new char[Name.size()+1];
    strcpy(nName,Name.c_str());
    Fl_Group *pG = new CScopeTabPane(x,y,w,h,nName);
    return pG;
}

//callback for save
void OnSave(Fl_Widget* pWidget,void * pParam)
{
    Fl_Tabs* pTab = (Fl_Tabs*)(pParam);
        std::ostringstream os;
        for(int i = 0;i<pTab->children();i++)
        {
                CScopeTabPane * pTabPane = (CScopeTabPane*)(pTab->child(i));
                std::string sHost;
                int nPort;
                pTabPane->GetMOOSInfo(sHost,nPort);
                os<<pTabPane->label()<<":"<<nPort<<"@"<<sHost<<",";
        }

        //here we try to load previous preferences
        Fl_Preferences app( Fl_Preferences::USER, "MOOS", "uMS" );
        app.set("Communities",os.str().c_str());
}

//callback for delete
void OnRemoveCommunity(Fl_Widget* pWidget,void * pParam)
{
    Fl_Tabs* pTab = (Fl_Tabs*)(pParam);

    if(pTab->children()>1)
    {       
    if(fl_ask(MOOSFormat("Really delete Scope of \"%s\" ?",pTab->value()->label()).c_str()))
    {
        Fl_Group* pG = (Fl_Group*)pTab->value();
        pG->clear();
        pTab->remove(pG);
        pTab->value(pTab->child(0));
        delete pG;
    }
    }
}



void OnRenameCommunity(Fl_Widget* pWidget,void * pParam)
{
    Fl_Tabs* pTab = (Fl_Tabs*)(pParam);
    const char * scName=fl_input("Enter a new name", "");
    if(scName==NULL)
    return ;
    
    char * sName = new char[strlen(scName)*2];
    strcpy(sName,scName);
    Fl_Widget* pW = (Fl_Widget*)pTab->value();
    
    pW->label(scName);
}


//callback for add Community
void OnAddCommunity(Fl_Widget* pWidget,void * pParam)
{
    const char * scName=fl_input("New Community Name", "");
    if(scName==NULL)
    return ;
    
    char * sName = new char[strlen(scName)*2];
    strcpy(sName,scName);
    
    Fl_Tabs* pTab = (Fl_Tabs*)(pParam);
    Fl_Group* pG = (Fl_Group*)pTab->value();

    pTab->begin();
    MakeCommunityPane(10,30,pG->w()-20,pG->h()-10,sName);
    pTab->end();
    
}

//heres the money.....
int main(int argc, char* argv[])
{
    
    //make the frame
    Fl_Double_Window *w = new Fl_Double_Window( 820, 460, "uMS" );
    w->size_range(DEFAULT_WIDTH,DEFAULT_HEIGHT);
    
    //OK - lets build a whole set of panes
    std::string sWho;
    {
    //note scoping
    Fl_Preferences app( Fl_Preferences::USER, "MOOS", "uMS" );
    char Space[2048];
    app.get("Communities",Space,"Unnamed:9000@LOCALHOST",sizeof(Space));
    sWho = std::string(Space);
    }
    
    Fl_Tabs* pTab = new Fl_Tabs(10, 10, 800, 400);
    
    while(!sWho.empty())
    {
    std::string sChunk = MOOSChomp(sWho,",");
    std::string sName = MOOSChomp(sChunk,":");
    int nPort = atoi(MOOSChomp(sChunk,"@").c_str());
    std::string sHost = sChunk;
    if(!sChunk.empty() && nPort>0)
    {
        Fl_Group *pG  = MakeCommunityPane(10,30,800,390,(char*)sName.c_str());
        CScopeTabPane * pTabPane = (CScopeTabPane*)(pG);
        pTabPane->SetMOOSInfo(sHost,nPort);
        Fl_Group::current()->resizable(pG);
    }
    }
    if(pTab->children()==0)
    {
    Fl_Group *pG  = MakeCommunityPane(10,30,800,390,"Unnamed");             
    }
    
    w->end();


    //and add some buttons to control them....
    w->begin();
    {
    //add
    Fl_Button * pNewCommunityButton = new Fl_Button(10,420,160,30,"Add Community");
    pNewCommunityButton->callback(OnAddCommunity,pTab);
    
    //delete
    Fl_Button * pDelCommunityButton = new Fl_Button(180,420,160,30,"Remove Community");
    pDelCommunityButton->callback(OnRemoveCommunity,pTab);
    
    //save
    Fl_Button * pSaveButton = new Fl_Button(350,420,160,30,"Save Layout");
    pSaveButton->callback(OnSave,pTab);
    pSaveButton->callback();
    
    //rename
    Fl_Button * pRenameButton = new Fl_Button(520,420,160,30,"Rename");
    pRenameButton->callback(OnRenameCommunity,pTab);
    pRenameButton->callback();
    
    }
    w->end();
    
    
    //need to do more work on resizing       - maybe even read the manual
    w->resizable(pTab);
    
    
    //and GO!
    w->show(argc, argv);
    return Fl::run();
    
}

