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
// MOOSTerrain.h: interface for the CMOOSTerrain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MOOSTERRAIN_H__E6A556D5_E919_4B46_9EDF_506259A8A7EF__INCLUDED_)
#define AFX_MOOSTERRAIN_H__E6A556D5_E919_4B46_9EDF_506259A8A7EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>
//using namespace std;

typedef std::vector< std::vector<double> > STLMATRIX;
 
class CMOOSTerrain  
{
public:
    double GetAltitude(double dfX,double dfY, double dfZ);
    CMOOSTerrain();
    virtual ~CMOOSTerrain();
    bool Load(const char *sFileName);
       STLMATRIX m_Data;

    
    std::vector<double> m_X;
    std::vector<double> m_Y;


protected:
    double m_dfMinZ;
       double m_dfMaxZ;

};

#endif // !defined(AFX_MOOSTERRAIN_H__E6A556D5_E919_4B46_9EDF_506259A8A7EF__INCLUDED_)
