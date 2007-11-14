// modified by t. schneider 11.5.07 tes@mit.edu
// added recursive nature and output to files (not stdout)
// also modified timestamping to be in accord with pLogger's
// .alog files.

#include <iostream>
#include <string>
#include <map>

#include "../../3rd-party-anrp/lib_anrp_util/esc_spc.h"


#define MAXLINE 10000	/* maximum input line size from file */
#define TIMESTAMPWIDTH	20	/* width of the timestamp in chars found in both in files */

int process_file(char *, char *, int, char *[]);
static int one (const struct dirent *);
int dir_search(const char *, int, char *[]);
int parse_file_name(char *, char *, char *);

extern "C" {
#include <stdio.h>
#include <zlib.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
}

using namespace std;

int main(int argc, char *argv[])
{
  if(argc <= 2) {
    cerr << "usage: " << argv[0] << " [path/to/zlog] [ VAR1 VAR2 ... [VAR-n] | all ]" << endl;
    cerr << "writes the alog same directory as zlog." << endl;
    return 1;
  }

  dir_search(argv[1], argc, argv); 
}

int dir_search(const char * directory, int argc, char *argv[])
{
  /* declare directory related vars */
  DIR *din;
  struct dirent **ein;
	
  printf("zlog2alog_tes: entering directory: '%s'\n", directory);
  int n;
  n = scandir (directory, &ein, one, alphasort);
  if (n >= 0)
    {
      int i;
      for (i = 0; i < n; ++i)
	{
	  if ((ein[i]->d_type) == DT_DIR)
	    {				
	      /* enter directory if it is not . or .. */
	      if (strcmp(ein[i]->d_name, ".") && strcmp(ein[i]->d_name,".."))
		{
		  char direct_in[1000];
		  strcpy(direct_in, directory);
		  strcat(strcat(direct_in, "/"), ein[i]->d_name);
					
		  dir_search(direct_in, argc, argv);
		}
	    }
	  /* check for regular file type */
	  else if ((ein[i]->d_type) == DT_REG)
	    {
	      char name[256], extension[100];
	      parse_file_name(ein[i]->d_name, name, extension);

	      /* if it is an zlog file, process it */
	      if(!strcmp(extension, "zlog"))
		{
		  printf("zlog2alog_tes: processing file: %s.%s\n", name, extension);
						
		  /* open the necessary files and check for success */
		  char fin1_full_name[2000], fout_full_name[2000];
						
		  /* create a string containing the full path to the input alog file */
		  strcpy(fin1_full_name, directory);
		  strcat(strcat(fin1_full_name, "/"), ein[i]->d_name);
		  
		  /* create a string with the full path to the output new alog file */
		  strcpy(fout_full_name, directory);
		  strcat(strcat(strcat(fout_full_name,"/"),name),".alog");
		  
			
		  process_file(fin1_full_name, fout_full_name, argc, argv);
								     
		}
  	
	    }
				
	}
      printf("zlog2alog_tes: leaving directory: '%s'\n", directory);		
      return 0;
    }
  else
    {
      printf("zlog2alog_tes: can't open the directory: '%s'", directory);
      return 1;
    }
	
  return 0;
}

	
/* 
 * this is the heart of the original zlog2alog code
 */
int process_file(char * fin_name, char * fout_name, int argc, char * argv[])
{	

  FILE	*fout; /* currently open output file */
  
  if ((fout = fopen(fout_name, "w")) == NULL)
    {
      printf("zlog2alog_tes: can't open %s.\n", fout_name);
      return 1;
    }

  
  bool allEnabled = false;
  map<string,bool> envar;
  gzFile ifp;
  
  ifp = gzopen(fin_name, "rb");
  if(argc == 3 && string(argv[2]) == string("all")) {
    allEnabled = true;
  } else {
    for(int i=2; i<argc; i++) {
      envar[string(argv[i])] = true;
    }
  }

  double dbST, lST, lastHitTime, mtime;
  string mkey, msender, mtype, mcontent;
	
  char inp[80];
  gzgets(ifp, inp, 80);
	
  int mtmp[4];
  sscanf(inp, "zLogger 1 %i.%i %i.%06i", &(mtmp[0]), &(mtmp[1]),
	 &(mtmp[2]), &(mtmp[3]));
	
  dbST = double(mtmp[0]) + (double(mtmp[1]) / 1000000.0);
  lST  = double(mtmp[2]) + (double(mtmp[3]) / 1000000.0);
	
  fprintf(fout, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"); // sheesh!
  fprintf(fout, "%%%% LOG FILE:       %s - converted by zlog2alog\n", argv[1]);
  fprintf(fout, "%%%% FILE OPENED ON  ---\n");
  fprintf(fout, "%%%% LOGSTART        %f\n", lST);
  fprintf(fout, "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n");
	
  while(!gzeof(ifp)) {
    char _tmp[32000];
    gzgets(ifp, _tmp, 32000);
    //if(gzeof(ifp)) continue;
    _tmp[strlen(_tmp)-1] = 0; // kill that newline! stupid pos
    string line = _tmp;
		
    switch(line[0]) {
    case 'H': // new time
      lastHitTime = atof((line.c_str())+1);
      break;
    case 'N': // new message .. doesn't mean much
      break;
    case 'V': // key (need to unesc)
      mkey = reverse_esc_spc(line.substr(1));
      break;
    case 'S': // sender (need to unesc)
      msender = reverse_esc_spc(line.substr(1));
      break;
    case 'W': // time offset sent at (relative to db time)
      mtime = atof((line.c_str())+1);
      break;
    case 'T': // type, D=double, S=string, U=undef
      mtype = line.substr(1);
      break;
    case 'C': // contents (need to unesc)
      mcontent = reverse_esc_spc(line.substr(1));

      // wee now to print it out
      if(allEnabled || envar.find(mkey) != envar.end())
	if(mtime > lST - dbST || 1)
	  fprintf(fout, "%.3f %-20s %-10s %s\n", mtime-lST, mkey.c_str(),
		 msender.c_str(), mcontent.c_str());
      break;
    default:
      cerr << "something isn't right here.. not a zlog? might also be a corrupt EOF" << endl;
      break;
    }
  }

  gzclose(ifp);
  fclose(fout);
}


/* picks a file name string of form {name}.{extension} into its parts */
int parse_file_name(char *whole, char *name, char *ext)
{
  int i = 0, per_flag = 0, per_loc;
	
  do
    {	
      if ((whole[i] == '.') && (per_flag == 0))
	{	
	  per_flag = 1;
	  per_loc = i;
	  name[i] = '\0';
	} 
      else if (!per_flag)
	{
	  name[i] = whole[i];
	}
      else
	{
	  ext[i-per_loc-1] = whole[i];
	}
      i++;	
    } while (whole[i-1] != '\0');
}


/* for use in reading all directories */
static int one (const struct dirent *unused)
{
  return 1;
}

