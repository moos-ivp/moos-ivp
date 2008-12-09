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
//   This file is part of a  MOOS Core Component. 
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
// MOOSTerrain.cpp: implementation of the CMOOSTerrain class.
//
//////////////////////////////////////////////////////////////////////


#include "MOOSTerrain.h"

#include <fstream>
#include <iostream>
using namespace std;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMOOSTerrain::CMOOSTerrain()
{
    m_dfMinZ=1e10;
    m_dfMaxZ=-1e10;

}

CMOOSTerrain::~CMOOSTerrain()
{

}

bool CMOOSTerrain::Load(const char *sFileName)
{
    ifstream In;

    In.open(sFileName);

    if(In.is_open())
    {
        double dfRows;
        double dfCols;


        In>>dfRows;
        In>>dfCols;
//        m_Data.ReSize(dfRows,dfCols);


    
        m_Data.resize((int)dfRows+1);

        STLMATRIX::iterator p;

        for(p=m_Data.begin();p!=m_Data.end();p++)
        {
            p->resize((int)(dfCols+1));
        }

    
        
        m_X.resize((int)dfCols+1);
        m_Y.resize((int)dfRows+1);

        double dfTmp;
        int i,j;
        for(i =1;i<=dfCols;i++)
        {
            In>>dfTmp;
            m_X[i]=dfTmp;
        }
        for(i =1;i<=dfRows;i++)
        {
            In>>dfTmp;
            m_Y[i]=dfTmp;
        }
        for(i =1;i<=dfRows;i++)
        {
            for(j =1;j<=dfCols;j++)
            {
                In>>dfTmp;

                m_dfMinZ = dfTmp<m_dfMinZ ? dfTmp : m_dfMinZ;
                m_dfMaxZ = dfTmp>m_dfMaxZ ? dfTmp : m_dfMaxZ;

                m_Data[i][j]=dfTmp;
            }
        }
    

        return true;
    }
    else
    {
        return false;
    }
}

double CMOOSTerrain::GetAltitude(double dfX, double dfY, double dfZ)
{
    double dfAltitude = -1;

    //find x and y 
    int n = m_X.size()-1;
    int I=-1;
    double dfAlphaI = 0;
    for(int i=2;i<=n;i++)
    {
        if(m_X[i-1]<=dfX && m_X[i]>=dfX)
        {
            I = i;
            if(m_X[i-1]!=m_X[i])
            {
                dfAlphaI = (dfX-m_X[i-1])/(m_X[i]-m_X[i-1]);

            }
            break;
        }
    }


    n = m_Y.size()-1;
    double dfAlphaJ = 0;
    int J=-1;
    for(int j=2;j<=n;j++)
    {
        if(m_Y[j-1]<=dfY && m_Y[j]>=dfY)
        {
            J = j;
            if(m_Y[j-1]!=m_Y[j])
            {
                dfAlphaJ = (dfY-m_Y[j-1])/(m_Y[j]-m_Y[j-1]);

            }
            break;
        }
    }

    if(J!=-1 && I!=-1)
    {
        double Z0 = m_Data[I-1][J-1];
        double Z1 = m_Data[I][J-1];
        double Z2 = m_Data[I][J];
        double Z3 = m_Data[I-1][J];


       dfAltitude =  dfZ-   (   (1-dfAlphaI)*(1-dfAlphaJ)*Z0 +
                            (1-dfAlphaI)*dfAlphaJ*Z3 +
                            dfAlphaI*(1-dfAlphaJ)*Z1 +
                            dfAlphaI*dfAlphaJ*Z2);



    }
    
    return dfAltitude;
}
