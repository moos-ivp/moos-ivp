/*****************************************************************/
/*    NAME: Kyle Woerner, Michael Benjamin                       */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: ScanHandler.h                                        */
/*    DATE: June 5th, 2008                                       */
/*                                                               */
/* This file is part of MOOS-IvP                                 */
/*                                                               */
/* MOOS-IvP is free software: you can redistribute it and/or     */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation, either version  */
/* 3 of the License, or (at your option) any later version.      */
/*                                                               */
/* MOOS-IvP is distributed in the hope that it will be useful,   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty   */
/* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See  */
/* the GNU General Public License for more details.              */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with MOOS-IvP.  If not, see                     */
/* <http://www.gnu.org/licenses/>.                               */
/*****************************************************************/

#ifndef SCAN_HANDLER_HEADER
#define SCAN_HANDLER_HEADER

#include "ScanReport.h"
#include <cmath>
#include <vector>

class EffHandler
{
 public:
  EffHandler();
  ~EffHandler() {}

  void setParam(const std::string&, const std::string&);
  void handle(int argc, char **argv);//const std::string& alogfile);

  void appStatReport();

  std::string procColor(std::string proc_name);
  void setALogFiles(int argc, char **argv);

 protected:

  struct effData {
    int counter;
    double eff_sum;
    double lin_dist_sum;
    double odo_dist_sum;
    double eff_mean; // mean
    double eff_var;  // variance
    double eff_std;  // standard deviation
    double eff_max;
    double eff_min;
    std::vector<double> eff;

    effData(){
      counter = 0;
      eff_sum = 0;
      lin_dist_sum = 0;
      odo_dist_sum = 0;
      eff_mean = 0; // mean
      eff_var = 0;  // variance
      eff_std = 0;  // standard deviation
      eff_max = -999;
      eff_min = 999;
    };
    void add_eff(double d1, double d2, double d3){
      counter++;
      lin_dist_sum += d1;
      odo_dist_sum += d2;
      eff_sum += d3;
      eff.push_back(d3);
    }

    void compute_stats(){
      eff_mean = (eff_sum / counter);
      std::cout << "computing statistics... ";
      for(std::vector<double>::iterator j=eff.begin();j!=eff.end();++j){
	eff_var += pow(( *j - eff_mean ),2);
	//	std::cout << "variance: " << doubleToString(eff_var) << std::endl;
	eff_min = fmin(eff_min, *j);
	eff_max = fmax(eff_max, *j);
      }
      eff_var =  eff_var / (counter - 1);
      // std::cout << "final variance: " << doubleToString(eff_var) << std::endl;
      eff_std = sqrt(eff_var);
      std::cout << "done." << std::endl;
      std::cout << "sum: " << doubleToString(eff_sum) << std::endl;
      std::cout << "counter: " << doubleToString(counter) << std::endl;

    }

    std::string get_report(){
      compute_stats();
      std::string str = "number of elements: " +  doubleToString(counter,0) + "\nmean = " + doubleToString(eff_mean) + "\nvariance: " + doubleToString(eff_var,8) + "\nstandard deviation: " + doubleToString(eff_std,8) + "\nmin = " + doubleToString(eff_min) + "\nmax = " + doubleToString(eff_max) + "\n";
      return str;
    }
  
  }; // end of structure

  


  double getParsedDoubleValue(std::string, std::string);

  std::string m_sort_style;
  std::string m_avoid_out_string;
  std::string m_transit_out_string;

  ScanReport  m_report;
  bool        m_use_colors;
  FILE *m_file;
  FILE *m_file_out;

  effData avoid_data;
  effData transit_data;
  std::vector<std::string> m_alog_files;
  //  std::string m_alog_files;
  int m_num_collisions;
  std::string  m_output_string;

  std::map<std::string, std::string> m_pcolor_map;
  std::vector<std::string>           m_pcolors;
  unsigned int                       m_next_color_ix;
};

#endif





