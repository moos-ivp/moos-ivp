/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE: AOFFactor.cpp                                        */
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
#include "AOFFactory.h"
#include "fileutil.h"
#include "stringutil.h"
#include <iostream>
#include <cstdlib>

#ifdef _WIN32
   #include <windows.h>
#else
   #include <unistd.h>
   #include <dlfcn.h>
#endif


using namespace std;

//========================================================================

AOFFactory::AOFFactory() {
  //cerr << "AOFFactory::AOFFactory()" << endl;
}

//========================================================================

AOFFactory::~AOFFactory() {
  //cerr << "AOFFactory::~AOFFactory()" << endl;
  // If this is being called as the program is being shut down, it's probably
  // superfluous.  But just in case it's not...
  
  for(unsigned int i = 0; i < open_library_handles.size(); ++i) {
    // dlclose(open_library_handles[i]);
  }
}

//========================================================================

#include <iostream>
using namespace std;

void AOFFactory::load_directory(string dirname) {
   vector<string> files;
   int status = listdir (dirname, files);
   if (status) {
      cerr << "Unable to read contents of the directory: " << dirname << endl;
      exit(status);
   }

   for(unsigned int i=0; i<files.size(); ++i) {
     const string & fname = files[i];
     const string fpath = dirname + '/' + fname;
    
     // Make sure it looks like an AOF's .so file...
     if(fname.substr(0, 7) != "libAOF_")
     {
         continue;
     }


     #ifdef __APPLE__
       const string library_suffix = ".dylib";
     #else
       const string library_suffix = ".so";
     #endif
     
     const string::size_type suffix_len = library_suffix.size();
     
     if (fname.substr(fname.length() - suffix_len, 
       suffix_len) != library_suffix)
     {
       continue;
     }

     if(! isRegularFile(fpath)) {
       cerr << "Warning: File " << fname << " isn't a regular file." << endl;
       continue;
     }

     // Strip off the leading 'lib' and trailing '.so'  / '.dylib' from the 
     // filename, because people using the AOFs want to call them just 
     // "AOF_...".
     string aof_name = fname.substr(3, fname.length() - (3 + suffix_len));

     cerr << "        About to load AOF library: " << aof_name << " ... ";
     
     // Load the library file, then go after the symbols we need...
#ifdef _WIN32
	 void* handle = LoadLibrary(fpath.c_str());
     
	 if (handle == NULL) {
       cerr << "Error calling LoadLibrary() on file " << fname << endl;
	   cerr << "GetLastError() returns: " << GetLastError() << endl;
#else
     void* handle = dlopen(fpath.c_str(), RTLD_LAZY);

     if (handle == NULL) {
       cerr << endl;
       cerr << "Error calling dlopen() on file " << fname << endl;
       cerr << "dlerror() returns: " << dlerror() << endl;
#endif
	   exit(1);
     }

     // Apparently ISO C++ doesn't permit you to cast a (pointer to an object) 
     // to (a pointer to a function).  And (at least) gcc 3.3 treads "void *" 
     // as a pointer to an object.  So it gives a compiler error when we use
     // "reinterpret_cast" in the statement below.  This problem seems absent
     // from (at lesat) gcc 4.2.3 and later.  But, we still want older compilers
     // to be able to build IvP, so we're going to use an old-style C cast to 
     // side-step the compiler error. -CJC


#ifdef _WIN32
     TFuncPtrCreateAOF createFn = 
       (TFuncPtrCreateAOF)(GetProcAddress((HMODULE)handle,"createAOF"));
     
     const DWORD dlsym_error = GetLastError();
     
     if (dlsym_error) {
       cerr << "Cannot load symbol 'createAOF' from file " << fname << endl;
       cerr << "dlerror() returns: " << dlsym_error << endl;
#else
       TFuncPtrCreateAOF createFn = 
	 (TFuncPtrCreateAOF)(dlsym(handle, "createAOF"));
       
     const char *dlsym_error = dlerror();
     if (dlsym_error) {
       cerr << endl;
       cerr << "Cannot load symbol 'createAOF' from file " << fname << endl;
       cerr << "dlerror() returns: " << dlsym_error << endl;
#endif
       exit(1);
     }
     
     cerr << "SUCCESS" << endl;
     
     creation_funcs[aof_name] = createFn;
     open_library_handles.push_back(handle);
     }
}

//=========================================================================

void AOFFactory::loadEnvVarDirectories(std::string envVar, bool verbose) {
  if (verbose) {
    cerr << "Loading AOF dynamic libraries...." << endl;
  }

  const char * dirs = getenv(envVar.c_str());
  if (! dirs) {
    if (verbose) {
      cerr << "    Can't load AOF libraries." << endl;
      cerr << "    Environment variable " << envVar << " isn't set." << endl;
    }

    cerr << "Loading AOF dynamic libraries - skipped." << endl;
    return;
  }
  
  vector<string> v = tokenize(dirs, ":");
  for(unsigned int i=0; i<v.size(); ++i) {
    string directory = v.at(i);

    if (isDirectory(directory)) {
      if (verbose) {
        cerr << "    Loading directory: " << directory << endl;
      }

      load_directory(directory);
    }
    else {
      if (verbose) {
        cerr << "    Skipping: " << directory << endl;
        cerr << "    (Seems not not be a directory.)" << endl;
      }
    }
  }

  cerr << "Loading AOF dynamic libraries - FINISHED." << endl;
}

//==============================================================================

bool AOFFactory::is_known_aof(string aof_name) const {
   return (creation_funcs.find(aof_name) != creation_funcs.end());
}

//==============================================================================

vector<string> AOFFactory::get_aof_names() const {
   vector<string> aof_names;
   
   for (CreationFuncsMap::const_iterator p = creation_funcs.begin();
         p != creation_funcs.end(); ++p) {
      aof_names.push_back(p->first);
   }

   return aof_names;
}

//==============================================================================

AOF* AOFFactory::new_aof(string name, IvPDomain domain) {
   if (creation_funcs.find(name) == creation_funcs.end()) {
      return NULL;
   }

   TFuncPtrCreateAOF createFunc = creation_funcs[name];
   AOF * pAOF = createFunc(name, domain);

   return pAOF;
}

//==============================================================================









