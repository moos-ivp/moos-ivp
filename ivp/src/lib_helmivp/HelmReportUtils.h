/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: HelmReportUtils.h                                    */
/*    DATE: Oct0511                                              */
/*****************************************************************/

#ifndef HELM_REPORT_UTILS_HEADER
#define HELM_REPORT_UTILS_HEADER

#include <string>
#include "HelmReport.h"

HelmReport string2HelmReport(const std::string&);
HelmReport string2HelmReport(const std::string&, const HelmReport&);

#endif

