/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BackImg.h                                            */
/*    DATE: Nov 16th 2004                                        */
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

#ifndef BACK_IMG_HEADER
#define BACK_IMG_HEADER

#include <string>

class BackImg
{
public:
  BackImg();
  ~BackImg();

  void copy(const BackImg&);

  void clearData();
  bool readTiff(std::string filename);
  void setTexture();

  bool locateTiffAndInfoFiles(std::string filename);
  
  void setVerbose(bool bval=true)           {m_verbose=false;}
  
  unsigned char* get_img_data() const       {return(m_img_data);}
  unsigned int   get_img_pix_width() const  {return(m_img_pix_width);}
  unsigned int   get_img_pix_height() const {return(m_img_pix_height);}
  unsigned int   get_img_mtr_width() const  {return(m_img_mtr_width);}
  unsigned int   get_img_mtr_height() const {return(m_img_mtr_height);}


  double get_img_centx() const       {return(m_img_centx);}
  double get_img_centy() const       {return(m_img_centy);}
  double get_img_meters() const      {return(m_img_meters_x);}

  double get_x_at_img_left() const   {return(m_x_at_img_left);}
  double get_x_at_img_right() const  {return(m_x_at_img_right);}
  double get_y_at_img_bottom() const {return(m_y_at_img_bottom);}
  double get_y_at_img_top() const    {return(m_y_at_img_top);}
  double get_x_at_img_ctr() const    {return(m_x_at_img_ctr);}
  double get_y_at_img_ctr() const    {return(m_y_at_img_ctr);}

  double get_pix_per_mtr_x() const
  {return((double)(m_img_pix_width) / (m_img_mtr_width));}
  double get_pix_per_mtr_y() const   
  {return((double)(m_img_pix_height) / (m_img_mtr_height));}

  double pixToPctX(double pix) const;
  double pixToPctY(double pix) const;

  void   setDatumLatLon(double lat, double lon);

  std::string getTiffFile() const {return(m_tiff_file);}
  std::string getInfoFile() const {return(m_info_file);}

  bool readTiffInfoEmpty(double, double, double, double);

  bool locateTiffAndInfoFiles();
  
  void   print();
  void   printTerse();

protected:
  bool readTiffData(std::string filename);
  bool readTiffInfo(std::string filename);
  bool processConfiguration();

private:
  // Info set from the tiff data file
  unsigned char* m_img_data;
  unsigned int m_img_pix_width;
  unsigned int m_img_pix_height;

  // Info set from the .info file boudaries
  double   m_img_mtr_width;
  double   m_img_mtr_height;

  bool     m_verbose;
  
  std::string m_tiff_file;
  std::string m_info_file;

  double   m_img_meters_x;  // Pct of image equivalent to 100 meters
  double   m_img_meters_y;  // Pct of image equivalent to 100 meters
  double   m_img_centx;
  double   m_img_centy;

  double   m_lon_west;
  double   m_lon_east;
  double   m_lat_north;
  double   m_lat_south;
  bool     m_boundary_set;

  // Local xy-cache information
  double   m_x_at_img_left;
  double   m_x_at_img_right;
  double   m_y_at_img_top;
  double   m_y_at_img_bottom;
  double   m_x_at_img_ctr;
  double   m_y_at_img_ctr;

  // Info on whether the datum has been set.
  double   m_datum_lat;
  double   m_datum_lon;
  bool     m_datum_lat_set;
  bool     m_datum_lon_set;
};

#endif 

