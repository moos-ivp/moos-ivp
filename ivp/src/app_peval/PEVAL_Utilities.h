/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PEVAL_Utilities.h                                    */
/*    DATE: March 5th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#ifndef PEVAL_UTILITIES_HEADER
#define PEVAL_UTILITIES_HEADER

#include <string>
#include <vector>
#include "VReport.h"

void printVReport(VReport);
void printSlotReport(SlotReport);

void latexVReportToFile(VReport, std::string);

std::vector<std::string>  latexVReport(VReport);
std::vector<std::string>  latexSlotReport(SlotReport);

#endif




