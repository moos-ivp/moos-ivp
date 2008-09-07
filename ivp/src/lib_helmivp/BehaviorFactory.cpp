#include "BehaviorFactory.h"
#include "fileutil.h"
#include "stringutil.h"
#include <dlfcn.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

//========================================================================

BehaviorFactory::BehaviorFactory() {
  cerr << "BehaviorFactory::BehaviorFactory()" << endl;
}

//========================================================================

BehaviorFactory::~BehaviorFactory() {
  cerr << "BehaviorFactory::~BehaviorFactory()" << endl;
  // If this is being called as the program is being shut down, it's probably
  // superfluous.  But just in case it's not...
  
  for(unsigned int i = 0; i < open_library_handles.size(); ++i) {
    // dlclose(open_library_handles[i]);
  }
}

//========================================================================

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
     if(fname.substr(0, 7) != "libBHV_")
       continue;

     if(fname.substr(fname.length() - 3, 3) != ".so")
       continue;

     if(! is_regular_file(fpath)) {
       cerr << "Warning: File " << fname << " isn't a regular file." << endl;
       continue;
     }

     // Strip off the leading 'lib' and trailing '.so' from the filename, 
     // because people using the behaviors want to call them just "BHV_...".
     string bhv_name = fname.substr(3, fname.length() - (3 + 3));
     
     // Load the .so file, then go after the symbols we need...
     void* handle = dlopen(fpath.c_str(), RTLD_LAZY);
     if (handle == NULL) {
       cerr << "Error calling dlopen() on file " << fname << endl;
         cerr << "dlerror() returns: " << dlerror() << endl;
         exit(1);
     }

     const char *dlsym_error;

     // Apparently ISO C++ doesnt' permit you to cast a (pointer to an object) 
     // to (a pointer to a function).  And (at least) gcc 3.3 treads "void *" 
     // as a pointer to an object.  To it gives a compiler error when we use
     // "reinterpret_cast" in the statement below.  This problem seems absent
     // from (at lesat) gcc 4.2.3 and later.  But, we still want older compilers
     // to be able to build IvP, so we're going to use an old-style C cast to 
     // side-step the compiler error. -CJC

     // TFuncPtrCreateBehavior createFn = 
     //   reinterpret_cast<TFuncPtrCreateBehavior>(dlsym(handle, "createBehavior"));
     TFuncPtrCreateBehavior createFn = 
       (TFuncPtrCreateBehavior)(dlsym(handle, "createBehavior"));

     dlsym_error = dlerror();
     if (dlsym_error) {
         cerr << "Cannot load symbol 'createBehavior' from file " << fname << endl;
         cerr << "dlerror() returns: " << dlsym_error << endl;
         exit(1);
      }

     creation_funcs[bhv_name] = createFn;
     open_library_handles.push_back(handle);
   }
}

//=========================================================================

void BehaviorFactory::loadEnvVarDirectories(std::string envVar, bool verbose) {
  if (verbose) {
    cerr << ">>> Loading behavior dynamic libraries." << endl;
  }

  const char * dirs = getenv(envVar.c_str());
  if (! dirs) {
    if (verbose) {
      cerr << "Can't load behavior libraries.  Environment variable " << envVar << " isn't set." << endl;
    }

    cerr << ">>> Exiting" << endl;
    return;
  }
  
  vector<string> v = tokenize(dirs, ":");
  for(unsigned int i=0; i<v.size(); ++i) {
    string d = v.at(i);

    if (isdir(d)) {
      if (verbose) {
        cerr << ">>> Loading directory: " << d << endl;
      }

      load_directory(d);
    }
    else {
      if (verbose) {
        cerr << ">>> Seems not not be a directory.  Skipping: " << d << endl;
      }
    }
  }

  cerr << ">>> Exiting" << endl;
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

