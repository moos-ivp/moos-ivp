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
#include "Random.h"

using namespace std;
CRandom::CRandom()
{
  phase = 0;
  iseed = -123456;
}

CRandom::~CRandom()
{
}


double CRandom::ran2(int & idum)
{
  int k,j;
  if (idum <= 0)
    {
      // Initialize first time around
      AM=1./IM1;
      IMM1=IM1-1;
      NDIV=1+IMM1/NTAB;
      RNMX=1.-EPS;
      
      iv.clear();
      for (int i=0; i<NTAB; i++)
	iv.push_back(0);
      idum2 = 123456789;

      idum=max(-idum,1);
      idum2=idum;
      for (j=NTAB+8;j>0;j--)
	{
	  k=idum/IQ1;
          idum=IA1*(idum-k*IQ1)-k*IR1;
          if (idum < 0) 
	    idum=idum+IM1;
          if (j <= NTAB) 
	    iv[j-1]=idum;
	}
      iy=iv[0];
    }

  k=idum/IQ1;
  idum=IA1*(idum-k*IQ1)-k*IR1;
  if (idum < 0) 
    idum=idum+IM1;
  k=idum2/IQ2;
  idum2=IA2*(idum2-k*IQ2)-k*IR2;
  if (idum2 < 0) 
    idum2=idum2+IM2;
  
  j=1+iy/NDIV;
  iy=iv[j-1]-idum2;
  iv[j-1]=idum;
  if(iy < 1)
    iy=iy+IMM1;
  return(min(AM*iy,RNMX));
}

double CRandom::gauss_dev(double mu, double sigma)
{
  //calculate Gaussian random deviates of complex variables.
  // Consegutive calls yield real and imaginary parts.
  //mu = mean of distribution
  //sigma= standard deviation
  //  static double V1,V2,S;
  //  static int phase = 0;
  double X;

  if (phase ==0){
    do{
      double U1 = ran2(iseed);
      double U2 = ran2(iseed);
      
      V1 = 2*U1-1;
      V2 = 2*U2-1;
      S = V1*V1 + V2*V2;
    } while(S >=1 || S == 0);

    X = mu + (V1*sqrt(-2*log(S)/S)) * sqrt(sigma);
  }
  else
    X = mu + (V2*sqrt(-2*log(S)/S)) * sqrt(sigma);

  phase = 1 - phase;

  return X;

}

double CRandom::gauss_real(double mu, double sigma)
{
  //calculate Gaussian random deviates of a real variable.
  //mu = mean of distribution
  //sigma= standard deviation
  //  static double V1,V2,S;
  //  static int phase = 0;
  double X;

  double U = ran2(iseed);
  
  double V = 2*U-1;
  double S = V*V;
  X = mu + (V*sqrt(-2*log(S)/S)) * sqrt(sigma);
  
  return X;
  
}

complex<double> CRandom::gauss_cmplx(double mu, double sigma)
{
  //calculate Gaussian random deviates of complex variables.
  // Consegutive calls yield real and imaginary parts.
  //mu = mean of distribution
  //sigma= standard deviation
  //  static double V1,V2,S;
  //  static int phase = 0;
  double Re, Im;


  do
    {
      double U1 = ran2(iseed);
      double U2 = ran2(iseed);
      
      V1 = 2*U1-1;
      V2 = 2*U2-1;
      S = V1*V1 + V2*V2;
    } while(S >=1 || S == 0);

    Re = mu + (V1*sqrt(-2*log(S)/S)) * sqrt(sigma);

    Im = mu + (V2*sqrt(-2*log(S)/S)) * sqrt(sigma);

    return(complex<double>(Re,Im));

}

double CRandom::rand()
{
  return(ran2(iseed));
}






