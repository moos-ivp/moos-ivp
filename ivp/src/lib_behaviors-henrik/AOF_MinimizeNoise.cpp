/***********************************************************/
/*    NAME: Henrik Schmidt                                  */
/*    FILE: AOF_MinimizeNoise.cpp                    */
/*    BORN: 8. May 2008                                   */
/***********************************************************/
#include <iostream>
#include <cstdlib>
#include <math.h> 
#include <assert.h>
#include "AOF_MinimizeNoise.h"
#include "AngleUtils.h"
#include "IvPDomain.h"

using namespace std;

/******************************************************************************
 *                                                                            *
 *  Method:AOF_MinimizeNoise(IvPDomain g_domain): AOF(g_domain)       *
 *                                                                            *
 *  Description: Class constructor 
 *                                                                            * 
 ******************************************************************************/

AOF_MinimizeNoise::AOF_MinimizeNoise(IvPDomain g_domain): AOF(g_domain)
{
  crs_ix = g_domain.getIndex("course");
  
  assert(crs_ix != -1);
  
}

/******************************************************************************
 *                                                                            *
 *  Method: setParam(const string& param, double param_val)                   *
 *                                                                            *
 *  Description: This method is called by BHV_MinimizeNoise and sets  *
 *  the needed parameters.                                                    * 
 *                                                                            * 
 ******************************************************************************/

bool AOF_MinimizeNoise::setParam(const string& param, double param_val)
{
  if(param == "osX") {
    osx = param_val;
    return(true);
  }
  else if(param == "osY") {
    osy = param_val;
    return(true);
  }  
   else if(param == "target_x") {
    targx = param_val;
    return(true);
  }
  else if(param == "target_y") {
    targy = param_val;
    return(true);
  }
  else
    return(false);  
}

void AOF_MinimizeNoise::resetNoise()
{
  m_noise.clear();
  m_bearing.clear();
}

/******************************************************************************
 *                                                                            *
 *  Method: setsNoiseBin(double bearing, double noise)                           *                                                                            *
 *  Description: sets and sorts the noise directionality                      *
 *  Must be called for all bearings                                           *  *                                                                            *  ******************************************************************************/

int AOF_MinimizeNoise::setNoiseBin(double bearing, double noise)
{
  int i;
  for (i=0;i<m_bearing.size();i++)
    {
      if (bearing == m_bearing[i])
	{
	  m_bearing[i] = bearing;
	  m_noise[i] = noise;
	  return(m_bearing.size());
	}
      else if (bearing < m_bearing[i])
	{
	  int last = m_bearing.size()-1;
	  m_bearing.push_back(m_bearing[last]);
	  m_noise.push_back(m_noise[last]);
	  for (int j=last; j> i; j--)
	    {
	      m_bearing[j] = m_bearing[j-1];
	      m_noise[j] = m_noise[j-1];
	    } 
	  m_bearing[i] = bearing;
	  m_noise[i] = noise;
	  return(m_bearing.size());
	}
    }
  m_bearing.push_back(bearing);
  m_noise.push_back(noise);
  return(m_bearing.size());
}

/******************************************************************************
 *                                                                            *
 *  Method: AOF_MinimizeNoise::evalBox(const IvPBox *b) const         *
 *                                                                            *
 *  Description: This method is called to evaluate a proposed course and      *
 *  returns a metric which corresponds to the utility of the proposed course. * 
 *                                                                            * 
 ******************************************************************************/ 

double AOF_MinimizeNoise::evalBox(const IvPBox *b) const
{
 
  double evalCRS;
  // Get course to be evaluated
  m_domain.getVal(crs_ix,b->pt(crs_ix,0),evalCRS);

  double target_bearing = radToHeading(atan2(targy-osy,targx-osx));

  double diff_angle = angle180(evalCRS-target_bearing);
  double bearing = angle360(target_bearing - diff_angle);

  for (int i=0;i<m_bearing.size();i++)
    {
      if (bearing == m_bearing[i])
	{
	  double value = -m_noise[i];
	  return(value);
	}
      else if (i < m_bearing.size()-1)
	{
	  if (bearing < m_bearing[i+1])
	    {
	      double value = - (m_noise[i] * (m_bearing[i+1]-bearing) 
				+ m_noise[i+1]*(bearing-m_bearing[i]) )
		/(m_bearing[i+1]-m_bearing[i]);
	      return(value);
	    }
	}
      else 
	{
	  double value = - (m_noise[i] * (360.0 + m_bearing[0] - bearing) 
			  + m_noise[0]*(bearing-m_bearing[i]) )
	    /(360.0+m_bearing[0]-m_bearing[i]);
	  return(value);
	}
    }
}


