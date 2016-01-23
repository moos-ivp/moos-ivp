/*****************************************************************/
/*    NAME: Michael Benjamin, Henrik Schmidt, and John Leonard   */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: Populator_KCache.cpp                                 */
/*    DATE: April 23rd 2015                                      */
/*****************************************************************/

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "MBUtils.h"
#include "FileBuffer.h"
#include "PopulatorAvkTable.h"
#include "Populator_KCache.h"

using namespace std;

//-------------------------------------------------------------
// Procedure: populate

bool Populator_KCache::populate(string avk_file)
{
  cout << "BEGIN Populator_KCache---------" << endl;
  PopulatorAvkTable populator;
  bool ok_file = populator.setAvkFile(avk_file);
  if(!ok_file) {
    cout << "Problem with file: [" << avk_file << "]. Exiting." << endl;
    return(false);
  }
  populator.populate();

  AVK_Table avk_table = populator.getAvkTable();
  //avk_table.print();
  //return(false);

  bool ok_interp = avk_table.interpolateAll();
  if(!ok_interp) {
    cout << "Problem with interpolate" << endl;
    //return(false);
  }
  
  bool ok_pcwise = avk_table.pcwiseBuildAll();
  if(!ok_pcwise) {
    cout << "Problem with pcwise build" << endl;
    return(false);
  }

  bool ok_expand = avk_table.reconfigure(1.0, 0.1, 2.0);
  if(!ok_expand) {
    cout << "Problem with expanding." << endl;
    return(false);
  }


  bool ok_interp2 = avk_table.interpolateAll();
  if(!ok_interp2) {
    cout << "Problem with interpolate" << endl;
    //return(false);
  }
  
  bool ok_pcwise2 = avk_table.pcwiseBuildAll();
  if(!ok_pcwise2) {
    cout << "Problem with pcwise build" << endl;
    return(false);
  }

  cout << "END Populator_KCache---------" << endl;  

  m_avk_table = avk_table;
  return(true);
}


//-------------------------------------------------------------
// Procedure: getKCache

KCache Populator_KCache::getKCache() const
{
  KCache kcache;
  kcache.configure(1.0, 0.1, 2.0);

  cout << "Getting KCache" << endl;

  vector<Seglr>  seglrs   = m_avk_table.getAllSeglrs();
  vector<double> dh_vals  = m_avk_table.getAllDH();
  vector<double> spd_vals = m_avk_table.getAllSpd();


  cout << "seglrs size: " << seglrs.size() << endl;
  cout << "dh_vals size: " << dh_vals.size() << endl;
  cout << "spd_vals size: " << spd_vals.size() << endl;


  
  unsigned int vsize = seglrs.size();
  if((vsize != dh_vals.size()) || (vsize != spd_vals.size()))
    return(kcache);

  cout << "Setting Cells: " << vsize << endl;

  bool ok = true;
  for(unsigned int i=0; i<vsize; i++) {
    bool this_ok = kcache.setCellSeglrOrig(spd_vals[i], dh_vals[i], seglrs[i]);
    //cout << "this_ok: " << boolToString(this_ok) << endl;
    ok = ok && this_ok;
  }

  cout << "Setting of cells ok: " << boolToString(ok) << endl;

  return(kcache);
}
