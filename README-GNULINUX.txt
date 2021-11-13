README-LINUX.txt
Last updated: 12 November 2021
Maintainer: Mike Benjamin (issues@moos-ivp.org)


OVERVIEW
========
This file gives GNU/Linux-specific steps for building and running 
MOOS-IvP software.

Testing is primarily done on Ubuntu 18.4LTS. We are 
happy to feedback from user of other GNU/Linux distros.

UBUNTU PACKAGES
========

Packages you probably already have installed:

  g++        - GNU C++ compiler
  subversion - Advanced version control system
  cmake      - cross-platform, open-source make system
  xterm      - X terminal application 

Additional packages:

  libfltk1.3-dev  - Fast Light Toolkit - development files
  freeglut3-dev   - OpenGL Utility Toolkit development files
  libpng-dev      - PNG library - development
  libjpeg-dev     - Independent JPEG Group's JPEG runtime library 
  libxft-dev      - FreeType-based font drawing library for X
  libxinerama-dev - X11 Xinerama extension library (development headers)
  libtiff5-dev    - Tag Image File Format library (TIFF), development files
  espeak          - Text to voice (used in the iSay app)
  
To do it all, cut and paste this:

  sudo apt install g++ subversion xterm cmake libfltk1.3-dev freeglut3-dev libpng12-dev libjpeg-dev libxft-dev libxinerama-dev libtiff4-dev espeak


NOTE: As of this date we are no longer including FLTK as part of the 
      MOOS-IvP tree. If you have trouble finding FLTK 1.3 in your 
      system's package manager, we recommend that you install from the
      source code from the FLTK website.


BUILDING MOOS-IvP
=================
  ./build-moos.sh
  ./build-ivp.sh


ENVIRONMENT VARIABLES
=====================
When you build the MOOS-IvP software, the executable programs are placed
in the "moos-ivp/bin" subdirectory of the source code tree.

We recommend that you put the absolute path to this directory into
your PATH environment variable.  This is especially important because the
"pAntler" program, which can launch other MOOS/IvP programs, relies on the
PATH variable to find those programs.

We normally just add lines to our ~/.bashrc or ~/.cshrc files to always append
these two directories to the PATH environment variable.

 
 
