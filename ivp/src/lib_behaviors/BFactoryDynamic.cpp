/*****************************************************************/
/*    NAME: Michael Benjamin and John Leonard                    */
/*    ORGN: NAVSEA Newport RI and MIT Cambridge MA               */
/*    FILE: BFactoryDynamic.cpp                                  */
/*    DATE: Feb 11th, 2010                                       */
/*          (initial work by cjc in 2008)                        */
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

#include "BFactoryDynamic.h"
#include "MBUtils.h"
#include "fileutil.h"
#include "stringutil.h"
#include <iostream>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <dlfcn.h>
#endif

using namespace std;

//--------------------------------------------------------------------
// Destructor

BFactoryDynamic::~BFactoryDynamic() 
{
  // cerr << "BFactoryDynamic::~BFactoryDynamic()" << endl;
  // If this is being called as the program is being shut down, it's 
  // probably superfluous.  But just in case it's not...

  // Note: Don't delete this BFactoryDynamic until you've destroyed all
  // behavior objects that were instantiated through it.  Otherwise
  // their class' object code may have been deleted and you'll 
  // probably get a SEGV (at best).
  
  for(unsigned int i = 0; i<m_open_library_handles.size(); ++i) {
    // dlclose(open_library_handles[i]);
  }
}

//--------------------------------------------------------------------
// Procedure: loadDirectory()
//      Note: Loads all the libraries named BHV_*.so (Linux) or 
//            BHV_*.dylib (OS X) from the specified directory.  
//      Note: Calls exit() if anything goes wrong.

void BFactoryDynamic::loadDirectory(string dirname) 
{
  vector<string> files;
  int status = listdir (dirname, files);
  if (status) {
    cerr << "Unable to read contents of the directory: " << dirname << endl;
    exit(status);
  }
  
  for(unsigned int i=0; i<files.size(); ++i) {
    const string & fname = files[i];
    const string fpath = dirname + '/' + fname;
    
    // Make sure it looks like a behavior's .so file...
#ifdef _WIN32
    if(fname.substr(0, 4) != "BHV_") continue;
#else
    if(fname.substr(0, 7) != "libBHV_") continue;
#endif

#ifdef __APPLE__
    const string library_suffix = ".dylib";
#elif _WIN32
    const string library_suffix = ".dll";
#else
    const string library_suffix = ".so";
#endif
     
    const string::size_type suffix_len = library_suffix.size();
    
    if(fname.substr(fname.length() - suffix_len, 
		    suffix_len) != library_suffix) {
      continue;
    }

    if(! is_regular_file(fpath)) {
      cerr << "Warning: File " << fname << " isn't a regular file." << endl;
      continue;
    }

    // Strip off the leading 'lib' and trailing '.so'  / '.dylib' from the 
    // filename, because people using the behaviors want to call them just 
    // "BHV_...".
#ifdef _WIN32
    string bhv_name = fname.substr(0, fname.length() - (suffix_len));
#else
    string bhv_name = fname.substr(3, fname.length() - (3 + suffix_len));
#endif

    cerr << "        About to load behavior library: " << bhv_name << " ... ";
    // Load the library file, then go after the symbols we need...

#ifdef _WIN32
    void* handle = LoadLibrary(fpath.c_str());
    if(handle == NULL) {
      cerr << "Error calling LoadLibrary() on file " << fname << endl;
      cerr << "GetLastError() returns: " << GetLastError() << endl;
      exit(1);
    }
#else
    void* handle = dlopen(fpath.c_str(), RTLD_LAZY);
    if(handle == NULL) {
      cerr << endl;
      cerr << "Error calling dlopen() on file " << fname << endl;
      cerr << "dlerror() returns: " << dlerror() << endl;
      exit(1);
    }
#endif

    // Apparently ISO C++ doesn't permit you to cast a (pointer to 
    // an object) to (a pointer to a function).  And (at least) gcc 3.3 
    // treats "void *" as a pointer to an object.  So it gives a 
    // compiler error when we use "reinterpret_cast" in the statement 
    // below.  This problem seems absent from (at lesat) gcc 4.2.3 and 
    // later.  But, we still want older compilers to be able to build 
    // IvP, so we're going to use an old-style C cast to side-step 
    // the compiler error. -CJC
    
#ifdef _WIN32
    TFuncPtrCreateBehavior createFn = 
      (TFuncPtrCreateBehavior)(GetProcAddress((HMODULE)handle,"createBehavior"));
    
    const DWORD dlsym_error = GetLastError();
    
    if(dlsym_error) {
      cerr << "Cannot load symbol 'createBehavior' from file " << fname << endl;
      cerr << "dlerror() returns: " << dlsym_error << endl;
      exit(1);
    }
#else
    TFuncPtrCreateBehavior createFn = 
      (TFuncPtrCreateBehavior)(dlsym(handle, "createBehavior"));
    
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
      cerr << endl;
      cerr << "Cannot load symbol 'createBehavior' from file " << fname << endl;
      cerr << "dlerror() returns: " << dlsym_error << endl;
      exit(1);
    }
#endif

    cerr << "SUCCESS" << endl;
    
    m_creation_funcs_map[bhv_name] = createFn;
    m_open_library_handles.push_back(handle);
  }
}

//--------------------------------------------------------------------
// Procedure: loadEnvVarDirectories()
//      Note: envVar is the name of an environment variable.  That 
//            variable should be a colon-separated list of directories 
//            that may contain behaviorshared objects.  
//      Note: This calls 'loadDirectory' once for each directory.
//      Note: It also skips over apparently invalid directories.
//      Note: If 'verbose' is true, will report progress to stderr.

void BFactoryDynamic::loadEnvVarDirectories(string envVar, 
					    bool verbose) 
{
  if(verbose)
    cerr << "Loading behavior dynamic libraries...." << endl;
  
  const char *dirs = getenv(envVar.c_str());
  if(!dirs) {
    if(verbose) {
      cerr << "    Can't load behavior libraries." << endl;
      cerr << "    Environment variable " << envVar << " isn't set." << endl;
    }
    
    cerr << "Loading behavior dynamic libraries - skipped." << endl;
    return;
  }
  
#ifdef _WIN32
  vector<string> v = tokenize(dirs, ";");
#else
  vector<string> v = tokenize(dirs, ":");
#endif 

  for(unsigned int i=0; i<v.size(); ++i) {
    string directory = stripBlankEnds(v.at(i));

    if(isdir(directory)) {
      if(verbose)
        cerr << "    Loading directory: " << directory << endl;
      loadDirectory(directory);
    }
    else {
      if(verbose) {
        cerr << "    Skipping: " << directory << endl;
        cerr << "    (Seems not not be a directory.)" << endl;
      }
    }
  }

  cerr << "Loading behavior dynamic libraries - FINISHED." << endl;
}

//--------------------------------------------------------------------
// Procedure: isKnownBehavior()

bool BFactoryDynamic::isKnownBehavior(string bhv_name) const 
{
  return(m_creation_funcs_map.find(bhv_name) != 
	 m_creation_funcs_map.end());
}

//--------------------------------------------------------------------
// Procedure: newBehavior()

IvPBehavior* BFactoryDynamic::newBehavior(string name) 
{
  if(m_domain.size() == 0)
    return(0);

  if(m_creation_funcs_map.find(name) == m_creation_funcs_map.end())
    return(0);
  
   TFuncPtrCreateBehavior createFunc = m_creation_funcs_map[name];
   IvPBehavior *ptr_behavior = createFunc(name, m_domain);
   
   // We need to remember the deletion function for each instance, 
   // because it's recommended here:  
   //     http://www.faqs.org/docs/Linux-mini/C++-dlopen.html
   // (near the end of that document's Section 3).
   // It may be overkill though.  If it is overkill, we can stop
   // thinking about deletion functions (even to the point of not 
   // requiring them to be defined in the behavior libraries), and 
   // this class can top trying to remember each Behavior instance just 
   // so it can look up the proper delete function.
   
   return(ptr_behavior);
}

