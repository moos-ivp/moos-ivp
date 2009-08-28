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

#include <MOOSGenLib/MOOSGenLibGlobalHelper.h>
#include "MOOSNavLibGlobalHelper.h"
#include <sstream>
#include <iomanip>
#include <stdio.h>

using namespace std;

void   MOOSMatrixTrace(const Matrix & Mat,const string & sStr)
{
    int nRows = Mat.Nrows();
    int nCols = Mat.Ncols();

    ostringstream os;

    os<<"Matrix "<<sStr.c_str()<<"["<<nRows<<" x "<<nCols<<"]"<<endl;

    int nWidth = 10;
    os.setf(ios::left);

    for(int nRow = 1; nRow<=Mat.Nrows();nRow++)
    {
        for(int nCol = 1; nCol<=Mat.Ncols();nCol++)
        {
            os<<setw(nWidth)<<setprecision(3);
            os<<Mat(nRow,nCol)<<" ";
        }
        os<<endl;
    }
    
    os<<endl<<ends;
    string sTxt = os.str();

//    os.rdbuf()->freeze(0);

    MOOSTrace(sTxt);
}




                 
double MOOS_WRAP_ANGLE(double d, bool Radians /* true */)
{
    double FullScale;
    if (Radians)
    {
        FullScale=2*PI;
    }
    else
    {
        FullScale=360.0;
    }

    d = fmod(d,FullScale);

    if( d >= (FullScale/2))
    {
      d-=FullScale;
    }
    else if (d<=-(FullScale/2))
    {
      d+=FullScale;
    }

    return d;
}

bool MOOSGetJacobian(Matrix & J,const Matrix & XAbout,
                 int nOut,
                 int nIn,
                 bool (*pfn)(Matrix & XOut,Matrix & XIn,void *pParam),
                 void * pParam)
{
    
    if(J.Nrows()!=nOut || J.Ncols()!=nIn)
    {
        J.ReSize(nOut,nIn);
    }
    
    if(XAbout.Nrows()!=nIn)
    {
        printf("cannot linearise about XAbout (wrong size)\n");
        return false;
    }
            
    Matrix XOutNominal(nOut,1);
    
    Matrix XPerturbed = XAbout;

    //calculate the nominal projected
    //vector XPreturbed hasn't yet been peturbed!
    if((*pfn)(XOutNominal,XPerturbed,pParam))
    {

        double dfPerturbation=1e-7;
    
        Matrix XOutPeturbed(nOut,1);
        
        for(int nVar = 1; nVar<= nIn;nVar++)
        {
            //perturb the set point
            XPerturbed(nVar,1)+=dfPerturbation;
            
            //project with perturbation
            if((*pfn)(XOutPeturbed,XPerturbed,pParam))
            {
                J.SubMatrix(1,nOut,nVar,nVar) = (XOutPeturbed-XOutNominal)/dfPerturbation;
            }

            XPerturbed(nVar,1)-=dfPerturbation;        
        }
    }
        
    return true;
}


/** this performs an n degree of freedom Chi squared test in the vector Innovation given its
covariance matrix Cov. The degrees of freedom is ROWS(Innovation)*/
bool MOOSChiSquaredTest(Matrix &Innovation, Matrix &Cov, bool bAlreadyInverted)
{
    Matrix mEpsilon;
    if(!bAlreadyInverted)
    {
        mEpsilon = (Innovation.t() * Cov.i()* Innovation);
    }
    else
    {
        mEpsilon = (Innovation.t() * Cov* Innovation);
    }

    double dfEpsilon = mEpsilon(1,1);


    double ChiSquared[] = {    12.12,    //1
                            15.20,    //2
                            17.73,    //3
                            20.00,    //4
                            22.11,    //5
                            24.10,    //6
                            26.02,    //7
                            27.87,    //8
                            29.76,    //9
                            31.42,    //10
                            33.14,    //11
                            34.82,    //12
                            36.48,    //13
                            38.11,    //14
                            39.72,    //15
                            41.31,    //16
                            42.88,    //17
                            44.43,    //18
                            45.97,    //19
                            47.50,    //20
                            };

    int MaxDOF =sizeof(ChiSquared)/sizeof(double);

    int DOF = Innovation.Nrows();

    if(DOF>MaxDOF)
    {
        MOOSTrace("MOOS data validation...Chi Squared  data for %d DOF  unavailable!\n",DOF);
        return false;
    }
    else
    {

//        MOOSTrace("df%d v'Sv = %e, Chi = %e \n", DOF, dfEpsilon,ChiSquared[DOF-1]);

        if(dfEpsilon<=ChiSquared[DOF-1])
        {
            return     true;
        }
        else
        {
//            MOOSTrace("df%d v'Sv = %e, Chi = %e \n", DOF, dfEpsilon,ChiSquared[DOF-1]);
            return false;
        }
    }                                        
    return false;
}

