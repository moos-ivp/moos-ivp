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
// MOOSObservation.cpp: implementation of the CLBLMaths class.
//
//////////////////////////////////////////////////////////////////////

#include <math.h>
#include "MOOSNavEntity.h"
#include "MOOSNavVehicle.h"

#include "MOOSNavBeacon.h"

#include "MOOSNavSensor.h"
#include "LBLMaths.h"
#include "MOOSNavLibGlobalHelper.h"

#define Diff1_TT_wrt_State(a,b,c,a1,b1,c1,T) \
    ((((T)==0)&&((b)==0))?0:(-((c1)+(b1)*(T)+(a1)*((c)+(T)*((b)+2*(a)*(T)))/(a))/((b)+2*(a)*(T))))



bool CLBLMaths::CalculateTwoWayTOF(CMOOSNavSensor *pInterrogatorSensor,
                                          CMOOSNavSensor *pResponderSensor,
                                          Matrix * pXEvaluate,
                                          double dfSV,
                                          double & dfTotalTOF
                                          )
{
    ///////////////////////////////////////////
    //I tested speed of thi function on a P3
    //took on average 20 uSeconds to complete
    //not a problem by any means
    //running on a machine 100 times slower it
    //would only take 2000uS = 2ms. No problem.
    
    
    
    //pointers to sensors (we shall swap what they point to
    //for different legs 
    CMOOSNavSensor * pTx;
    CMOOSNavSensor * pRx;
    
    ///////////////////////////////////////////////////
    //            calculate last leg:
    ///////////////////////////////////////////////////
    double dfLastLegTOF=0;
    
    pTx = pResponderSensor;
    pRx = pInterrogatorSensor;
    
    if(!CalculateLegTravelTime(0,pTx,pRx,pXEvaluate,dfSV,dfLastLegTOF))
    {
        return false;
    }
    
    ///////////////////////////////////////////////////
    //            now calculate first leg:
    ///////////////////////////////////////////////////
    
    double dfTAT=0;
    CMOOSNavBeacon * pBeacon = dynamic_cast<CMOOSNavBeacon*>(pResponderSensor->GetParent());        
    if(pBeacon!=NULL)
    {
        dfTAT = pBeacon->m_dfTAT;
    }
    
    double dfTimeAgo = (dfLastLegTOF+dfTAT);
    
    pTx = pInterrogatorSensor;
    pRx = pResponderSensor;
    
    double dfFirstLegTOF=0;
    
    if(!CalculateLegTravelTime(dfTimeAgo,pTx,pRx,pXEvaluate,dfSV,dfFirstLegTOF))
    {
        return false;
    }
    

    //finally we get our result :-)
    dfTotalTOF = dfLastLegTOF+dfTAT+dfFirstLegTOF;
    
    return true;
}



bool CLBLMaths::CalculateTwoWayTOFJacobians(CMOOSNavSensor *pInterrogatorSensor,
                                          CMOOSNavSensor *pResponderSensor,
                                          Matrix * pXEvaluate,
                                          double dfSV,
                                          Matrix & jH
                                          )
{

    int nCols = pXEvaluate->Nrows();

    if(jH.Nrows()!=1 || jH.Ncols()!=nCols)
    {
        jH.ReSize(1,nCols);
        jH = 0;
    }

       
    //pointers to sensors (we shall swap what they point to
    //for different legs 
    CMOOSNavSensor * pTx;
    CMOOSNavSensor * pRx;
    
    ///////////////////////////////////////////////////
    //            calculate last leg:
    ///////////////////////////////////////////////////
   
    double dfTimeAgo = 0;
    double dfLastLegTOF;
    double dfFirstLegTOF;

    pTx = pResponderSensor;
    pRx = pInterrogatorSensor;

    SetUpLBLData(dfTimeAgo,pTx,pRx,pXEvaluate,dfSV);

    if(!CalculateLegTravelTime(dfTimeAgo,pTx,pRx,pXEvaluate,dfSV,dfLastLegTOF))
    {
        return false;
    }

    if(!AddJacobianLegComponent(jH,pTx,pRx,dfLastLegTOF,dfTimeAgo))
    {
        return false;
    }
    

    ///////////////////////////////////////////////////
    //            now calculate first leg:
    ///////////////////////////////////////////////////

    
    //look for TAT
    double dfTAT=0;
    CMOOSNavBeacon * pBeacon = dynamic_cast<CMOOSNavBeacon*>(pResponderSensor->GetParent());        
    if(pBeacon!=NULL)
    {
        dfTAT = pBeacon->m_dfTAT;
    }




    dfTimeAgo = (dfLastLegTOF+dfTAT);
    
    pTx = pInterrogatorSensor;
    pRx = pResponderSensor;
    
    SetUpLBLData(dfTimeAgo,pTx,pRx,pXEvaluate,dfSV);

    if(!CalculateLegTravelTime(dfTimeAgo,pTx,pRx,pXEvaluate,dfSV,dfFirstLegTOF))
    {
        return false;
    }

    if(!AddJacobianLegComponent(jH,pTx,pRx,dfFirstLegTOF,dfTimeAgo))
    {
        return false;
    }

    return true;
}    
 

bool CLBLMaths::CalculateLegTravelTime(double dfTimeAgo,
                                              CMOOSNavSensor * pTxSensor,
                                              CMOOSNavSensor * pRxSensor,
                                               Matrix * pXEvaluate,
                                               double dfSV,
                                              double & dfTravelTimeResult
                                             )
{
    



    if(!SetUpLBLData(dfTimeAgo,pTxSensor,pRxSensor,pXEvaluate,dfSV))
        return false;



    dfTravelTimeResult =  (-b-sqrt(b*b-4*a*c))/(2*a);
    
    return true;
}


bool CLBLMaths::SetUpLBLData(double dfTimeAgo,
                                    CMOOSNavSensor *pTxSensor,
                                    CMOOSNavSensor *pRxSensor,
                                    Matrix * pXEvaluate,
                                    double dfSV)
{

    m_dfSV = dfSV;

    //get coordinates and rates of vehicles involved in
    //cardinal coordinates    
    Matrix VehTx,VehRx;
    
    //her we get the entities to manage things themselves
    //they know best...
    if(!pTxSensor->GetParent()->GetFullState(VehTx,pXEvaluate))
        return false;
    
    if(!pRxSensor->GetParent()->GetFullState(VehRx,pXEvaluate))
        return false;
    
    //now extract single numbers for use later on
    Rx_Veh_X = I_X(VehRx,1);
    Rx_Veh_Y = I_Y(VehRx,1);
    Rx_Veh_Z = I_Z(VehRx,1);
    Rx_Veh_H = I_H(VehRx,1);
    
    Rx_Veh_Xdot = I_Xdot(VehRx,1);
    Rx_Veh_Ydot = I_Ydot(VehRx,1);
    Rx_Veh_Zdot = I_Zdot(VehRx,1);
    Rx_Veh_Hdot     = I_Hdot(VehRx,1);
    
    Tx_Veh_X = I_X(VehTx,1);
    Tx_Veh_Y = I_Y(VehTx,1);
    Tx_Veh_Z = I_Z(VehTx,1);
    Tx_Veh_H = I_H(VehTx,1);
    
    Tx_Veh_Xdot = I_Xdot(VehTx,1);
    Tx_Veh_Ydot = I_Ydot(VehTx,1);
    Tx_Veh_Zdot = I_Zdot(VehTx,1);
    Tx_Veh_Hdot     = I_Hdot(VehTx,1);
    
    //now get sensor coordinates
    
    //need to convert to global aligned frame
    pRxSensor->GetAlignedOffsets(Rx_Veh_H,Rx_Sen_X,Rx_Sen_Y,Rx_Sen_Z);
    pTxSensor->GetAlignedOffsets(Tx_Veh_H,Tx_Sen_X,Tx_Sen_Y,Tx_Sen_Z);
    
    
    /*
    dE = (E of Rx Sen - E of Tx Sen) instantaneously.
    dN = (N of Rx Sen - N of Tx Sen) instantaneously.
    dZ = (Z of Rx Sen - Z of Tx Sen) instantaneously.
    
      a =  SQR(TxVelE-TxSenN*TxAngVel)+SQR(TxVelN+TxSenE*TxAngVel)+SQR(TxVelD)-SQR(SVel)
      b =  2*((dE*(TxVelE-TxSenN*TxAngVel))+(dN*(TxVelN+TxSenE*TxAngVel))+(dZ*TxVelZ))
      c =  SQR(dE)+SQR(dN)+SQR(dZ)
    */
    //figure out velocity of Tx sensor.. 
    Tx_Sen_Xdot = Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot;
    Tx_Sen_Ydot = Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot;
    
    dE = ((Rx_Veh_X+Rx_Sen_X -
        (Rx_Veh_Xdot-Rx_Sen_Y*Rx_Veh_Hdot)*dfTimeAgo) -
        (Tx_Veh_X+Tx_Sen_X - Tx_Sen_Xdot*dfTimeAgo));
    
    dN = ((Rx_Veh_Y+Rx_Sen_Y -
        (Rx_Veh_Ydot+Rx_Sen_X*Rx_Veh_Hdot)*dfTimeAgo) -
        (Tx_Veh_Y+Tx_Sen_Y - Tx_Sen_Ydot*dfTimeAgo));
    
    dZ = ((Rx_Veh_Z+Rx_Sen_Z-Rx_Veh_Zdot*dfTimeAgo) -
        (Tx_Veh_Z+Tx_Sen_Z-Tx_Veh_Zdot*dfTimeAgo));
    
        /* Ugly bodge to prevent possibility of solution
        becoming trapped on one of the cardinal axes... 
    interesting*/
    
    dE=fabs(dE)<1e-3?MOOSWhiteNoise(1e-4):dE;
    dN=fabs(dN)<1e-3?MOOSWhiteNoise(1e-4):dN;
    dZ=fabs(dZ)<1e-3?MOOSWhiteNoise(1e-4):dZ;
    
    a = SQR(Tx_Sen_Xdot)+SQR(Tx_Sen_Ydot)+SQR(Tx_Veh_Zdot)-SQR(m_dfSV);
    b = 2*(dE*Tx_Sen_Xdot+dN*Tx_Sen_Ydot+dZ*Tx_Veh_Zdot);
    c = SQR(dE)+SQR(dN)+SQR(dZ);       
    

  
    return true;

}


bool CLBLMaths::AddJacobianLegComponent(Matrix & H,
                                        CMOOSNavSensor* pTxSensor,
                                        CMOOSNavSensor* pRxSensor,
                                         double TTime,
                                         double dt)
{

    CMOOSNavVehicle* pVehRx = dynamic_cast< CMOOSNavVehicle * >(pRxSensor->GetParent());
    CMOOSNavVehicle* pVehTx = dynamic_cast< CMOOSNavVehicle * >(pTxSensor->GetParent());


    bool bRxIsVehicle = pVehRx!=NULL;
    bool bTxIsVehicle = pVehTx!=NULL;

    int nObsNdx = 1;

    if(bRxIsVehicle)
    {
        int RxNdx = pVehRx->m_nStart;

        H(nObsNdx,RxNdx+iiX)+= dTTbydEr(TTime);
        H(nObsNdx,RxNdx+iiY)+= dTTbydNr(TTime);
        H(nObsNdx,RxNdx+iiZ)+= dTTbydZr(TTime);
        H(nObsNdx,RxNdx+iiH)+= dTTbydHr(TTime,dt);
        
        if(pVehRx->m_eType==CMOOSNavVehicle::POSE_AND_RATE)
        {
            H(nObsNdx,RxNdx+iiXdot)+= dTTbydErvel(TTime,dt);
            H(nObsNdx,RxNdx+iiYdot)+= dTTbydNrvel(TTime,dt);
            H(nObsNdx,RxNdx+iiZdot)+= dTTbydZrvel(TTime,dt);
            H(nObsNdx,RxNdx+iiHdot)+= dTTbydHrvel(TTime,dt);
        }
    }

    //if we are imuune to acoustic observations don't add any thing here..
    if( bTxIsVehicle)
    {
        int TxNdx = pVehTx->m_nStart;

        
        H(nObsNdx,TxNdx+iiX)+= dTTbydEt(TTime);
        H(nObsNdx,TxNdx+iiY)+= dTTbydNt(TTime);
        H(nObsNdx,TxNdx+iiZ)+= dTTbydZt(TTime);
        H(nObsNdx,TxNdx+iiH)+= dTTbydHt(TTime,dt);
        
        if(pVehTx->m_eType==CMOOSNavVehicle::POSE_AND_RATE)
        {
            H(nObsNdx,TxNdx+iiXdot)+= dTTbydEtvel(TTime,dt);
            H(nObsNdx,TxNdx+iiYdot)+= dTTbydNtvel(TTime,dt);
            H(nObsNdx,TxNdx+iiZdot)+= dTTbydZtvel(TTime,dt);
            H(nObsNdx,TxNdx+iiHdot)+= dTTbydHtvel(TTime,dt);
        }
    }


    return true;
}




double CLBLMaths::dTTbydEt( double T)
{


    return Diff1_TT_wrt_State(a, b, c,
        0.0,                                                                        /* a1 */
       -2*(Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot),                         /* b1 */
       -2*dE,                                                                       /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydNt( double T)
{


    return Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
       -2*(Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot),                         /* b1 */
       -2*dN,                                                                       /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydZt( double T)
{

    return Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
       -2*Tx_Veh_Zdot,                                                         /* b1 */
       -2*dZ,                                                                       /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydHt ( double T, double dT)
{

    return Diff1_TT_wrt_State(a,b, c,
       -2 * ((Tx_Sen_X*Tx_Veh_Xdot +                                     /* al */
              Tx_Sen_Y*Tx_Veh_Ydot) * Tx_Veh_Hdot),
       -2 * (Tx_Sen_X*(Tx_Veh_Hdot*(dE+Tx_Sen_X+dT*Tx_Veh_Xdot)+Tx_Veh_Ydot)+
             Tx_Sen_Y*(Tx_Veh_Hdot*(dN+Tx_Sen_Y+dT*Tx_Veh_Ydot)-Tx_Veh_Xdot)),
       -2 * ((dE*Tx_Sen_X+dN*Tx_Sen_Y)*Tx_Veh_Hdot*dT +
              dN*Tx_Sen_X-dE*Tx_Sen_Y),
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydEtvel( double T, double dT)
{

    
    return Diff1_TT_wrt_State(a,b, c,
        2 * (Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot),                       /* a1 */
        2 * (dE + (Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot)*dT),             /* b1 */
        2 * dE * dT,                                                                /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydNtvel( double T, double dT)
{

    
    return Diff1_TT_wrt_State(a,b, c,
        2 * (Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot),                       /* a1 */
        2 * (dN + (Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)*dT),             /* b1 */
        2 * dN * dT,                                                                /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydZtvel( double T, double dT)
{

    
    
    return Diff1_TT_wrt_State(a,b, c,
        2 * Tx_Veh_Zdot,                                                       /* a1 */
        2 * (dZ + Tx_Veh_Zdot*dT),                                             /* b1 */
        2 * dZ * dT,                                                                /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydHtvel( double T, double dT)
{

    
    
    return Diff1_TT_wrt_State(a,b, c,
        2 * (Tx_Sen_X*(Tx_Veh_Ydot+Tx_Veh_Hdot*Tx_Sen_X)-       /* a1 */
             Tx_Sen_Y*(Tx_Veh_Xdot-Tx_Veh_Hdot*Tx_Sen_Y)+ 
            (Tx_Sen_X*(Tx_Veh_Xdot-Tx_Veh_Hdot*Tx_Sen_Y)+
             Tx_Sen_Y*(Tx_Veh_Ydot+Tx_Veh_Hdot*Tx_Sen_X))*
             Tx_Veh_Hdot*(dT+T)),
        2 * (Tx_Sen_X*(dN+dT*(Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)+/* b1 */
            (Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot+
             Tx_Veh_Hdot*(dE+dT*(Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot)))*(dT+T))-
             Tx_Sen_Y*(dE+dT*(Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot)+ 
            (Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot-
             Tx_Veh_Hdot*(dN+dT*(Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)))*(dT+T))),
        2 * (Tx_Sen_X*(dN*dT+(dN+dE*dT*Tx_Veh_Hdot)*(dT+T)) -              /* c1 */
             Tx_Sen_Y*(dE*dT+(dE-dN*dT*Tx_Veh_Hdot)*(dT+T))),
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydEr( double T)
{

        double dfVal = Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
        2 * (Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot),                       /* b1 */
        2 * dE,                                                                     /* c1 */
        T);                         /* Estimated travel time                              */

        return dfVal;
}

double CLBLMaths::dTTbydNr( double T)
{

    
    return Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
        2 * (Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot),                       /* b1 */
        2 * dN,                                                                     /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydZr( double T)
{

    return Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
        2 * Tx_Veh_Zdot,                                                       /* b1 */
        2 * dZ,                                                                     /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydHr(  double T, double dT)
{



    return Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
        2 * ((Rx_Sen_X+Rx_Sen_Y*Rx_Veh_Hdot*dT)*                              /* b1 */
              (Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)-
             (Rx_Sen_Y-Rx_Sen_X*Rx_Veh_Hdot*dT)*
              (Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot)),
        2 * (Rx_Sen_X*(dN+dE*Rx_Veh_Hdot*dT)-                              /* c1 */
             Rx_Sen_Y*(dE-dN*Rx_Veh_Hdot*dT)),
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydErvel( double T, double dT)
{

    return Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
       -2 * (Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot) * dT,                  /* b1 */
       -2 * dE * dT,                                                                /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydNrvel( double T, double dT)
{

    
    return Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
       -2 * (Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)*dT,                    /* b1 */
       -2 * dN * dT,                                                                /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydZrvel( double T, double dT)
{
    
    
    return Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
       -2 * Tx_Veh_Zdot * dT,                                                  /* b1 */
       -2 * dZ * dT,                                                                /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydHrvel( double T, double dT)
{

    
    
    return Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
        2 * ((Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot)*                      /* b1 */
              (2*Rx_Sen_Y-Rx_Sen_X*Rx_Veh_Hdot*dT)-
             (Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)*
              (2*Rx_Sen_X+Rx_Sen_Y*Rx_Veh_Hdot*dT))*dT,
       -2 * (dN*(2*Rx_Sen_X+Rx_Sen_Y*Rx_Veh_Hdot*dT)-                /* c1 */
             dE*(2*Rx_Sen_Y-Rx_Sen_X*Rx_Veh_Hdot*dT))*dT,
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydxt( double T, double dT)
{

    return Diff1_TT_wrt_State(a,b, c,
        2 * (CosTx*(Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)-                /* a1 */
             SinTx*(Tx_Veh_Xdot-Tx_Veh_Hdot*Tx_Sen_Y))*Tx_Veh_Hdot,
       -2 * (CosTx*(Tx_Veh_Xdot-(dN+Tx_Sen_Y+                            /* b1 */
                (Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)*dT)*Tx_Veh_Hdot)+
             SinTx*(Tx_Veh_Ydot+(dE+Tx_Sen_X+
                (Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot)*dT)*Tx_Veh_Hdot)),
       -2 * (CosTx*(dE-dN*Tx_Veh_Hdot*dT)+SinTx*(dN+dE*Tx_Veh_Hdot*dT)),      /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydyt( double T, double dT)
{
    
    
    return Diff1_TT_wrt_State(a,b, c,
       -2 * (CosTx*(Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot)+                /* a1 */
             SinTx*(Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot))*Tx_Veh_Hdot,
        2 * (SinTx*(Tx_Veh_Xdot-(dN+Tx_Sen_Y+                            /* b1 */
                        (Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)*dT)*Tx_Veh_Hdot)-
             CosTx*(Tx_Veh_Ydot+(dE+Tx_Sen_X+
                        (Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot)*dT)*Tx_Veh_Hdot)),
       -2 * (CosTx*(dN+dE*Tx_Veh_Hdot*dT)-SinTx*(dE-dN*Tx_Veh_Hdot*dT)),
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydxr(  double T, double dT)
{

    
    return Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
        2 * (CosRx*((Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot)-               /* b1 */
                    (Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)*Rx_Veh_Hdot*dT)+
             SinRx*((Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)+
                    (Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot)*Rx_Veh_Hdot*dT)),
        2 * (CosRx*(dE-dN*Rx_Veh_Hdot*dT)+SinRx*(dN+dE*Rx_Veh_Hdot*dT)),      /* c1 */
        T);                         /* Estimated travel time                              */
}

double CLBLMaths::dTTbydyr (  double T, double dT)
{
    
    return Diff1_TT_wrt_State(a,b, c,
        0.0,                                                                        /* a1 */
        2 * (CosRx*((Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot)*Rx_Veh_Hdot*dT+
                    (Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot))+
             SinRx*((Tx_Veh_Ydot+Tx_Sen_X*Tx_Veh_Hdot)*Rx_Veh_Hdot*dT-
                    (Tx_Veh_Xdot-Tx_Sen_Y*Tx_Veh_Hdot))),
        2 * (CosRx*(dN+dE*Rx_Veh_Hdot*dT)-SinRx*(dE-dN*Rx_Veh_Hdot*dT)),      /* c1 */
        T);                         /* Estimated travel time                              */
}

void CLBLMaths::DoDebug()
{

    
}
