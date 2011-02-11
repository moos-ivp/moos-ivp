/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BehaviorReport.cpp                                   */
/*    DATE: Apr 16th 2008                                        */
/*****************************************************************/

#include "BehaviorReport.h"

using namespace std;

//-----------------------------------------------------------
// Procedure: reset()

void BehaviorReport::reset()
{
  m_good_updates  = 0;
  m_bad_updates   = 0;
  m_update_age    = 0;
  m_duration      = 0;
  m_start_time    = 0;
  m_pieces        = 0;
  m_priority      = 0;
  m_duration_left = 0;
  m_create_time   = 0;
  m_avg_error     = 0;
  m_wst_error     = 0;
  m_samples       = 0;
  m_descriptor    = "";
  m_update_var    = "";
  m_state         = "";
  m_unif_piece    = "";
  m_messages.clear();
}
