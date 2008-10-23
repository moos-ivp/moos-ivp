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
#ifndef LBLMathsh
#define LBLMathsh

class CMOOSNavSensor;
class CLBLMaths
{
    
public:
    
    
    double            dTTbydEt( double T);                        
    double            dTTbydNt( double T);                                        
    double            dTTbydZt( double T);                                        
    double            dTTbydHt ( double T, double dT);                                        
    double            dTTbydEtvel( double T, double dT);                                        
    double            dTTbydNtvel( double T, double dT);                                        
    double            dTTbydZtvel( double T, double dT);                                        
    double            dTTbydHtvel( double T, double dT);                                        
    double            dTTbydEr( double T);                                        
    double            dTTbydNr( double T);                                        
    double            dTTbydZr( double T);                                        
    double            dTTbydHr( double T, double dT);                                        
    double            dTTbydErvel( double T, double dT);                                        
    double            dTTbydNrvel( double T, double dT);                                        
    double            dTTbydZrvel( double T, double dT);                                        
    double            dTTbydHrvel(double T, double dT);                                        
    double            dTTbydxt( double T, double dT);                                        
    double            dTTbydyt( double T, double dT);            
    double            dTTbydxr( double T, double dT);
    double            dTTbydyr ( double T, double dT);
    
    bool    SetUpLBLData(   double dfTimeAgo,
                            CMOOSNavSensor* pTxSensor,
                            CMOOSNavSensor* pRxSensor,
                            Matrix * pXEvaluate,
                            double dfSV);

    bool    CalculateTwoWayTOF(    CMOOSNavSensor* pInterrogatorSensor,
                                CMOOSNavSensor* pResponderSensor,
                                Matrix * pXEvaluate,
                                double dfSV,
                                double & dfTotalTOF
                                );


    bool    CalculateLegTravelTime(    double dfTimeAgo,
                                CMOOSNavSensor * pTxSensor,
                                CMOOSNavSensor * pRxSensor,
                                Matrix * pXEvaluate,
                                double dfSV,
                                double & dfTravelTimeResult);


    bool    CalculateTwoWayTOFJacobians(CMOOSNavSensor *pInterrogatorSensor,
                                          CMOOSNavSensor *pResponderSensor,
                                          Matrix * pXEvaluate,
                                          double dfSV,
                                          Matrix & jH
                                          );

    bool    AddJacobianLegComponent(Matrix & H,
                                        CMOOSNavSensor* pTxSensor,
                                        CMOOSNavSensor* pRxSensor,
                                         double TTime,
                                         double dt);

    
    double m_dfSV;
    double Rx_Veh_X,Rx_Veh_Y,Rx_Veh_Z,Rx_Veh_H;
    double Rx_Veh_Xdot,Rx_Veh_Ydot,Rx_Veh_Zdot,Rx_Veh_Hdot;
    double Tx_Veh_X,Tx_Veh_Y,Tx_Veh_Z,Tx_Veh_H;
    double Tx_Veh_Xdot,Tx_Veh_Ydot,Tx_Veh_Zdot,Tx_Veh_Hdot;
    
    double Rx_Sen_X,Rx_Sen_Y,Rx_Sen_Z,Rx_Sen_H;
    double Rx_Sen_Xdot,Rx_Sen_Ydot,Rx_Sen_Zdot,Rx_Sen_Hdot;
    
    double Tx_Sen_X,Tx_Sen_Y,Tx_Sen_Z,Tx_Sen_H;
    double Tx_Sen_Xdot,Tx_Sen_Ydot,Tx_Sen_Zdot,Tx_Sen_Hdot;
    
    double a,b,c;
    
    double dE,dN,dZ;
    
    double CosTx,SinTx,CosRx,SinRx;

    
        
    
protected:
    void DoDebug();
};
#endif
