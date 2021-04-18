README-WINDOWS.txt
Last updated: 02 February 2021
Maintainer: No one (volunteer?)


==============================================================================
OVERVIEW
==============================================================================
This file gives steps specific to Windows 10, for building and running
MOOS-IvP software.


==============================================================================
NEEDED APPLICATIONS
==============================================================================

1) Windows Subsystem for Linux (WSL)
   You need to install the Windows Subsystem for Linux (WSL). The official,
   detailed instructions can be found at the following link:
   https://docs.microsoft.com/en-us/windows/wsl/install-win10

   Key instructions are reproduced here for reference. In a PowerShell
   session, run the following:

   > dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
   > dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart

   To switch your default to WSL2:

   > wsl --set-default-version 2


2) WSL Linux distro
   You will also need to install a suitable Linux distro to run in your WSL.
   We typically recommend Ubuntu, and these instructions are made with the
   assumption that you will use this distro in particular. The distro image
   can be downloaded and installed via the Microsoft Store.


3) X11 Server
   In order to launch MOOS-IvP graphical utilities, you will need to install
   an X11 server. Some options include:

   - VcXsrv:   https://sourceforge.net/projects/vcxsrv/
   - Xming:    https://sourceforge.net/projects/xming/
   - Cygwin/X: https://x.cygwin.com/

   These instructions are based on testing with VcXsrv, where the key
   requirement is that the Windows firewall must allow VcXsrv to access

                     ** BOTH PUBLIC AND PRIVATE **

   networks; otherwise, the WSL distro will not be able to connect to the
   X11 server to show graphical interfaces. The options for VcXsrv, by
   configuration screen, are:

   1. Multiple windows
   2. Start no client
   3. Check all boxes, including 'Disable access control'

   To configure your WSL distro to connect to the X server, add the following
   to your distro's .bashrc:

   export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0.0
   export LIBGL_ALWAYS_INDIRECT=1


==============================================================================
SOFTWARE PACKAGES
==============================================================================

Once you install Ubuntu for WSL, you'll need to install the following
packages. These instructions assume you are running Ubuntu 20.04

Packages you probably already have installed:

   g++        - GNU C++ compiler

Additional packages and development libraries:

   subversion - Advanced version control system
   cmake      - cross-platform, open-source make system
   xterm      - X terminal application

   libfltk1.3-dev  - Fast Light Toolkit - development files
   freeglut3-dev   - OpenGL Utility Toolkit development files
   libpng-dev      - PNG library - development
   libjpeg-dev     - Independent JPEG Group's JPEG runtime library
   libtiff-dev     - Tag Image File Format library (TIFF), development files

** libxft-dev      - FreeType-based font drawing library for X
** libxinerama-dev - X11 Xinerama extension library (development headers)  ??

To do it all, cut and paste this:

   sudo apt install g++ subversion xterm cmake libfltk1.3-dev freeglut3-dev libpng-dev libjpeg-dev libxft-dev libxinerama-dev libtiff4-dev


==============================================================================
BUILDING MOOS-IvP
==============================================================================
To build both MOOS and MOOS-IvP in one go, run the following command:
   ./build.sh

To build the MOOS core and MOOS-IvP separately, run the following commands:
   ./build-moos.sh
   ./build-ivp.sh


==============================================================================
ENVIRONMENT VARIABLES
==============================================================================
When you build the MOOS-IvP software, the executable programs are placed
in the "moos-ivp/bin" subdirectory of the source code tree.

We recommend that you put the absolute path to this directory into
your PATH environment variable.  This is especially important because the
"pAntler" program, which can launch other MOOS/IvP programs, relies on the
PATH variable to find those programs.

We normally just add lines to our ~/.bashrc or ~/.cshrc files to always append
these two directories to the PATH environment variable.


******************************************************************************
*** THIS FILE IS A WORK IN PROGRESS. CORRECTIONS OR ADDITIONS ARE GREATLY  ***
*** APPRECIATED. PLEASE CONTACT THE MAINTAINER FOR CLARIFICATIONS OR       ***
*** SUGGESTIONS.                                                           ***
******************************************************************************

