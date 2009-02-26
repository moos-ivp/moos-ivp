# This file is configured by CMake automatically as DartConfiguration.tcl
# If you choose not to use CMake, this file may be hand configured, by
# filling in the required variables.


# Configuration directories and files
SourceDirectory: /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9
BuildDirectory: /home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9

# Site is something like machine.domain, i.e. pragmatic.crd
Site: socrates

# Build name is osname-revision-compiler, i.e. Linux-2.4.2-2smp-c++
BuildName: Linux-c++

# Submission information
IsCDash: 
DropSite: public.kitware.com
DropLocation: /cgi-bin/HTTPUploadDartFile.cgi
DropSiteUser: 
DropSitePassword: 
DropSiteMode: 
DropMethod: http
TriggerSite: http://public.kitware.com/cgi-bin/Submit-Fltk-TestingResults.pl
ScpCommand: /usr/bin/scp

# Dashboard start time
NightlyStartTime: 21:00:00 EDT

# Commands for the build/test/submit cycle
ConfigureCommand: "/usr/bin/cmake" "/home/cjc/moos-ivp/ivp/src/lib_fltk-1.1.9"
MakeCommand: /usr/bin/make -i

# CVS options
# Default is "-d -P -A"
CVSCommand: CVSCOMMAND-NOTFOUND
CVSUpdateOptions: -d -A -P

# Subversion options
SVNCommand: /usr/bin/svn
SVNUpdateOptions: 

# Generic update command
UpdateCommand: 
UpdateOptions: 
UpdateType: 

# Dynamic analisys and coverage
PurifyCommand: 
ValgrindCommand: 
ValgrindCommandOptions: 
MemoryCheckCommand: /usr/bin/valgrind
MemoryCheckCommandOptions: 
MemoryCheckSuppressionFile: 
CoverageCommand: /usr/bin/gcov

# Testing options
# TimeOut is the amount of time in seconds to wait for processes
# to complete during testing.  After TimeOut seconds, the
# process will be summaily terminated.
# Currently set to 25 minutes
TimeOut: 1500
