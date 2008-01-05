/* t. schneider 7.10.07
 * tes@mit.edu
 * mit / whoi joint program in ocean engineering
 * use of this software outside of the schmidt laboratory at mit requires permission of the author.
 * 
 * this is a MOOS *.alog post processing tool. it takes a directory with *.alog files and recursively
 * attempts to use GPS NMEA strings logged in the file to correct the initial time stamp. the new (corrected)
 * file is output as {original_name}.timefixed.alog and the old file is renamed (untouched) as {original_name}._alog
 * 
 * if the script can find a *.slog file with the same {original_name} as the corrected *.alog file, it will process that as
 * well, producing a new file {original_name}.timefixed.slog and rename the old file as {original_name}._slog
 * 
 * compile: gcc -o time_fix_alog time_fix_alog.c -lm
 * usage: ./time_fix_alog directory_with_alog_files MOOS_variable_with_GPS [mode: 0 (default) - use raw NMEA, 1 - use preparsed GPS Time]
 * example 1: ./time_fix_alog ~/project-plus07-data/Unicorn/ GPS_RAW 0
 * GPS_RAW looks like:
 * -0.210 GPS_RAW              iGPS       $GPGGA,173210.2,4739.65331,N,12249.36092,W,2,11,1.0,-2.2,M,-18.7,M,,*4F

 * example 2: ./time_fix_alog ~/project-plus07-data/Unicorn/ GPS_TIME 1
 * GPS_TIME looks like:
 * -0.082 GPS_TIME             iGPS       173210.400000
 * 
 * IMPORTANT: this script assumes the timestamp present in the original file is not wrong by MORE than 
 * 12 hours! (this is because the gps time we are using here is time of day) be sure to examine the 
 * runtime output of this script to verify reasonable behavior as well.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <math.h>

#define MAXLINE 100000	/* maximum input line size from file */
#define TIMESTAMPWIDTH	20	/* width of the timestamp in chars found in both in files */

int getline(FILE *, char [], int);
int addblanks(FILE *, int);
int process_file(FILE *, FILE *, char *, char *);
static int one (const struct dirent *);
int parse_file_name(char *, char *, char *);
int dir_search(const char *);

char * m_gps_varname;
int m_mode;

int main(int argc, char *argv[])
{	
  if (argc < 3)
    {	
      printf("time_fix_alog: missing arguments. syntax is './time_fix_alog directory_with_alog_files MOOS_variable_with_GPS [mode: 0 (default) - use raw NMEA, 1 - use preparsed GPS Time]'\n");
      return 1;
    }

  if (argc > 3)
    m_mode = atoi(argv[3]);
  else
    m_mode = 0;

  m_gps_varname = argv[2];

  dir_search(argv[1]);

  return 0;
}

int dir_search(const char * directory)
{
  /* declare directory related vars */
  DIR *din;
  struct dirent **ein;
	
  printf("time_fix_alog: entering directory: '%s'\n", directory);
  int n;
  n = scandir (directory, &ein, one, alphasort);
  if (n >= 0)
    {
      int i;
      for (i = 0; i < n; ++i)
	{
	  if ((ein[i]->d_type) == DT_DIR)
	    {				
	      /* enter directory if it does not begin with .  */
	      //if (strcmp(ein[i]->d_name, ".") && strcmp(ein[i]->d_name,".."))
	      if((ein[i]->d_name)[0] != '.')
	      {
		  char direct_in[1000];
		  strcpy(direct_in, directory);
		  strcat(strcat(direct_in, "/"), ein[i]->d_name);
					
		  dir_search(direct_in);
		}
	    }
	  /* check for regular file type */
	  else if ((ein[i]->d_type) == DT_REG)
	    {
	      char name[256], extension[100];
	      parse_file_name(ein[i]->d_name, name, extension);

	      /* if it is an alog file, process it */
	      if(!strcmp(extension, "alog"))
		{
		  printf("time_fix_alog: processing file: %s.%s\n", name, extension);
						
		  /* open the necessary files and check for success */
		  char fin1_full_name[2000], fin2_full_name[2000], fout1_full_name[2000], fout2_full_name[2000];
						
		  /* create a string containing the full path to the input alog file */
		  strcpy(fin1_full_name, directory);
		  strcat(strcat(fin1_full_name, "/"), ein[i]->d_name);
						
		  /* create a string containing the full path to the input slog file */
		  strcpy(fin2_full_name, directory);
		  strcat(strcat(strcat(fin2_full_name,"/"),name),".slog");
		  		  


		  FILE	*fin1; /* currently open alog */
		  FILE  *fin2; /* currently open slog */

		  if ((fin1 = fopen(fin1_full_name, "r")) == NULL)
		    {
		      printf("time_fix_alog: can't open %s.\n", fin1_full_name);
		      return 1;
		    }		

		  
		  if ((fin2 = fopen(fin2_full_name, "r")) == NULL)
		    {
		      printf("time_fix_alog: cannot open (probably .slog file does not exist!): %s.\n", fin2_full_name);
		    }		
		  
		  
		  /* create a string with the full path to the output new alog file */
		  strcpy(fout1_full_name, directory);
		  strcat(strcat(strcat(strcat(fout1_full_name,"/"),name),"_timefixed."),extension);
		  

		  /* create a string with the full path to the output new slog file */
		  strcpy(fout2_full_name, directory);
		  strcat(strcat(strcat(strcat(fout2_full_name,"/"),name),"_timefixed."),"slog");
		  

		  /* if successful, do the file moves */
		  if(!process_file(fin1, fin2, fout1_full_name, fout2_full_name))
		    {
		      char mv_command[4000];
		      strcpy(mv_command, "mv ");
		      strcat(strcat(strcat(strcat(strcat(strcat(mv_command, fin1_full_name), " "), directory), "/"), name),"._alog");
		      printf("%s\n", mv_command);
		      
		      system(mv_command);

		      if(fin2)
			{
			  strcpy(mv_command, "mv ");
			  strcat(strcat(strcat(strcat(strcat(strcat(mv_command, fin2_full_name), " "), directory), "/"), name),"._slog");
			  printf("%s\n", mv_command);
			  
			  system(mv_command);

			}
      
		    }
						
		  /* close the files we used */
		  fclose(fin1);
		  if(fin2)
		    fclose(fin2);
		}
  	
	    }
				
	}
      printf("time_fix_alog: leaving directory: '%s'\n", directory);		
      return 0;
    }
  else
    {
      printf("time_fix_alog: can't open the directory: '%s'", directory);
      return 1;
    }
	
  return 0;
}

	
/* given two files, fin1 = *.alog file, fin2 = *.slog file (if exists) 
 * fout = new *.timefixed.alog file,
 * process_file performs the actual parsing and output
 */


int process_file(FILE *fin1, FILE *fin2, char * fout1_full_name, char * fout2_full_name)
{	
  char line1[MAXLINE], line2[MAXLINE];
  int len1, len2; /* length of the lines from input file */


  /* extract start time from the alog file (4 lines in) */
  double start_time; 
  
  char tempa[100], tempb[100];
  
  int j;
  for (j = 0; j < 4; j++)
    {
      len1 = getline(fin1, line1, MAXLINE);
    }
  
  sscanf(line1, "%s %s %lf", tempa, tempb, &start_time);

  //printf("start time = %lf\n", start_time);

  double gps_time = -1;
  double offset_at_gps_time;

  /* while reading non zero lines from *.alog, keep going until a valid gps_time is found */ 
  while ((len1 = getline(fin1, line1, MAXLINE)) > 0 && gps_time == -1)
    {	
      /* parse the alog line into parts - timecode | variablename | device | value */			
      char alog_varname[100], alog_device[100], alog_value[MAXLINE - 300];
      double alog_code;

      sscanf(line1, "%lf %s %s %s", &alog_code, alog_varname, alog_device, alog_value);

      switch(m_mode)
	{
	default:
	  /* look for gps_message with NMEA */
	  if(!strcmp(alog_varname, m_gps_varname))
	    {
	      char nmea_code[7];

	      //printf("%s", line1);

	      //nmea code is 6 long
	      strncpy(nmea_code, alog_value, 6);

	      //codes with times
	      if(!strcmp(nmea_code, "$GPGGA") || !strcmp(nmea_code, "$GPBWC") || !strcmp(nmea_code, "$GPRMC") || !strcmp(nmea_code, "$GPZDA"))
		{
  		  char gps_time_s [10];
		  strncpy(gps_time_s, alog_value+7, 9);

		  //printf("%s\n", nmea_code);
		  //printf("%s\n", gps_time_s);
		  
		  gps_time = atof(gps_time_s);
		  offset_at_gps_time = alog_code;
		}

	    }
	  break;

	case 1:
	  /* look for pure preparsed GPS time (of format HHMMSS.SS) */
	  if(!strcmp(alog_varname, m_gps_varname))
	    {
	      gps_time = atof(alog_value);
	      offset_at_gps_time = alog_code;
	    }

	  break;


	}
    }
  
  if (gps_time == -1)
    {
      printf("time_fix_alog: failed to find a gps time!\n");
      return 1;
    }

  printf("time_fix_alog: found a gps time: %lf @ offset: %lf from start_time: %lf\n", gps_time, offset_at_gps_time, start_time);

  /* do the hardcore time stuff */

  /* first, find the number of seconds from the start of the *day* the gps_time represents */
  double hour_sec, min_sec, sec;

  hour_sec = (floor(gps_time / 10000)*3600);
  min_sec = (fmod(floor(gps_time / 100),100)*60);
  sec = fmod(gps_time,100);
  double gps_sec = hour_sec + min_sec + sec - offset_at_gps_time;

  //printf("hour = %lf, min = %lf, sec = %lf, total = %lf\n", hour_sec, min_sec, sec, gps_sec);

  /* now, find the unix epoch timestamp for the file start time that corresponds to the previous day and the next day,
     and the offsets (in seconds) to both */
  double unix_last_day, unix_next_day;
  
  /* 86400 is number of seconds in a day */
  unix_last_day = start_time - fmod(start_time, 86400);

  double f_sec = start_time - unix_last_day;

  printf("time_fix_alog: unix epoch for last day: %lf, seconds from start of day are file = %lf, gps = %lf\n", unix_last_day, f_sec, gps_sec);
  
  /* must assume time difference is less than 12 hours. deal with condition where one time "wraps" over a day */
  if (fabs(gps_sec - f_sec) > (86400/2))
    f_sec = f_sec + 86400;
  
  double new_start_time = gps_sec - f_sec + start_time;

  printf("time_fix_alog: correcting file by %lf seconds (which is %lf hours). new start_time: %lf\n", gps_sec-f_sec, (gps_sec-f_sec)/3600 ,new_start_time);


  rewind(fin1);


  FILE	*fout1; /* currently open output alog file */
  FILE  *fout2; /* currently open output slog file */

  if ((fout1 = fopen(fout1_full_name, "w")) == NULL)
    {
      printf("time_fix_alog: can't open %s.\n", fout1_full_name);
      return 1;
    }
  

  if (fin2)
    {
      if ((fout2 = fopen(fout2_full_name, "w")) == NULL)
	{
	  printf("time_fix_alog: can't open %s.\n", fout2_full_name);
	  return 1;
	}
    }
  
  /* generate the new file(s)! */


  for (j = 0; j < 3; j++)
    {
      len1 = getline(fin1, line1, MAXLINE);
      //printf("line1: %s", line1);
      fprintf(fout1, "%s", line1);
      
      if(fin2)
	{
	  len2 = getline(fin2, line2, MAXLINE);
	  //printf("line2: %s", line2);
	  fprintf(fout2, "%s", line2);
	}
    }
  
    len1 = getline(fin1, line1, MAXLINE);
    //printf("line1: %s", line1);
    fprintf(fout1, "%%%% LOGSTART               %10.2lf\n", new_start_time);
      
    if(fin2)
	{
	  len2 = getline(fin2, line2, MAXLINE);
	  //printf("line2: %s", line2);
	  fprintf(fout2, "%%%% LOGSTART               %10.2lf\n", new_start_time);
	}
  
  while ((len1 = getline(fin1, line1, MAXLINE)) > 0)
    {
      //printf("line1: %s", line1);
      fprintf(fout1, "%s", line1);
    }

  if(fin2)
    {
      while ((len2 = getline(fin2, line2, MAXLINE)) > 0)
	{
	  //printf("line2: %s", line2);
	  fprintf(fout2, "%s", line2);
	}
    }    
  

  printf("time_fix_alog: generated file: %s\n", fout1_full_name);
  
  if(fin2)
      printf("time_fix_alog: generated file: %s\n", fout2_full_name);

  fclose(fout1);

  return 0;
}


/* reads a line from the input source and returns the length
 * modified from function from Kernighan *The C Programming Language, 2nd Edition*
 */
int getline(FILE *fp, char s[], int lim)
{
  int c, i;
	
  for(i=0; i<lim-1 && (c=getc(fp))!=EOF && c!='\n'; ++i)
    s[i] = c;
  if (c == '\n')
    {
      s[i] = c;
      ++i;
    }
  s[i] = '\0';
  return i;
}

/* adds blanks to FILE *fout based on number in int blanks
 * if blanks is nonpositive, outputs a single space instead */
int addblanks(FILE *fout, int blanks)
{	
  if (blanks < 0)
    {	
      putc(' ',fout);
    }	
  else
    {
      for(blanks; blanks > 0; blanks--)
	{
	  putc(' ',fout);
	}
    }
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

