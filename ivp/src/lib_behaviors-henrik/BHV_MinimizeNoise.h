/***********************************************************/
/*    NAME: Henrik Schmidt                                  */
/*    FILE: BHV_MinimizeNoise.h                    */
/***********************************************************/
 
#ifndef BHV_MINIMIZENOISE_HEADER
#define BHV_MINIMIZENOISE_HEADER

#include <string>

#include "IvPBehavior.h"
#include "LinearExtrapolator.h"

using namespace std;

class IvPDomain;

class BHV_MinimizeNoise : public IvPBehavior {
public:
  BHV_MinimizeNoise(IvPDomain);
  ~BHV_MinimizeNoise() {};
  
  IvPFunction*  onRunState();
  bool setParam(std::string, std::string);

protected:
  
  void updateContactList();
  bool updateInfoIn();
  double getRelevance();
  int parseNoise(std::string);

  double sigmapos,sigmatheta,ty,tx,heading,speed,time; 
  int state,node;

 private:
  std::string m_contact_name; // Name for them in InfoBuffer
  std::vector<std::string> m_contact_list;

  double  m_osx; // ownship x-position
  double  m_osy; // ownship y-position
  double  m_osh; // ownship heading
  double  m_osv; // ownship velocity

  double  m_cnx; // contact x-position
  double  m_cny; // contact y-position
  double  m_cnh; // contact heading
  double  m_cnv; // contact velocity
  double  m_cnutc; // UTC time of last contact report

  bool    m_extrapolate;

  LinearExtrapolator m_extrapolator;

  std::vector<double> m_bearing;
  std::vector<double> m_noise;

  int num_bins;


};
#endif

