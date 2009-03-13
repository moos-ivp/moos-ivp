///////////////////////////////////////////////////////////////////////////
//
//   MOOS - Mission Oriented Operating Suite 
//  
//   A suit of Applications and Libraries for Mobile Robotics Research 
//   Copyright (C) 2001-2005 Massachusetts Institute of Technology and 
//   Oxford University. 
//    
//   This software was written by Paul Newman and others
//   at MIT 2001-2002 and Oxford University 2003-2005.
//   email: pnewman@robots.ox.ac.uk. 
//      
//   This file is part of a  MOOS Basic (Common) Application. 
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
// MOOSTaskReader.cpp: implementation of the CMOOSTaskReader class.
//
//////////////////////////////////////////////////////////////////////
#include "MOOSBehaviour.h"
#include "MOOSTaskDefaults.h"
#include "LimitDepth.h"
#include "SeabedTrack.h"
#include "LimitAltitude.h"
#include "GoToWayPoint.h"
#include "EndMission.h"
#include "TimerTask.h"
#include "HoldingPatternTask.h"
#include "ConstantDepthTask.h"
#include "ConstantHeadingTask.h"
#include "GoToDepth.h"
#include "ZPatternTask.h"
#include "XYPatternTask.h"
#include "SteerThenDriveXYPatternTask.h"
#include "OrbitTask.h"
#include "OverallTimeOut.h"
#include "SurveyTask.h"
#include "ThirdPartyTask.h"
#include "TrackLineTask.h"
#include "LimitBox.h"
#include "PilotTask.h"
#include "VariableHeadingTask.h"
#include "AGVHeadingSpeedTask.h"
#include "VehicleFrameWayPointTask.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
#include "MOOSTaskReader.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSTaskReader::CMOOSTaskReader()
{

}

CMOOSTaskReader::~CMOOSTaskReader()
{

}

bool CMOOSTaskReader::Run(const char *sFile, 
                          CProcessConfigReader* pMissionFileReader,
                          CMOOSBehaviour::CControllerGains Gains,
                          TASK_LIST &Tasks)
{


    if(SetFile(sFile))
    {
        Reset();
        CMOOSBehaviour * pNewTask = NULL;

        if(!GetFile()->is_open())
        {

            MOOSTrace("CMOOSTaskReader::failed to open task file %s\n",sFile);
            return false;
        }

        while(!GetFile()->eof())
        {
            string sLine = GetNextValidLine();   

            MOOSToUpper(sLine);

            string sTok,sVal;
            if(GetTokenValPair(sLine,sTok,sVal))
            {
                if(sTok=="TASK")
                {
                    pNewTask = MakeNewTask(sVal);    

                    if(pNewTask!=NULL)
                    {
                        //maybe it needs the mission file to get parameters etc
                        pNewTask->SetMissionFileReader(pMissionFileReader);

                        if(StuffTask(pNewTask))
                        {
                            //we have already read the gins for it (stops possibly dynaically
                            //made task having to do file parsing!
                            pNewTask->SetGains(Gains);

                            Tasks.push_front(pNewTask);
                        }
                        else
                        {
                            MOOSTrace("task config failed\n");
                            delete pNewTask;
                            return false;
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
    }
    else
    {
        MOOSTrace("Cannot open file containing task specification!!!\n");
        return false;
    }

    MOOSTrace("TaskReader makes %d tasks\n",Tasks.size());

    return true;
}



bool CMOOSTaskReader::StuffTask(CMOOSBehaviour *pTask)
{
    
    string sBracket = GetNextValidLine();
    
    if(sBracket!="{")
    {
        MOOSTrace("CMOOSTaskReader::StuffTask ->no opening bracket\n");
        delete pTask;
        return false;
    }

    string sLine;
    while((sLine=GetNextValidLine())!="}")
    {
        string sTok,sVal;
        
        if(GetTokenValPair(sLine, sTok, sVal))
        {
            if(!pTask->SetParam(sTok,sVal))
            {
                MOOSTrace("failed parse...line = %s",sLine.c_str());
                return false;               
            }
        }
        else
        {
            MOOSTrace("failed parse...line = %s",sLine.c_str());
            return false;
        }
    }


    return true;
}

CMOOSBehaviour * CMOOSTaskReader::MakeNewTask(string sTaskType)
{
    //add new task types  here..
    CMOOSBehaviour * pNewTask = NULL;

    MOOSToUpper(sTaskType);

    if(sTaskType =="GOTOWAYPOINT")
    {
        pNewTask = new CGoToWayPoint;
    }
    else if(sTaskType =="TRACKLINE")
    {
        pNewTask = new CTrackLineTask;
    }
    else if(sTaskType =="OVERALLTIMEOUT")
    {
        pNewTask = new COverallTimeOut;
    }
    else if(sTaskType =="ORBIT")
    {
        pNewTask = new COrbitTask;
    }
    else if(sTaskType =="XYPATTERN")
    {
        pNewTask = new CXYPatternTask;
    }
    else if(sTaskType =="STEERTHENDRIVEXYPATTERN")
    {
        pNewTask = new CSteerThenDriveXYPatternTask;
    }
    else if(sTaskType =="THIRDPARTY")
    {
        pNewTask = new CThirdPartyTask;
    }
    else if(sTaskType =="GOTODEPTH")
    {
        pNewTask = new CGoToDepth;
    }
    else if(sTaskType == "LIMITDEPTH")
    {
        pNewTask = new CLimitDepth;
    }
    else if(sTaskType == "SEABEDTRACK")
    {
        pNewTask = new CSeabedTrack;
    }
    else if(sTaskType == "LIMITALTITUDE")
    {
        pNewTask = new CLimitAltitude;
    }
    else if(sTaskType == "ENDMISSION")
    {
        pNewTask = new CEndMission;
    }
    else if(sTaskType == "TIMER")
    {
        pNewTask = new CTimerTask;
    }
    else if(sTaskType == "HOLDINGPATTERN")
    {
        pNewTask = new CHoldingPatternTask;
    }    
    else if(sTaskType == "CONSTANTDEPTH")
    {
        pNewTask = new CConstantDepthTask;
    }
    else if(sTaskType == "CONSTANTHEADING")
    {
        pNewTask = new CConstantHeadingTask;
    }
    else if(sTaskType == "DIVE")
    {
        MOOSTrace("Dive task is depreciated, use GoToDepth\n");
        return NULL;
    }
    else if(sTaskType == "ZPATTERN")
    {
        pNewTask = new CZPatternTask;
    }
    else if(sTaskType == "SURVEY")
    {
        pNewTask = new CSurveyTask;
    }
    else if(sTaskType == "LIMITBOX")
    {
        pNewTask = new CLimitBox;
    }
    else if(sTaskType == "PILOT")
    {
        pNewTask = new CPilotTask;
    }
    else if(sTaskType == "VARIABLEHEADING")
    {
        pNewTask = new CVariableHeadingTask;
    }
    else if(MOOSStrCmp(sTaskType,"AGVHeadingSpeed"))
    {
        pNewTask = new CAGVHeadingSpeedTask;
    }
    else if(MOOSStrCmp(sTaskType,"VehicleFrameWayPoint"))
    {
        pNewTask = new CVehicleFrameWayPointTask;
    }
    
    else
    {
        MOOSTrace("Task Type \"%s\" is unknown\n",sTaskType.c_str());
    }
        
    return pNewTask;
}
