# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/local/bin/cmake

# The command to remove a file.
RM = /opt/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /opt/local/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413

# Include any dependencies generated for this target.
include Core/tools/umm/CMakeFiles/umm.dir/depend.make

# Include the progress variables for this target.
include Core/tools/umm/CMakeFiles/umm.dir/progress.make

# Include the compile flags for this target's objects.
include Core/tools/umm/CMakeFiles/umm.dir/flags.make

Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o: Core/tools/umm/CMakeFiles/umm.dir/flags.make
Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o: Core/tools/umm/umm.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o"
	cd /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/Core/tools/umm && /usr/bin/g++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/umm.dir/umm.cpp.o -c /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/Core/tools/umm/umm.cpp

Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/umm.dir/umm.cpp.i"
	cd /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/Core/tools/umm && /usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/Core/tools/umm/umm.cpp > CMakeFiles/umm.dir/umm.cpp.i

Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/umm.dir/umm.cpp.s"
	cd /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/Core/tools/umm && /usr/bin/g++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/Core/tools/umm/umm.cpp -o CMakeFiles/umm.dir/umm.cpp.s

Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o.requires:
.PHONY : Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o.requires

Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o.provides: Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o.requires
	$(MAKE) -f Core/tools/umm/CMakeFiles/umm.dir/build.make Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o.provides.build
.PHONY : Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o.provides

Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o.provides.build: Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o

# Object files for target umm
umm_OBJECTS = \
"CMakeFiles/umm.dir/umm.cpp.o"

# External object files for target umm
umm_EXTERNAL_OBJECTS =

/Users/mikerb/Research/moos-ivp/trunk/bin/umm: Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o
/Users/mikerb/Research/moos-ivp/trunk/bin/umm: Core/tools/umm/CMakeFiles/umm.dir/build.make
/Users/mikerb/Research/moos-ivp/trunk/bin/umm: lib/libMOOS.a
/Users/mikerb/Research/moos-ivp/trunk/bin/umm: Core/tools/umm/CMakeFiles/umm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable /Users/mikerb/Research/moos-ivp/trunk/bin/umm"
	cd /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/Core/tools/umm && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/umm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Core/tools/umm/CMakeFiles/umm.dir/build: /Users/mikerb/Research/moos-ivp/trunk/bin/umm
.PHONY : Core/tools/umm/CMakeFiles/umm.dir/build

Core/tools/umm/CMakeFiles/umm.dir/requires: Core/tools/umm/CMakeFiles/umm.dir/umm.cpp.o.requires
.PHONY : Core/tools/umm/CMakeFiles/umm.dir/requires

Core/tools/umm/CMakeFiles/umm.dir/clean:
	cd /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/Core/tools/umm && $(CMAKE_COMMAND) -P CMakeFiles/umm.dir/cmake_clean.cmake
.PHONY : Core/tools/umm/CMakeFiles/umm.dir/clean

Core/tools/umm/CMakeFiles/umm.dir/depend:
	cd /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413 /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/Core/tools/umm /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413 /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/Core/tools/umm /Users/mikerb/Research/moos-ivp/trunk/MOOS/MOOSCoreMar2413/Core/tools/umm/CMakeFiles/umm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Core/tools/umm/CMakeFiles/umm.dir/depend
