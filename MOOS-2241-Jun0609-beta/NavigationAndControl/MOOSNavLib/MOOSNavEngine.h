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
// MOOSNavEngine.h: interface for the CMOOSNavEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSNAVENGINE_H__17C4BBBD_B280_4686_97B0_C3A1C9F39CB2__INCLUDED_)
#define AFX_MOOSNAVENGINE_H__17C4BBBD_B280_4686_97B0_C3A1C9F39CB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <MOOSLIB/MOOSLib.h>



#include "MOOSNavBase.h"
#include "MOOSObservation.h"
#include "MOOSNavEntity.h"
#include "MOOSSensorChannel.h"
#include "MOOSNavSensor.h"
#include <map>
#include "MOOSNavLogger.h"    // Added by ClassView


class CMOOSNavEntity;
class CMOOSNavVehicle;
class CMOOSNavBeacon;
class CMOOSNavObsStore;
class CMOOSNavSensor;

using namespace std;
typedef list<CMOOSObservation> OBSLIST;
typedef list<CMOOSNavBeacon*> BEACONLIST;
typedef map<string,CMOOSSensorChannel> SOURCE_SENSORCHANNEL_MAP;
typedef list<CMOOSNavEntity*> ENTITY_LIST;

class CMOOSNavEngine  : public CMOOSNavBase
{
    class CObservationStatistic
    {
    public:
        CObservationStatistic();
        string  m_sType;
        int     m_nTotal;
        double  m_dfRejectionRate;

        bool AddPoint(bool bAccepted);
    };

    typedef map<string, CObservationStatistic> OBSSTATISTIC_MAP;
    OBSSTATISTIC_MAP m_ObsStatisticsMap;

public:
    virtual bool    DoObservationSummary();
    double GetTimeStarted();
    bool SetTimeStarted(double dfTime);
    bool SetMissionFileName(const string & sFileName);
    double GetYoungestDataTime();
    CMOOSSensorChannel * GetSensorChannel(const string & sKey);

    bool IsEnabled();
    bool Enable(bool bEnable);
    int GetIterations();
    bool SetOnline(bool bOnline);
    bool IsOnline();
    virtual bool Reset();
    bool ForceTrackedPosition(double dfX,double dfY,double dfZ,double dfH = 0);
    bool GetTrackedPosition(double  & dfX,double & dfY,double &dfZ,double & dfH,double & dfLastUpdate);
    bool GetTrackedUncertainty(double &dfPX,
                               double &dfPY,
                               double &dfPZ,
                               double &dfPH);                                           

    
    
    bool MakeObsMatrices();

    bool GuessVehicleLocation();
    bool GetNextResult(CMOOSMsg & ResultMsg);
    bool AddFixedObservation(CMOOSObservation::Type eType,
                                double dfVal,
                                double dfVariance=-1 );
    
    bool AddAcousticBeacon(const string & sName,
                                       int nChan,
                                       double dfTAT,
                                       double dfX,
                                       double dfY,
                                       double dfZ);

    bool AddSensor(    const string & sSource,
                    const string & sName,
                    CMOOSNavSensor::Type eType,
                    double dfX,
                    double dfY,
                    double dfZ,
                    double dfNoise = -1);

    virtual bool Initialise(STRING_LIST sParams);
    virtual bool AddData(const CMOOSMsg & Msg);
    virtual bool Iterate(double dfTimeNow);
    CMOOSNavEngine();
    virtual ~CMOOSNavEngine();

protected:    
            bool    DoObservationStatistics();
            bool    TraceDiagPhat();
            bool    RecordObsStatistics(Matrix * pInnov,Matrix * pS);
            bool    MarkObservationsDA(bool bGoodDA);
    virtual bool    LogObservationSet(double dfTimeNow, int nthUpdate);

            bool    LimitObservationTypes();
    virtual bool    AddTheVehicle(STRING_LIST & sParams);
            bool    GetTATByChannel(int nChannel,double &dfTAT);
            bool    WrapAngleStates();
            bool    LimitObservations(CMOOSObservation::Type eType,int nNumber);
            bool    SetUpGlobalStates();
            bool    TraceObservationSet();
            bool    IndexObservations(int & nObsDim);
            bool    AddEntity(CMOOSNavEntity * pEntity,
                    Matrix * pCovariance=NULL,
                    Matrix * pCrossCovariance=NULL);

            //observation creation
            bool    DataToObservations(const CMOOSMsg & Msg,OBSLIST & Obs);
            bool    MakeControlObservations(const CMOOSMsg & Msg, OBSLIST & ObsList);
            bool    MakeBodyVelObservations(const CMOOSMsg & Msg, OBSLIST & ObsList,double dfSF = 1);
            bool    MakeXYObservations(const CMOOSMsg & Msg, OBSLIST & ObsList);
            bool    MakeLBLObservations(const CMOOSMsg & Msg,OBSLIST & Obs);
            bool    MakeDepthObservations(const CMOOSMsg & Msg,OBSLIST & Obs);
            bool    MakeYawObservations(const CMOOSMsg & Msg,OBSLIST & Obs);

            //this is sensor based rejection
            bool    AgreesWithHistory(CMOOSObservation & rObs);
            bool    AddToHistory(CMOOSObservation & rObs);
            bool    SetUpSensorChannels(STRING_LIST  sParams,string sToken);
            bool    PreFilterData();


            int        GetNextID(){return m_nNextID++;};    
    
    CMOOSNavSensor*        GetSensorBySource(const string & sMOOSSource,const string & sDataName);
    CMOOSNavBeacon *    GetBeaconByChannel(int nChannel);
    CMOOSNavBeacon *    GetBeaconByName(const string & sName);
    CMOOSNavSensor*        GetSensorByName(const string &sSensorName);
    CMOOSNavSensor::Type SensorTypeFromDataName(const string & sDataName);



    BEACONLIST                    m_Beacons;
    int                            m_nNextID;
    double                        m_dfTimeStarted;
    CMOOSNavLogger                m_Logger;
    SENSOR_MAP                    m_SourceToSensorMap;
    SOURCE_SENSORCHANNEL_MAP    m_SensorChannelMap;
    OBSLIST                        m_Observations;
    OBSLIST                        m_FixedObservations;


    Matrix m_jH;
    Matrix m_R;
    Matrix m_Innov;
    Matrix m_Xhat;
    Matrix m_Phat;
    Matrix m_XhatTmp;
    Matrix m_PhatTmp;
    Matrix m_Ihat;

    CMOOSNavObsStore*        m_pStore;
    CMOOSNavVehicle *        m_pTracked;
    MOOSMSG_LIST            m_ResultsList;

    CMOOSNavEntity::Type    m_eVehicleType;




    double    m_dfLastUpdate;
    double    m_dfUpdatePeriod;
    double    m_dfTimeNow;
    int     m_nIterations;

    bool    m_bEnabled;
    bool    m_bOnline;
    bool    m_bInitialOnline;

    //true is we can map thrust to velocity...
    bool    m_bThrust2Vel;
    double  m_dfThrust2VelGain;
    double    m_dfSV;

    //are we using heading bias state?
    bool    m_bEstimateHeadingBias;

    //name of mission file
    string    m_sMissionFileName;

};

#endif // !defined(AFX_MOOSNAVENGINE_H__17C4BBBD_B280_4686_97B0_C3A1C9F39CB2__INCLUDED_)
