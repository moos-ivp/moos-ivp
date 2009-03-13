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
#ifndef AUVPARAMH
#define AUVPARAMH


#ifndef PI
#define PI 3.141592653589
#endif

#define YAW_PID_KP 0.5
#define YAW_PID_KD 0.2
#define YAW_PID_KI 0.00
#define YAW_PID_INTEGRAL_LIMIT 2


#define PITCH_PID_KP 10
#define PITCH_PID_KD 50
#define PITCH_PID_KI 0.1
#define PITCH_PID_INTEGRAL_LIMIT 10

#define Z_TO_PITCH_PID_KP 10
#define Z_TO_PITCH_PID_KD 50
#define Z_TO_PITCH_PID_KI 0.1
#define Z_TO_PITCH_PID_INTEGRAL_LIMIT 10

#define PITCH_MAX MOOSDeg2Rad(10)


#define ELEVATOR_MAX 30
#define ELEVATOR_MIN -30
#define RUDDER_MAX 20
#define RUDDER_MIN -20
#define THRUST_MAX 100
#define THRUST_MIN -100


#endif
