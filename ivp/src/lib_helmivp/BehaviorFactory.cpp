#include "BehaviorFactory.h"
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

//========================================================================

BehaviorFactory::BehaviorFactory() {
  //cerr << "BehaviorFactory::BehaviorFactory()" << endl;
}

//========================================================================

BehaviorFactory::~BehaviorFactory() {
  //cerr << "BehaviorFactory::~BehaviorFactory()" << endl;
  // If this is being called as the program is being shut down, it's probably
  // superfluous.  But just in case it's not...
  
  for(unsigned int i = 0; i < open_library_handles.size(); ++i) {
    // dlclose(open_library_handles[i]);
  }
}

//========================================================================

#include <iostream>
using namespace std;

void BehaviorFactory::load_directory(string dirname) {
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
	 if(fname.substr(0, 4) != "BHV_")
#else
     if(fname.substr(0, 7) != "libBHV_")
#endif
     {
         continue;
     }


     #ifdef __APPLE__
       const string library_suffix = ".dylib";
     #elif _WIN32
	   const string library_suffix = ".dll";
     #else
       const string library_suffix = ".so";
     #endif
     
     const string::size_type suffix_len = library_suffix.size();
     
     if (fname.substr(fname.length() - suffix_len, 
       suffix_len) != library_suffix)
     {
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

     // TFuncPtrCreateBehavior createFn = 
     //   reinterpret_cast<TFuncPtrCreateBehavior>(dlsym(handle, "createBehavior"));
#ifdef _WIN32
	TFuncPtrCreateBehavior createFn = 
		(TFuncPtrCreateBehavior)(GetProcAddress((HMODULE)handle,"createBehavior"));

	const DWORD dlsym_error = GetLastError();

	if (dlsym_error) {
         cerr << "Cannot load symbol 'createBehavior' from file " << fname << endl;
         cerr << "dlerror() returns: " << dlsym_error << endl;
#else
     TFuncPtrCreateBehavior createFn = 
       (TFuncPtrCreateBehavior)(dlsym(handle, "createBehavior"));

     const char *dlsym_error = dlerror();
     if (dlsym_error) {
         cerr << endl;
         cerr << "Cannot load symbol 'createBehavior' from file " << fname << endl;
         cerr << "dlerror() returns: " << dlsym_error << endl;
#endif
         exit(1);
      }
     

     cerr << "SUCCESS" << endl;

     creation_funcs[bhv_name] = createFn;
     open_library_handles.push_back(handle);
   }
}

//=========================================================================

void BehaviorFactory::loadEnvVarDirectories(std::string envVar, bool verbose) {
  if (verbose) {
    cerr << "Loading behavior dynamic libraries...." << endl;
  }

  const char * dirs = getenv(envVar.c_str());
  if (! dirs) {
    if (verbose) {
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

    if (isdir(directory)) {
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

  cerr << "Loading behavior dynamic libraries - FINISHED." << endl;
}

//==============================================================================

bool BehaviorFactory::is_known_behavior(string bhv_name) const {
   return (creation_funcs.find(bhv_name) != creation_funcs.end());
}

//==============================================================================

vector<string> BehaviorFactory::get_behavior_names() const {
   vector<string> behavior_names;
   
   for (CreationFuncsMap::const_iterator p = creation_funcs.begin();
         p != creation_funcs.end(); ++p) {
      behavior_names.push_back(p->first);
   }

   return behavior_names;
}

//==============================================================================

IvPBehavior* BehaviorFactory::new_behavior(string name, IvPDomain domain) {
   if (creation_funcs.find(name) == creation_funcs.end()) {
      return NULL;
   }

   TFuncPtrCreateBehavior createFunc = creation_funcs[name];
   IvPBehavior * pBehavior = createFunc(name, domain);

   // We need to remember the deletion function for each instance, because it's
   // recommended here:  http://www.faqs.org/docs/Linux-mini/C++-dlopen.html
   // (near the end of that document's Section 3).
   // It may be overkill though.  If it is overkill, we can totally stop
   // thinking about deletion functions (even to the point of not requiring them
   // to be defined in the behavior libraries), and this class can top trying
   // to remember each Behavior instance just so it can look up the proper
   // delete function.
//    bhv_delete_fn_map[pBehavior] = deletion_funcs[name];

   return pBehavior;
}
/*
//==============================================================================

void BehaviorFactory::delete_behavior(IvPBehavior* pBehavior) {
   if (bhv_delete_fn_map.find(pBehavior) == bhv_delete_fn_map.end()) {
      cerr << "BehaviorFactory::delete_behavior: Tried to delete "
         << "behavior at address " << static_cast<void*>(pBehavior)
         << ", but this factory has no record of it." << endl;
      exit(1);
   }

   TFuncPtrDeleteBehavior deletionFunc = bhv_delete_fn_map[pBehavior];
   deletionFunc(pBehavior);
}

//==============================================================================


*/

