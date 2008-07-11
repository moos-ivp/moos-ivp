/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BackImg.h                                            */
/*    DATE: Nov 16th 2004                                        */
/*                                                               */
/* This program is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU General Public License   */
/* as published by the Free Software Foundation; either version  */
/* 2 of the License, or (at your option) any later version.      */
/*                                                               */
/* This program is distributed in the hope that it will be       */
/* useful, but WITHOUT ANY WARRANTY; without even the implied    */
/* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR       */
/* PURPOSE. See the GNU General Public License for more details. */
/*                                                               */
/* You should have received a copy of the GNU General Public     */
/* License along with this program; if not, write to the Free    */
/* Software Foundation, Inc., 59 Temple Place - Suite 330,       */
/* Boston, MA 02111-1307, USA.                                   */
/*****************************************************************/

#ifndef BACK_IMG_HEADER
#define BACK_IMG_HEADER

#include <string>

class BackImg
{
public:
  BackImg();
  ~BackImg();

  bool readTiff(std::string filename);
  bool readTiffData(std::string filename);
  bool readTiffInfo(std::string filename);
  void readBlankTiff();
  void setTexture();

  unsigned char* get_img_data() {return(img_data);};
  int   get_img_width()    {return(img_width);};
  int   get_img_height()   {return(img_height);};
  float get_img_centx()    {return(img_centx);};
  float get_img_centy()    {return(img_centy);};
  float get_img_meters()   {return(img_meters);};

  float get_x_at_img_left()   {return(x_at_img_left);};
  float get_x_at_img_right()  {return(x_at_img_right);};
  float get_y_at_img_bottom() {return(y_at_img_bottom);};
  float get_y_at_img_top()    {return(y_at_img_top);};
  float get_x_at_img_ctr()    {return(x_at_img_ctr);};
  float get_y_at_img_ctr()    {return(y_at_img_ctr);};

  float get_pix_per_mtr()     {return(img_meters*img_width/100);};

  float pixToPctX(float pix);
  float pixToPctY(float pix);

private:
  int            img_width;
  int            img_height;
  unsigned char* img_data;
  float          img_theta;
  float          img_centx;
  float          img_centy;
  float          img_meters;

  float          x_at_img_ctr;
  float          y_at_img_ctr;
  float          x_at_img_left;
  float          x_at_img_right;
  float          y_at_img_top;
  float          y_at_img_bottom;

};

#endif 

