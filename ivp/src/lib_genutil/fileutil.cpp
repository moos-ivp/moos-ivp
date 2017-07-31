/*****************************************************************/
/*    NAME: Michael Benjamin                                     */
/*    ORGN: Dept of Mechanical Eng / CSAIL, MIT Cambridge MA     */
/*    FILE:                                                      */
/*    DATE:                                                      */
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
#include "fileutil.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <cstring>
#include <cerrno>
#include <cstdlib>
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

//-------------------------------------------------
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

//-----------------------------------------------
// isDirectory

bool isDirectory(std::string filename) {
   struct stat buf;
   int rc = stat(filename.c_str(), &buf);
   
   if(rc != 0) 
     return(false);
   else
     return(S_ISDIR(buf.st_mode) != 0);
}

//-----------------------------------------------
// isRegularFile()

bool isRegularFile(std::string filename) {
   struct stat buf;
   int rc = stat(filename.c_str(), &buf);
   
   if(rc != 0)
     return(false);
   else
     return(S_ISREG(buf.st_mode) != 0);
}








