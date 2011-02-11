#include "fileutil.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <iostream>

#ifdef _WIN32
   // include Windows Files 
#   include <io.h>
#   include <sys/stat.h>

#   ifndef S_ISDIR
#      define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#   endif

#   ifndef S_ISREG
#      define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#   endif
#else
#   include <unistd.h>
#   include <dirent.h>
#endif


using namespace std;

//==============================================================================
#ifdef _WIN32
int listdir (std::string dir, std::vector<std::string> &files) {
   intptr_t dp;
   struct _finddata_t fileinfo;

   dir.append("\\*");
   if( (dp = _findfirst(dir.c_str(), &fileinfo)) ==  -1 ){
		cerr << "Error(" << errno << ") opening " << dir << endl;
		return errno;
   }

   do{
	   files.push_back(string(fileinfo.name));
   }while( ( _findnext(dp, &fileinfo)) != -1 );

   _findclose(dp);
   return 0;
}
#else
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
#endif
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
