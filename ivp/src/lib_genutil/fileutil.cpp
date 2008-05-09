#include "fileutil.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

//==============================================================================

int listdir (std::string dir, std::vector<std::string> &files) {
   DIR *dp;
   struct dirent *dirp;
   if((dp = opendir(dir.c_str())) == NULL) {
      cerr << "Error(" << errno << ") opening " << dir << endl;
      return errno;
   }
   
   while ((dirp = readdir(dp)) != NULL) {
      files.push_back(string(dirp->d_name));
   }

   closedir(dp);
   return 0;
}

//==============================================================================

bool isdir(std::string filename) {
   struct stat buf;
   int rc = stat(filename.c_str(), &buf);
   
   if (rc != 0) {
      cerr << "Error(" << errno << ") calling stat() on " << filename << endl;
      exit(errno);
   }

   return (S_ISDIR(buf.st_mode) != 0);
}

//==============================================================================

bool is_regular_file(std::string filename) {
   struct stat buf;
   int rc = stat(filename.c_str(), &buf);
   
   if (rc != 0) {
      cerr << "Error(" << errno << ") calling stat() on " << filename << endl;
      cerr << "Error string: " << strerror(errno) << endl;
      exit(errno);
   }

   return (S_ISREG(buf.st_mode) != 0);
}

//==============================================================================
