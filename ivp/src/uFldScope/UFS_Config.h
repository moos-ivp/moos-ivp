/****************************************************************/
/*   NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*   ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*   FILE: UFS_Config.h                                         */
/*   DATE: Nov 23rd 2011                                        */
/****************************************************************/

#ifndef UFS_CONFIG_HEADER
#define UFS_CONFIG_HEADER

#include <string>
#include "MOOSLib.h"

class UFS_Config
{
 public:
  UFS_Config(const std::string& s="");
  virtual ~UFS_Config() {};

  // Setters
  void init(const std::string&);

  // Analysis
  bool valid() const;

  // Getters
  std::string getMOOSVar() const   {return(m_var);};
  std::string getKey() const       {return(m_key);};
  std::string getField() const     {return(m_fld);};
  std::string getAlias() const     {return(m_alias);};
  std::string getColor() const     {return(m_color);};
  std::string getXColor() const    {return(m_xcolor);};
  double      getXThresh() const   {return(m_xthresh);};

 protected:
  std::string m_var;      // MOOS Variable   
  std::string m_key;      // Vehicle name key (default = "name")
  std::string m_fld;      // Field to find and report
  std::string m_alias;    // Alias for the field used in report
  std::string m_color;    // Color reported under normal situations
  std::string m_xcolor;   // Color reported when new info is late
  double      m_xthresh;  // Time after which report is considered late.
};

#endif 
