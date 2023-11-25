/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Engineering, MIT, Cambridge MA    */
/*    FILE: BFactoryDynamic.cpp                                  */
/*    DATE: Feb 11th, 2010                                       */
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

#include "BFactoryDynamic.h"
#include "MBUtils.h"
#include "ColorParse.h"
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
  
  unsigned int i;
  for(i = 0; i<m_open_library_handles.size(); ++i) {
    // dlclose(open_library_handles[i]);
  }
}

//--------------------------------------------------------------------
// Procedure: loadDirectory()
//      Note: Loads all the libraries named BHV_*.so (Linux) or 
//            BHV_*.dylib (OS X) from the specified directory.  
//   Returns: false if the directory cannot be found
//            true if directory and all found behaviors loaded properly
//      Note: Calls exit() if anything goes wrong loading a behavior.

bool BFactoryDynamic::loadDirectory(string dirname) 
{
  // If the dirname does not end in a '/' character, add one. If we dont
  // do this, the same directory may be regarded as different if it is
  // loaded once with the trailing '/', and once without. This robustness
  // bug was discovered in May 2019, during 2.680, helping K.Jung.
  if(!strEnds(dirname, "/"))
    dirname = dirname + "/";
  
  if(m_loaded_dirs.count(dirname)) {
    cerr << "  Directory " << dirname << " already loaded. Skipped."<< endl;
    return(true);
  }
  m_loaded_dirs.insert(dirname);

  vector<string> files;
  int status = listdir(dirname, files);
  if(status) {
    cerr << "Unable to read contents of directory: " << dirname << endl;
    return(false);
  }
  
  unsigned int i, fsize = files.size();
  for(i=0; i<fsize; ++i) {
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

    if(!isRegularFile(fpath)) {
      cerr << "Warning: File " << fname << " not a regular file." << endl;
      continue;
    }

    // Strip off the leading 'lib' and trailing '.so' / '.dylib' from
    // the filename, because people using the behaviors want to call
    // them just "BHV_...".
#ifdef _WIN32
    string bhv_name = fname.substr(0, fname.length() - (suffix_len));
#else
    string bhv_name = fname.substr(3, fname.length() - (3 + suffix_len));
#endif

    // Check the given behavior name and directory. If the behaviors has
    // been loaded previously with the same directory name, this is ok.
    // If a behavior has been loaded previously with the same name but
    // different directory, this ambiguity should not be tolerated.

    string prev_dir = m_map_bhv_dir[bhv_name];
    if(prev_dir != "") {
      if(prev_dir != dirname) {
	cerr << "  Fatal Error: A behavior of name " << bhv_name << endl;
	cerr << "  is being loaded now from directory " << dirname << endl;
	cerr << "  Behavior of same name has been loaded" << endl;
	cerr << "  previously from directory " << prev_dir << "." << endl;
	cerr << "  This is dangerously ambiguous. Exiting. " << endl;
	exit(-1);
      }
    }
    m_map_bhv_dir[bhv_name] = dirname;
    

    cout << "    About to load behavior library: ";
    cout << termColor("magenta") << bhv_name << termColor() << " ... ";
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
      cerr << "Cant load symbol 'createBehavior' from file " <<fname<<endl;
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

    cout << "SUCCESS" << endl;
    
    m_creation_funcs_map[bhv_name] = createFn;
    m_open_library_handles.push_back(handle);
  }
  return(true);
}

//--------------------------------------------------------------------
// Procedure: loadEnvVarDirectories()
//      Note: envVar is the name of an environment variable.  That 
//            variable should be a colon-separated list of directories 
//            that may contain behaviorshared objects.  
//      Note: This calls 'loadDirectory' once for each directory.
//      Note: It also skips over apparently invalid directories.
//      Note: If 'verbose' is true, will report progress to stderr.

void BFactoryDynamic::loadEnvVarDirectories(string envVar) 
{
  cout << termColor("green");
  cout << "Loading behavior dynamic libraries specified in ";
  cout << envVar << "..."  << endl << termColor();

  const char *dirs = getenv(envVar.c_str());
  if(!dirs) {
    cerr << "  Environment variable " << envVar << " not set." << endl;
    cerr << "Loading behavior dynamic libraries specified in ";
    cerr << envVar << " skipped."  << endl;
    return;
  }
  
#ifdef _WIN32
  vector<string> v = tokenize(dirs, ";");
#else
  vector<string> v = tokenize(dirs, ":");
#endif 

  for(unsigned int i=0; i<v.size(); ++i) {
    string directory = stripBlankEnds(v.at(i));

    if(isDirectory(directory)) {
      cout << "  Loading directory: " << directory << endl;
      loadDirectory(directory);
    }
    else {
      cerr << "  Skipping: " << directory << endl;
      cerr << "  (Seems not be a directory.)" << endl;
    }
  }

  cout << termColor("green");
  cout << "Loading behavior dynamic libraries - FINISHED." << endl;
  cout << termColor();
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












