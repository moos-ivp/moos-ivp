# Install script for directory: /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Debug")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/FL" TYPE FILE FILES
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Multiline_Input.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Multiline_Input.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/gl_draw.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu_.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Progress.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Scrollbar.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Multiline_Output.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Bitmap.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/dirent.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Single_Window.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Hold_Browser.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Counter.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/math.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Slider.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Multi_Label.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_BMP_Image.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu_Window.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Hor_Value_Slider.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_XPM_Image.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Clock.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Overlay_Window.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Group.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Value_Slider.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_FormsPixmap.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Round_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Radio_Round_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_message.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu_Bar.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_BMP_Image.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/gl2opengl.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Light_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_XPM_Image.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Text_Buffer.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Roller.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Enumerations.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_message.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Toggle_Round_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Repeat_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Help_Dialog.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/glu.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Round_Clock.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Round_Clock.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Window.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Slider.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Export.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Hor_Slider.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Tooltip.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Group.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/math.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Input.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Hor_Nice_Slider.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Multi_Browser.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Sys_Menu_Bar.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Check_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_FormsPixmap.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Hor_Value_Slider.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Multi_Browser.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Output.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_RGB_Image.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Float_Input.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu_Item.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Output.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Check_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_GIF_Image.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Dial.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Chart.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu_Bar.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Radio_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Valuator.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/glut.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_FormsBitmap.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Spinner.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Scroll.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Hold_Browser.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Image.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_show_colormap.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Toggle_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Input_Choice.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_File_Icon.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Chart.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/win32.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/forms.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Simple_Counter.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Nice_Slider.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Radio_Light_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/glu.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/gl_draw.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_show_colormap.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Pack.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Preferences.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu_.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Radio_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Multi_Label.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Tabs.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Text_Editor.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Tile.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Positioner.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Shared_Image.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Text_Buffer.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_JPEG_Image.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_file_chooser.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/names.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Hor_Fill_Slider.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Tabs.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_file_chooser.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Free.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/win32.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Pack.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Secret_Input.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Wizard.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Simple_Counter.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_XBM_Image.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Input_Choice.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Choice.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Value_Input.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_ask.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Box.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_RGB_Image.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Overlay_Window.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/filename.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Help_View.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Gl_Window.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Positioner.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Scrollbar.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/names.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Check_Browser.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Widget.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Timer.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/dirent.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_File_Input.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_GIF_Image.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Spinner.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/mac.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/mac.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Line_Dial.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_draw.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Fill_Dial.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Hor_Nice_Slider.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Pixmap.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Shared_Image.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Value_Slider.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Return_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Radio_Light_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Help_View.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_ask.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Browser.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/filename.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Select_Browser.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/mac.r"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Input.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Tile.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Repeat_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Adjuster.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Text_Display.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Dial.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu_Window.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/gl.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_File_Browser.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Counter.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Wizard.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/gl2opengl.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Toggle_Light_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Tooltip.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Value_Output.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_File_Icon.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Nice_Slider.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Browser_.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Input_.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Valuator.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Gl_Window.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Toggle_Round_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Check_Browser.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Adjuster.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu_Item.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Scroll.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Color_Chooser.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Round_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_PNM_Image.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Fill_Slider.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Fill_Slider.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Help_Dialog.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/x.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/glut.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/x.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Browser.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/forms.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_FormsBitmap.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Text_Editor.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Light_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Choice.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Double_Window.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Text_Display.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_File_Browser.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Int_Input.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Timer.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_File_Input.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Toggle_Button.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Single_Window.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Image.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Input_.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_PNM_Image.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Secret_Input.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_PNG_Image.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_XBM_Image.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Box.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Fill_Dial.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Object.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Float_Input.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_File_Chooser.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Tiled_Image.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Window.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Value_Output.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Free.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_PNG_Image.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Select_Browser.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Preferences.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Export.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Int_Input.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Multiline_Output.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Radio_Round_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Bitmap.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_show_input.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Menu.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Line_Dial.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Sys_Menu_Bar.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Double_Window.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Roller.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Clock.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Progress.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Color_Chooser.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/gl.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Tiled_Image.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_File_Chooser.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Browser_.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_draw.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Return_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Enumerations.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Hor_Fill_Slider.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Object.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/fl_show_input.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Value_Input.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_JPEG_Image.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Widget.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Hor_Slider.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Pixmap.H"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl_Toggle_Light_Button.h"
    "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FL/Fl.H"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/FLTK-1.1" TYPE FILE FILES "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/CMake/FLTKUse.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/FLTK-1.1" TYPE FILE FILES "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FLTKBuildSettings.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/FLTK-1.1" TYPE FILE FILES "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/FLTKLibraryDepends.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/FLTK-1.1" TYPE FILE FILES "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/CMake/FLTKConfig.cmake")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/zlib/cmake_install.cmake")
  INCLUDE("/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/jpeg/cmake_install.cmake")
  INCLUDE("/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/png/cmake_install.cmake")
  INCLUDE("/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9/src/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

