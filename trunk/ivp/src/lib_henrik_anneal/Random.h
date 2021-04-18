/*****************************************************************/
/*    NAME: Henrik Schmidt                                       */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE:                                                      */
/*    DATE:                                                      */
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
#ifndef __CRandom_h__
#define __CRandom_h__

#include <math.h>
#include <vector>
#include <complex>

#define IM1 2147483563
#define IM2 2147483399
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 64
#define EPS 1.2e-15

class CRandom 
{
 public:

  CRandom();
  virtual ~CRandom();

  double ran2(int & idum);
  double gauss_dev(double mu, double sigma);
  double gauss_real(double mu, double sigma);
  std::complex<double> gauss_cmplx(double mu, double sigma);
  double rand();

 protected:
  
  std::vector<int> iv;
  int iy;
  int idum2;
  double AM;
  int IMM1;
  int NDIV;
  double RNMX;

  double V1;
  double V2;
  double S;
  int phase;
  int iseed;
};

#endif /* __Random_h__ */




