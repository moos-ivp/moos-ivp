/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOFFactor.h                                          */
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
#ifndef BEHAVIOR_LOADER_HEADER
#define BEHAVIOR_LOADER_HEADER

#include<string>
#include<map>
#include<vector>

#include "AOF.h"
#include "IvPDomain.h"

class AOFFactory {
   public:
      AOFFactory();
      virtual ~AOFFactory();

      // Loads all the libraries named AOF_*.so (Linux) or BHV_*.dylib (OS X)
      // from the specified directory.  Calls exit() if anything goes wrong.
      void load_directory(std::string dirname);

      // envVar is the name of an environment variable.  That variable should
      // be a colon-separated list of directories that may contain behavior
      // shared objects.  This calls 'load_directory' once for each directory.
      // It also skips over elements that appear to not be valid directories.
      // 
      // If 'vebose' is true, this will print to stderr a report of its 
      // progress.
      void loadEnvVarDirectories(std::string envVar, bool verbose);
     
      std::vector<std::string> get_aof_names() const;
      bool is_known_aof(std::string aof_name) const;

      // Note: Don't delete this AOFFactory until you've destroyed all of
      // the behavior objects that were instantiated through it.  Otherwise
      // their class' object code may have been deleted and you'll probably
      // get a SEGV (at best).
      //
      // Iff the factory doesn't recognize the behavior type 'name', this method
      // returns NULL.
      AOF* new_aof(std::string name, IvPDomain domain);

   private:
      // These describe the signature of the functions that create an AOF 
      // object.  Each shared library that implements an AOF must have one of 
      // each of these functions, and it must be named "createAOF"...
      typedef AOF* (*TFuncPtrCreateAOF) (std::string, IvPDomain);

      typedef std::map<std::string, TFuncPtrCreateAOF> CreationFuncsMap;
     
      CreationFuncsMap creation_funcs;

      std::vector<void*> open_library_handles;
};

#endif









