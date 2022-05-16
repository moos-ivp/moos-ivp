/*********************************************************/
/*    NAME: Michael Benjamin                             */
/*    ORGN: Dept of Mechanical Eng, MIT Cambridge MA     */
/*    FILE: PolarPlot.h                                  */
/*    DATE: July 16th 2021                               */
/*********************************************************/

#ifndef POLAR_PLOT_HEADER
#define POLAR_PLOT_HEADER

#include <map>
#include <string>
#include <vector>

class PolarPlot {
public:
  PolarPlot();
  ~PolarPlot() {}

  bool addSetPoint(double hdg, double val);
  bool addSetPoints(std::string);
  
  std::map<double, double> getSetPoints() {return(m_setpts);}

  std::vector<double> getSetHdgs() const;
  std::vector<double> getSetVals() const;

  std::vector<double> getSetHdgsRotated() const;

  std::vector<double> getAllHdgs();
  std::vector<double> getAllVals();

  void setWindAngle(double angle);
  void modWindAngle(double degrees);
  
  void grabPoint(double hdg, double val) {};
  void modPoint(double hdg, double val);
  void removePoint(double hdg, double val);
  
  void   interpLinear();

  double getPolarPct(double);

  double getWindAngle() {return(m_wind_angle);}
  
  std::string getSpec() const;   

  bool set() const {return(m_setpts.size() > 0);}
  
  unsigned int size() const {return(m_setpts.size());}
  
protected:
  double interpLinear(double);           
  bool   getNeighbors(double hdg, double& hmin, double& vmin,
		      double& hmax, double& vmax);  
  double getClosestHdg(double hdg, double val);
  
protected:

  std::map<double, double> m_setpts;

  std::map<int, double> m_allpts;

  bool   m_interpolated;
  
  double m_max_value;

  double m_wind_angle;
};

PolarPlot stringToPolarPlot(std::string);


#endif
