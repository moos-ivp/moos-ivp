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


// SixDOFAUV.cpp: implementation of the CSixDOFAUV class.
//
//////////////////////////////////////////////////////////////////////
#ifdef _WIN32
    #pragma warning(disable : 4786)
#endif



#include <math.h>
#include "SixDOFAUV.h"
#include "AcousticTransceiver.h"
using namespace std;

#ifndef PI
    #define PI 3.141592653589793
#endif

Matrix CrossProduct(Matrix& v1, Matrix& v2)
{
    double a1 = v1(1,1);
    double a2 = v1(2,1);
    double a3 = v1(3,1);
    
    double b1 = v2(1,1);
    double b2 = v2(2,1);
    double b3 = v2(3,1);

    Matrix Cross(3,1);
    Cross<<(a2* b3 - a3*b2)<<
           (a3*b1 - a1*b3)<<
           (a1*b2 - a2*b1);

    return Cross;
}




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSixDOFAUV::CSixDOFAUV()
{

}

CSixDOFAUV::~CSixDOFAUV()
{

}


bool CSixDOFAUV::Initialise()
{
    //if you were to say - Oi! Whats with all these
    //out-of-the-blue numbers? I would have to concur....
    //later versions added these to a mission file..

    double Mass        = 30.0;
    m_dfWeight        = Mass*9.81;
    m_dfBuoyancy    = m_dfWeight+0.001;
    m_dfHeading         = 0;

    //y points to front of vessel
//    double Ixx  = 10.00;
    double Ixx  = 5.00;
    double Iyy  = 2.08;
    double Izz  = 8.0;
    
    m_M.ReSize(6,6);
    m_M = 0;
    m_M(1,1) = Mass;
    m_M(2,2) = Mass;
    m_M(3,3) = Mass;
    m_M(4,4) = Ixx;
    m_M(5,5) = Iyy;
    m_M(6,6) = Izz;
    m_Mi = m_M.i();



    m_Pos_e.ReSize(6,1);
    m_Pos_e = 0;
    
    m_Vel_e.ReSize(6,1);
    m_Vel_e = 0;

    m_Vel_v.ReSize(6,1);
    m_Vel_v = 0;

    //entirely made up numbers.....but they are
    //not ridiculous
    m_KDrag.ReSize(6,1);
    m_KDrag<<100<<60<<100<<200<<10<<70.0;

    m_Drag.ReSize(6,1);

    m_JacVel.ReSize(6,6);
    m_JacVel = 0;


    m_Uf.ReSize(6,1);
    m_Uf = 0;

    m_dfThrust = 0;
    m_dfRudder = 0;
    m_dfElevator =0;

    
    m_Hf.ReSize(6,1);
    m_COG.ReSize(3,1);
    m_COG = 0;

    m_COB.ReSize(3,1);
    m_COB<<0.0<<0.0<<0.05;

    m_dfDepth = 0;
    m_dfAltitude = 0;

/*
    // Set up acoustic facility
    CAcousticNode* pTheAcousticsBit = new CAcousticTransceiver;

    //pass a pointer to the global params block..
    pTheAcousticsBit->SetParams(m_pParams);

    //give it a parent
    pTheAcousticsBit->SetParent(this);

    //name the acoustic node
    pTheAcousticsBit->SetName("Tcvr1");


    m_AcousticNodes.push_back(pTheAcousticsBit);
*/

    return true;
}

bool CSixDOFAUV::FillVelJacobian()
{
    double dfPhi = m_Pos_e(4,1);    double cPhi = cos(dfPhi);         double sPhi=sin(dfPhi);
    double dfTheta = m_Pos_e(5,1);  double cTheta = cos(dfTheta);   double sTheta = sin(dfTheta);
    double dfPsi = m_Pos_e(6,1);    double cPsi=cos(dfPsi);         double sPsi = sin(dfPsi);


      m_JacVel                  << cPsi*cTheta  << -sPsi*cPhi+cPsi*sTheta*sPhi  <<    sPsi*sPhi+cPsi*sTheta*cPhi  <<0<<0<<0
                                   << sPsi*cTheta  <<  cPsi*cPhi+sPsi*sTheta*sPhi  <<    -cPsi*sPhi+sPsi*sTheta*cPhi <<0<<0<<0
                                   <<  -sTheta     <<            cTheta*sPhi         <<                cTheta*cPhi       <<0<<0<<0
                  
                                <<0<<0<<0<<     1.0         <<   sPhi*tan(dfTheta)          <<  cPhi*tan(dfTheta)   
                                   <<0<<0<<0<<     0           <<      cPhi                    <<       -sPhi          
                                   <<0<<0<<0<<     0           <<      sPhi/cTheta             <<      cPhi/cTheta;





    return true;

}

bool CSixDOFAUV::Iterate(double dfTimeNow,double dfDT)
{
        
    DoModel(dfDT);


    return true;
}


bool CSixDOFAUV::DoModel(double dfDT)
{

    //prepare velocity jacobian
    FillVelJacobian();

    Matrix J1=m_JacVel.SubMatrix(1,3,1,3);

    //////////////////////////////////////////////
    //      calculate body forces....
    //////////////////////////////////////////////
    
    Matrix B(3,1); Matrix W(3,1);

    //if we surface we have a discontinuity!
    //alternatively you could integrate over submerged volume
    //but this is simpler
   
    int nSignDepth = m_dfDepth>=0.0?1:-1;
   

    B<<0.0<<0.0<<m_dfBuoyancy*nSignDepth;
    W<<0.0<<0.0<<-m_dfWeight*nSignDepth;

    Matrix Fg = J1.i()*W;
    Matrix Fb = J1.i()*B;

    m_Hf = (Fg+Fb) & (CrossProduct(m_COG,Fg)+CrossProduct(m_COB,Fb));


    ////////////////////////////////////////
    //  Calculate thrust vector
    ////////////////////////////////////////

    // uncomment this block for stand alone testing
    
    //        m_dfThrust = 5.0;
    //        m_dfElevator = 5.0*PI/180.0;
    //        m_dfRudder = 5.0*PI/180.0;
    

    ///////////////////////////////////////////////////////////
    //note without proper added mass terms vehicle turns very
    //sharply hence scaling terms in the following force vector
    //can be extended later.
    ///////////////////////////////////////////////////////////

    /*
        Matrix Thrust(3,1);

        Thrust<<m_dfThrust*cos(m_dfElevator)*cos(m_dfRudder)<<
                m_dfThrust*cos(m_dfElevator)*sin(m_dfRudder)<<
                -m_dfThrust*sin(m_dfElevator);

        Matrix Tail(3,1); Tail<<-dfLen<<0.0<<0;

        m_Uf  = Thrust & (CrossProduct(Tail,Thrust));
    */



    //note Tq_y = 0 we assume no propellor reaction torque
    //simple thing to add though Tprop = k* vel ?

    //half length of vehicle (assumed)
    double dfLen = 1.0;

    //m_dfRudder = 0;
    //m_dfElevator = MOOSDeg2Rad(4);
    //m_dfThrust = 40;

    m_Uf   <<  -m_dfThrust*cos(m_dfElevator)*sin(m_dfRudder)<<
                3*m_dfThrust*cos(m_dfElevator)*cos(m_dfRudder)<<
                0.1*m_dfThrust*sin(m_dfElevator)<<
                -/*0.1**/dfLen*m_dfThrust*sin(m_dfElevator)<<
                0.0<< 
                -0.8*m_dfThrust*dfLen*cos(m_dfElevator)*sin(m_dfRudder);
        

    //MOOSTrace("Elevator = %f Thrust = %f Rudder = %f\n",m_dfElevator,m_dfThrust,m_dfRudder);
    ////////////////////////////////////////
    //  calculate drag vector
    ////////////////////////////////////////
    m_Drag  <<  -(m_KDrag(1,1)*m_Vel_v(1,1)*fabs(m_Vel_v(1,1)))<<
                -(m_KDrag(2,1)*m_Vel_v(2,1)*fabs(m_Vel_v(2,1)))<<
                -(m_KDrag(3,1)*m_Vel_v(3,1)*fabs(m_Vel_v(3,1)))<<
                -(m_KDrag(4,1)*m_Vel_v(4,1)*fabs(m_Vel_v(4,1)))<<
                -(m_KDrag(5,1)*m_Vel_v(5,1)*fabs(m_Vel_v(5,1)))<<
                -(m_KDrag(6,1)*m_Vel_v(6,1)*fabs(m_Vel_v(6,1)));


    ///////////////////////////////////////
    //  utterly trivial integration
    //  extend to 4th order RK when required
    ///////////////////////////////////////

    // Newton III :
    m_Acc_v = m_Mi * (m_Uf+m_Drag+m_Hf);

    // integrate (1)
    m_Vel_v += m_Acc_v*dfDT;

    //transform to body coordinates with jacobian
    m_Vel_e = m_JacVel*m_Vel_v;

    //integrate (2)
    m_Pos_e += m_Vel_e*dfDT;


    ////////////////////////////////////////
    // post integration house keeping
    ////////////////////////////////////////

    //Look after angles...
    m_Pos_e(6,1) = MOOS_ANGLE_WRAP(m_Pos_e(6,1));

    //figure out our heading -ve around z axis from x axis...
    m_dfHeading = HeadingFromYaw(m_Pos_e(6,1));

    //figure out speed..
    m_dfSpeed = sqrt(pow(m_Vel_v(1,1),2)+pow(m_Vel_v(2,1),2)+pow(m_Vel_v(3,1),2));

    ////////////////////////////////////////////////////////
    // Environmental properties...
    //  some things are a function of environment and so we need
    //  to access it to resolve some properties....
    if(m_pEnvironment!=NULL)
    {
        m_dfDepth = m_pEnvironment->GetDepth(GetZ());
        m_dfAltitude = m_pEnvironment->GetAltitude(GetX(),GetY(),GetZ());
    }

    return true;
}




double CSixDOFAUV::GetDepth()
{
  
    return m_dfDepth;
}
