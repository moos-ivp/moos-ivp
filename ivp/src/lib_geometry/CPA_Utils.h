/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: CPA_Utils.h                                          */
/*    DATE: Mar 11, 2007 Sat afternoon at Brugger's              */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/
 
#ifndef XY_CPA_UTILS_HEADER
#define XY_CPA_UTILS_HEADER

double evalCPA(double cnx, double cny, double cnspd, double cnhdg,
	       double osx, double osy, double osspd, double oshdg,
	       double ostol);

bool   crossesBow(double cnx, double cny, double cnspd, double cnhdg,
		  double osx, double osy, double osspd, double oshdg,
		  double ostol=0);

double closingSpeed(double osx, double osy, double osspd, 
		    double oshdg, double px, double py);
		    

double closingSpeed(double osx, double osy, double osspd, double oshdg,
		    double cnx, double cny, double cnspd, double cnhdg);


void   velocityVectorSum(double hdg1, double spd1, double hdg2, 
			 double spd2, double& hdg, double& spd);

#endif














