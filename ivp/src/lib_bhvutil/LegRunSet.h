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
  
public: // Getters
  
  std::string getLegRunFile() const {return(m_legrun_file);}

  LegRun getLegRun(std::string id) const;

  std::vector<std::string> getLegIDs() const;

  std::vector<LegRun> getLegRuns() const;
  
public: // Analyzers

  unsigned int size() const {return(m_map_legruns.size());}
  
private: 
  std::map<std::string, LegRun> m_map_legruns;
  
  std::string m_legrun_file;
};

#endif 
