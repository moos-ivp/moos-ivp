/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: MapMarkers.h                                    */
/*    DATE: Sep 25th, 2023                                  */
/************************************************************/

#ifndef MAP_MARKERS_HEADER
#define MAP_MARKERS_HEADER

#include "MOOS/libMOOS/Thirdparty/AppCasting/AppCastingMOOSApp.h"
#include <string> 
#include <map> 
#include "XYMarker.h"
#include "OpField.h"

class MapMarkers : public AppCastingMOOSApp
{
 public:
  MapMarkers() ;
  ~MapMarkers() {};

 protected: // Standard MOOSApp functions to overload  
  bool OnNewMail(MOOSMSG_LIST &NewMail);
  bool Iterate();
  bool OnConnectToServer();
  bool OnStartUp();

 protected: // Standard AppCastingMOOSApp function to overload 
  bool buildReport();

  bool addMarker(std::string);
  void postMarkers();
  
 protected:
  void registerVariables();
  bool handleConfigFileOPF(std::string);
  bool handleConfigMarkerShape(std::string);
  bool buildOpFieldFromFiles();
  
 private: // Configuration variables

  std::map<std::string, XYMarker>    m_map_markers;

  std::vector<std::string> m_opf_files;
  
  bool         m_show_markers;
  bool         m_show_mlabels;
  unsigned int m_marker_size;    // default size

  std::string  m_map_marker_key; // A MOOS var for toggling view
  std::string  m_marker_color;
  std::string  m_marker_lcolor;
  std::string  m_marker_ecolor;
  std::string  m_marker_shape;
  
  OpField m_opfield;
  
 private: // State variables

  double m_post_markers_utc;
  bool   m_refresh_needed;
};

#endif 
