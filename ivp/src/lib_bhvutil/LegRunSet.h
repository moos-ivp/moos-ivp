/************************************************************/
/*    NAME: Mike Benjamin                                   */
/*    ORGN: MIT, Cambridge MA                               */
/*    FILE: LegRunSet.h                                     */
/*    DATE: June 11th, 2023                                 */
/************************************************************/

#ifndef LEG_RUN_SET_HEADER
#define LEG_RUN_SET_HEADER

#include <string>
#include <map>
#include "LegRun.h"

class LegRunSet
{
public:
   LegRunSet();
  ~LegRunSet() {};

public: // Leg Setters
  bool setLegParams(std::string);

  bool handleLegRunFile(std::string, double, std::string&);
  
  bool addLegRun(std::string name, double leglen,
		 double turn1_rad, double turn2rad,
		 double turn1_bias, double turn2_bias,
		 double turn1_ext, double turn2_ext,
		 std::string turn1_dir,
		 std::string turn2_dir);
  
  
public: // Getters
  
  std::string getLegRunFile() const {return(m_legrun_file);}
  
public: // Analyzers
  bool valid() const;
  
  unsigned int size() const {return(m_map_legruns.size());}
  
private: // Config vars
  std::map<std::string, LegRun> m_map_legruns;
  
  std::string m_legrun_file;
  
private: // State var
  
};

#endif 
