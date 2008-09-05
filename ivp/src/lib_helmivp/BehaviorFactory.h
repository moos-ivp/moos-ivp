#ifndef BEHAVIOR_LOADER_HEADER
#define BEHAVIOR_LOADER_HEADER

#include<string>
#include<map>
#include<vector>

#include "IvPDomain.h"
#include "IvPBehavior.h"

class BehaviorFactory {
   public:
      BehaviorFactory();
      virtual ~BehaviorFactory();

      // Loads all the .so libraries named BHV_*.so from the specified
      // directory.  Calls exit() if anything goes wrong.
      void load_directory(std::string dirname);

      // envVar is the name of an environment variable.  That variable should
      // be a colon-separated list of directories that may contain behavior
      // shared objects.  This calls 'load_directory' once for each directory.
      // It also skips over elements that appear to not be valid directories.
      // 
      // If 'vebose' is true, this will print to stderr a report of its 
      // progress.
      void loadEnvVarDirectories(std::string envVar, bool verbose);
     
      std::vector<std::string> get_behavior_names() const;
      bool is_known_behavior(std::string bhv_name) const;

      // Note: Don't delete this BehaviorFactory until you've destroyed all of
      // the behavior objects that were instantiated through it.  Otherwise
      // their class' object code may have been deleted and you'll probably
      // get a SEGV (at best).
      //
      // Iff the factory doesn't recognize the behavior type 'name', this method
      // returns NULL.
      IvPBehavior* new_behavior(std::string name, IvPDomain domain);
//       void delete_behavior(IvPBehavior* pBehavior);

   private:
      // These describe the signature of the functions that create and delete
      // a Behavior object.  Each shared library that implements a behavior
      // must have one of each of these functions, and they must be named
      // "create_behavior" and "delete_behavior", respectively...
      typedef IvPBehavior * (*TFuncPtrCreateBehavior) (std::string, IvPDomain);
//       typedef void (*TFuncPtrDeleteBehavior) (IvPBehavior*);

      typedef std::map<std::string, TFuncPtrCreateBehavior> CreationFuncsMap;
//       typedef std::map<std::string, TFuncPtrDeleteBehavior> DeletionFuncsMap;
     
      CreationFuncsMap creation_funcs;
//       DeletionFuncsMap deletion_funcs;
//       std::map<IvPBehavior*, TFuncPtrDeleteBehavior> bhv_delete_fn_map;

      std::vector<void*> open_library_handles;
};

#endif
