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
#ifndef MOOSNavLibDefsh
#define MOOSNavLibDefsh

enum GlobalStateIndexes
{
    TIDE_STATE_INDEX=1,
    HEADING_BIAS_STATE_INDEX = 2,
    
    //don't add beyond this
    FIRST_NON_GLOBAL_STATE,
};

// these define the maximum mobility of vehicles
#define FULL_SCALE_Q_ACC_XY        0.5            //ms-2
#define FULL_SCALE_Q_ACC_Z        0.2            //ms-2
#define FULL_SCALE_HEADING_ACC    0.03        //rad s^-2 (2 deg/s/s)
#define FULL_SCALE_Q_VEL        2.0            //ms-1
#define FULL_SCALE_HEADING_VEL    0.3        //rad s^-1 (0.6 deg/s)
#define FULL_SCALE_SURFACE_STD  0.5            //m


#endif
