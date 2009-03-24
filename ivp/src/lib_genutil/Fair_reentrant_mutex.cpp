#include <Fair_reentrant_mutex.h>
#include <cassert>

using namespace std;

//==============================================================================

Fair_reentrant_mutex::Fair_reentrant_mutex() 
   : hold_depth(0)
{
   // Let's ensure that the mutex does error checking, just to be safe...
   int rc;
   pthread_mutexattr_t mtx_attr;
   
   rc = pthread_mutexattr_init(& mtx_attr);
   assert(! rc);
   
   rc = pthread_mutexattr_settype(& mtx_attr, PTHREAD_MUTEX_ERRORCHECK);
   assert(! rc);
   
   rc = pthread_mutex_init(& mtx, & mtx_attr);
   assert(! rc);

   rc = pthread_mutexattr_destroy(& mtx_attr);
   assert(! rc);

   //---------------------------------------------------------------------------

   rc = pthread_cond_init(& lock_available_cond, NULL);
   assert(! rc);
}

//==============================================================================

Fair_reentrant_mutex::~Fair_reentrant_mutex() {
   int rc;
   rc = pthread_mutex_destroy(& mtx);
   assert(! rc);
   
   rc = pthread_cond_destroy(& lock_available_cond);
   assert(! rc);
}

//==============================================================================

void Fair_reentrant_mutex::lock() {
   int rc;
   rc = pthread_mutex_lock(& mtx);
   assert(! rc);
   
   const pthread_t self = pthread_self();

   // If this thread already owns the lock, we're just going to bump its 
   // count...
   if ((! waiters.empty()) && (waiters.front() == self)) {
      hold_depth++;
   }
   else {
      // The queue might or might not be empty, but either way, go to the back
      // of it.
      waiters.push(self);
      
      // pthread conditions can experience spurious wakeups, so we can't assume
      // that being signalled means our condition has been met...
      while (waiters.front() != self) {
         rc = pthread_cond_wait(& lock_available_cond, & mtx);
         assert(! rc);
      }

      assert(hold_depth == 0);
      hold_depth = 1;
   }
   
   rc = pthread_mutex_unlock(& mtx);
   assert(! rc);
}

//==============================================================================

void Fair_reentrant_mutex::unlock() {
   int rc;
   rc = pthread_mutex_lock(& mtx);
   assert(! rc);

   const pthread_t self = pthread_self();
   assert(waiters.front() == self);
   
   if (hold_depth > 1) {
      hold_depth--;
   }
   else {
      hold_depth = 0;
      waiters.pop();
      
      if (! waiters.empty()) {
         rc = pthread_cond_signal(& lock_available_cond);
         assert(! rc);
      }
   }

   rc = pthread_mutex_unlock(& mtx);
   assert(! rc);
}

//==============================================================================
