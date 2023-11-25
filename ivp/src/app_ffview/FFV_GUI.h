/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: FFV_GUI.h                                            */
/*    DATE: Apr 15th 2005                                        */
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

#ifndef FFV_GUI_HEADER
#define FFV_GUI_HEADER

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Output.H>
#include "FFV_Viewer.h"
#include "Common_IPF_GUI.h"

class FFV_GUI : public Common_IPF_GUI {
public:
  FFV_GUI(int w, int h, const char *label=0);
  virtual ~FFV_GUI() {};
  
  int  handle(int);
  void setAOF(AOF* aof)           
    {m_ffv_viewer->setAOF(aof);}

  bool setViewerParam(std::string, std::string);
  bool setViewerParam(std::string, double);
    
 protected:
  void augmentMenu();

  void resize(int, int, int, int);
  void initWidgets();
  void resizeWidgetsShape();
  void resizeWidgetsText();
  void updateXY();
  
 protected:
  FFV_Viewer  *m_ffv_viewer;

  Fl_Output   *m_fld_samp_count;
  Fl_Output   *m_fld_worst_err;
  Fl_Output   *m_fld_piece_count;

  Fl_Output   *m_fld_avg_err;
  Fl_Output   *m_fld_square_err;
  Fl_Output   *m_fld_unif_aug_size;

  Fl_Output   *m_fld_samp_high;
  Fl_Output   *m_fld_samp_low;
  Fl_Output   *m_fld_create_time;

  Fl_Output   *m_fld_uniform_str;
  Fl_Output   *m_fld_refine_reg_str;
  Fl_Output   *m_fld_reflector_errors;

  Fl_Output   *m_fld_auto_peak;
  Fl_Output   *m_fld_refine_pce_str;

private:
  inline void cb_ModBaseAOF_i(int);
  static void cb_ModBaseAOF(Fl_Widget*, int);

  inline void cb_ToggleAOF_i();
  static void cb_ToggleAOF(Fl_Widget*);

  inline void cb_ToggleIPF_i();
  static void cb_ToggleIPF(Fl_Widget*);

  inline void cb_ToggleDirectedRefine_i();
  static void cb_ToggleDirectedRefine(Fl_Widget*);

  inline void cb_ToggleSmartAug_i();
  static void cb_ToggleSmartAug(Fl_Widget*);

  inline void cb_ToggleAutoPeak_i();
  static void cb_ToggleAutoPeak(Fl_Widget*);

  inline void cb_SmartAugAmt_i(int);
  static void cb_SmartAugAmt(Fl_Widget*, int);

  inline void cb_SmartAugPct_i(int);
  static void cb_SmartAugPct(Fl_Widget*, int);

  inline void cb_ToggleFrame_i();
  static void cb_ToggleFrame(Fl_Widget*);

  inline void cb_FrameHeight_i(int);
  static void cb_FrameHeight(Fl_Widget*, int);

  inline void cb_ToggleStrict_i();
  static void cb_ToggleStrict(Fl_Widget*);

  inline void cb_ModPatchAOF_i(int);
  static void cb_ModPatchAOF(Fl_Widget*, int);

  inline void cb_ModUniformAug_i(int);
  static void cb_ModUniformAug(Fl_Widget*, int);

  inline void cb_Sample_i(int);
  static void cb_Sample(Fl_Widget*, int);

  inline void cb_Rebuild_i(int);
  static void cb_Rebuild(Fl_Widget*, int);

  inline void cb_MakeUniform_i(int);
  static void cb_MakeUniform(Fl_Widget*, int);

  inline void cb_MakePieces_i(int);
  static void cb_MakePieces(Fl_Widget*, int);

  inline void cb_PrintParams_i();
  static void cb_PrintParams(Fl_Widget*);

  inline void cb_ModPolar_i(int);
  static void cb_ModPolar(Fl_Widget*, int);

#if 0
  inline void cb_set_uniform_str_i();
  static void cb_set_uniform_str(Fl_Input*, void*);

  inline void cb_set_refine_reg_i();
  static void cb_set_refine_reg(Fl_Input*, void*);

  inline void cb_set_refine_pce_i();
  static void cb_set_refine_pce(Fl_Input*, void*);
#endif
  static void cb_Quit();
};
#endif










