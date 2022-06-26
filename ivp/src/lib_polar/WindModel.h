/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: WindModel.h                                          */
/*    DATE: Jan 24th 2022                                        */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s), or those designated by the author.   */
/*****************************************************************/

#ifndef WIND_MODEL_HEADER
#define WIND_MODEL_HEADER

#include <string>
#include "XYArrow.h"
#include "PolarPlot.h"

class WindModel
{
public:
  WindModel();
  ~WindModel() {}

 public: // Setters
  bool   setConditions(std::string);

  bool   setWindDir(double);  // Set
  bool   setWindSpd(double);  // and Drift

 public: // Getters
  double getWindDir() const {return(m_wind_dir);}
  double getWindSpd() const {return(m_wind_spd);}

  bool   set() const {return(m_set);}
  
 public:
  double getWindDir(double x, double y) const;
  double getWindSpd(double x, double y) const;

  double getMaxSpeed(PolarPlot, double hdg) const;
  
 public: // Visuals
  XYArrow getArrow() const {return(m_arrow);}

  std::string getArrowSpec() const;
  std::string getArrowSpec(double x, double y) const;
  std::string getArrowSpec(std::string params) const;

  std::string getModelSpec() const;

protected:
  bool   m_set;

  double m_wind_dir;
  double m_wind_spd;
  
  XYArrow m_arrow;
};

#endif 

