README-WINDOWS.txt
Last updated: 19 June 2012
Maintainer: No one (volunteer?)




The below Windows help was last updated in 2009. We would be happy to have 
volunteers for updating the build process for Windows.





******************************************************************************
*** THIS FILE IS A WORK IN PROGRESS. CORRECTIONS OR ADDITIONS ARE GREATLY  ***
*** APPRECIATED. PLEASE CONTACT THE MAINTAINER FOR CLARIFICATIONS OR       ***
*** SUGGESTIONS.                                                           ***
******************************************************************************

OVERVIEW
========
This file gives Windows-specific steps for building and running 
MOOS-IvP software.


PROGRAMS
========
The following software programs must be installed on the Windows computer 
in order to build MOOS-IvP.  For each program we've listed some information 
on how it can be obtained.

********************
* Windows programs *
********************

(1) CMake (at least version 2.6)
Website: cmake.org

*********************
* Windows Compilers *
*********************
The following are the compilers that are known to successfully build MOOS-IvP. 

(1) Microsoft Visual Studio 9 2008
Website: http://msdn.microsoft.com/en-us/vstudio/default.aspx

DIRECTORY OVERVIEW
==================
The following is an overview of some of the important directories used by
the MOOS-IvP project. Please note that there are other important directories,
but these directories should get you started:

bin--------------------- Location of IvP generated binary files
ivp--------------------- Location of IvP files
ivp\src----------------- Location of IvP source files and IvP CMakeLists.txt 
                         file
lib--------------------- Location of IvP generated library files
MOOS-####-DATE---------- Location of MOOS files where #### is the revision 
                         number and DATE is the date that it was added to 
                         MOOS-IvP. Also the location of the MOOS 
                         CMakeLists.txt file.
MOOS-####-DATE\MOOSBin-- Location of MOOS generated library files and binary
                         files.


BUILDING MOOS-IvP
=================

**************
* Build MOOS *
**************
Use CMake to generate the project files for the MOOS project:
(1) Launch CMake.
(2) Set the directory of the source code for MOOS ( MOOS-####-DATE ).
(3) Set the binaries to be placed in the same directory as the 
    source code.
(4) Select "Configure".
(5) Select the compiler (Visual Studio 9 2008)
(6) CMake will generate some options for the project. (No action)
(7) Select "Configure"
(8) Select "OK"

Use the solution files to open the MOOS project in Visual Studio. 
Select the "Build" Menu and select "Build Solution".

*************
* Build IvP *
*************
Use CMake to generate the project files for the IvP project:
(1) Launch CMake.
(2) Set the directory of the source code for IvP ( ivp\src ).
(3) Set the binaries to be placed in the same directory as the 
    source code.
(4) Select "Configure".
(5) Select the compiler (Visual Studio 9 2008)
(6) CMake will generate some options for the project. (No action)
(7) Select "Configure"
(8) Select "OK"

Use the solution files to open the IvP project in Visual Studio. 
Select the "Build" Menu and select "Build Solution".


ENVIRONMENT VARIABLES
=====================
The MOOS-IvP source tree contains the source code for two somewhat independent
software packages: MOOS, and IvP.  

When you build the MOOS software, the MOOS executable programs get placed in 
the "MOOS/MOOSBin" subdirectory of the source code tree. Under some compilers
the executable programs may also be placed in a configuration directory such
as Debug. In these cases the path will be "MOOS/MOOSBin/Debug".

When you build the IvP software, the IvP executable programs get placed in
the "bin" subdirectory of the source code tree. Under some compilers
the executable programs may also be place in a configuration director such
as Debug. In these cases the path will be "bin/Debug".

We recommend that you put the absolute path to both of those directories into
your PATH environment variable.  This is especially important because the 
"pAntler" program, which can launch other MOOS/IvP programs, relies on the
PATH variable to find those programs. 

To view or change environment variables:
(1) Right-click "My Computer", and then click "Properties".
(2) Click the "Advanced" tab.
(3) Click "Environment variables".
(4) Click one of the following options, for either a user or a system variable:
    (a) Click "New" to add a new variable name and value.
    (b) Click an existing variable, and then click "Edit" to change its name 
        or value.
    (c) Click an existing variable, and then click "delete to remove it.


