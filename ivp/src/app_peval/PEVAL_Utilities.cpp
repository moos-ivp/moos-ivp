/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: PEVAL_Utilities.cpp                                  */
/*    DATE: March 5th, 2006                                      */
/*                                                               */
/* This is unreleased BETA code. No permission is granted or     */
/* implied to use, copy, modify, and distribute this software    */
/* except by the author(s).                                      */
/*****************************************************************/

#include <iostream>
#include "MBUtils.h"
#include "PEVAL_Utilities.h"

using namespace std;

//----------------------------------------------------------------
// Procedure: printVReport()

void printVReport(VReport vreport)
{
  int vehicles = vreport.getSize();

  for(int i=0; i<vehicles; i++) {
    SlotReport prox_report = vreport.getProxReport(i);
    SlotReport stat_report = vreport.getStatReport(i);

    cout << "Vehicle " << i << endl;
    cout << "---------------------------------------" << endl;

    printSlotReport(prox_report);
    printSlotReport(stat_report);

    cout << endl;
  }
}


//----------------------------------------------------------------
// Procedure: printSlotReport()

void printSlotReport(SlotReport slot_report)
{
  int vsize = slot_report.getSize();
  if(vsize == 0) {
    cout << "  -- Empty Slot Report -- " << endl;
    return;
  }
  
  cout << "Total Time: " << slot_report.getTimeTotal() << endl;

  cout << "0 <= x <= " << slot_report.getSlotVal(0) << ": ";
  cout << slot_report.getSlotTime(0) << endl;

  for(int i=1; i<vsize; i++) {
    double lval = slot_report.getSlotVal(i-1);
    double hval = slot_report.getSlotVal(i);
    cout << lval << "  < x <= " << hval << ": ";
    cout << slot_report.getSlotTime(i) << endl;
  }

  if(vsize > 1) {
    double lval = slot_report.getSlotVal(vsize-1);
    cout << lval << " <= x "  << ": ";
    cout << slot_report.getTimeOver() << endl;
  }

}



//----------------------------------------------------------------
// Procedure: latexVReportToFile()

void latexVReportToFile(VReport vreport, string filename)
{
  vector<string> svector = latexVReport(vreport);
  FILE *f = fopen(filename.c_str(), "w");
  if(!f)
    return;

  int vsize = svector.size();
  for(int i=0; i<vsize; i++)
    fprintf(f, "%s\n", svector[i].c_str());
  fclose(f);
}

//----------------------------------------------------------------
// Procedure: latexVReport()

vector<string> latexVReport(VReport vreport)
{
  SlotReport prox_report = vreport.getProxReport(0);
  SlotReport stat_report = vreport.getStatReport(0);

  vector<string> svector;
  
  svector.push_back("\\newcommand{\\tableone}{");
  svector.push_back("\\setlength{\\tabcolsep}{0.4mm}");
  svector.push_back("\\begin{tabular}{ r c l c c c r c l c c }");
  svector.push_back(" ");
  svector.push_back("\\multicolumn{5}{c}{CPA Distances (meters)} & &");
  svector.push_back("\\multicolumn{5}{c}{Transit Values (seconds)} \\\\ ");
  svector.push_back("\\cline{1-5} \\cline{7-11}");


  vector<string> slot_vector1 = latexSlotReport(prox_report);
  vector<string> slot_vector2 = latexSlotReport(stat_report);
  
  int vsize1 = slot_vector1.size();
  int vsize2 = slot_vector2.size();
  int vmax   = (vsize2>vsize1)?vsize2:vsize1;
  
  for(int i=0; i<vmax; i++) {
    string line;
    if(i >= vsize1)
      line = " & & & & ";
    else
      line = slot_vector1[i];
    line += " & \\hspace{1.0in} & ";
    if(i >= vsize2)
      line += " & & & & ";
    else
      line += slot_vector2[i];
    line += " \\\\ ";
    svector.push_back(line);
  }
  svector.push_back("\\cline{1-5} \\cline{7-11}");


  svector.push_back("\\multicolumn{5}{c}{CPA Score: 33} & &");
  svector.push_back("\\multicolumn{5}{c}{Transit Score: 44}   \\\\ \\\\");
  svector.push_back("\\multicolumn{11}{c}{Overall Score: 39} \\\\    ");
  svector.push_back(" ");
  svector.push_back("\\end{tabular}");
  svector.push_back("\\normalsize}");
  svector.push_back(" ");
  svector.push_back(" ");
  svector.push_back(" ");
  svector.push_back("\\begin{table}");
  svector.push_back("\\caption{Results with no collision avoidance at all}");
  svector.push_back("\\vspace{0.3in}");
  svector.push_back("\\centering \\tableone");
  svector.push_back("\\end{table}");

  return(svector);
}

//----------------------------------------------------------------
// Procedure: latexSlotReport()

vector<string> latexSlotReport(SlotReport slot_report)
{
  vector<string> svector;
  
  double val;
  int vsize = slot_report.getSize();
  if(vsize == 0)
    return(svector);

  string str;
  str += "0 & $\\ldots$ & ";
  val = slot_report.getSlotVal(0);
  str += dstringCompact(doubleToString(val)); 
  str += " & \\hspace{0.2in} & ";
  val = slot_report.getSlotTime(0);
  str += dstringCompact(doubleToString(val));
  svector.push_back(str);

  for(int i=1; i<vsize; i++) {
    str = "";
    double lval = slot_report.getSlotVal(i-1);
    double hval = slot_report.getSlotVal(i);
    str += dstringCompact(doubleToString(lval));
    str += " & $\\ldots$ & ";
    str += dstringCompact(doubleToString(hval)); 
    str += " & \\hspace{0.2in} & ";
    val = slot_report.getSlotTime(i);
    str += dstringCompact(doubleToString(val));
    svector.push_back(str);
  }

  if(vsize > 1) {
    str = "";
    double lval = slot_report.getSlotVal(vsize-1);
    str += dstringCompact(doubleToString(lval));
    str += " & $\\ldots$ & $\\infty$ & \\hspace{0.2in} & ";
    val = slot_report.getTimeOver();
    str += dstringCompact(doubleToString(val));
    svector.push_back(str);
  }

  return(svector);
}




