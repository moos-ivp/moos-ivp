/****************************************************************/
/*    NAME: Mike Benjamin                                       */
/*    FILE: IC_Viewer.h                                         */
/*    DATE: Apr 19th 2011                                       */
/****************************************************************/

#ifndef IVP_CHECK_VIEWER_HEADER
#define IVP_CHECK_VIEWER_HEADER

#include "Common_IPFViewer.h"
#include "IvPChecker.h"
#include "FColorMap.h"
#include "IPF_BundleSeries.h"

class IC_Viewer : public Common_IPFViewer
{
 public:
  IC_Viewer(int x, int y, int w, int h, const char *l=0);
  ~IC_Viewer() {};

  void setIvPChecker(IvPChecker v);
  
  void checkSolutions();

  // Pure virtuals that need to be defined
  void  draw();
  int   handle(int event)
  {return Common_IPFViewer::handle(event);};

  void  setIPF_BundleSeries(IPF_BundleSeries bundle_series);
  
  void  incCurrFunction();
  void  decCurrFunction();
  
  void  setCollective(const std::string&);
  
  std::string  getTotalFunctions();
  std::string  getCurrDescriptor();
  std::string  getCurrPieces();
  std::string  getCurrPriority();
  
 public:
  void  resetQuadSet();
  
 private:
  FColorMap     m_color_map;
  unsigned int  m_curr_iter;
  std::string   m_curr_source;
  bool          m_collective;
  IvPChecker    m_ivp_checker;
  
  IPF_BundleSeries m_bundle_series;
};

#endif 
