/* t. schneider 1.3.08
;5B;5B;5B * tes@mit.edu
 * mit / whoi joint program in ocean engineering
 * use of this software outside of the schmidt laboratory at mit requires permission of the author.
 * 
 * this is a special purpose script to convert the data from the oct 07 dabob
 * bay, washington cruise pt sur ship data into the geov database. geov
 * db server information is defined below
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <math.h>

// for mysql C API
#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>


#define MAXLINE 100000	/* maximum input line size from file */
#define TIMESTAMPWIDTH	20	/* width of the timestamp in chars found in both in files */

int getline(FILE *, char [], int);
int addblanks(FILE *, int);
int process_file(FILE *);
static int one (const struct dirent *);
int parse_file_name(char *, char *, char *);
int dir_search(const char *);
double utcmktime(int, int, int, int, int, double);


#define DB_HOST "18.251.3.46"
#define DB_USER "sea"
#define DB_PASS "saline12"
#define DB_PORT 3306
#define DB_NAME "ge_db"
#define CRUISE_ID 1
#define VEHICLE_ID 5

MYSQL *conn;
double last_import_time;

int main(int argc, char *argv[])
{	
  /* initialize connection handler */
  conn = mysql_init (NULL);
  if (conn == NULL)
    {
      fprintf (stderr, "ptsur2sql: mysql_init() failed (probably out of memory)\n");
      exit (1);
    }
  /* connect to mysql server */
  if (mysql_real_connect (conn, DB_HOST, DB_USER, DB_PASS,
			  DB_NAME, DB_PORT, "", 0) == NULL)
    {
      fprintf (stderr, "ptsur2sql: mysql_real_connect() failed\n");
      mysql_close (conn);
      exit (1);
    }

  /* select db */
  if (mysql_select_db(conn, DB_NAME) != 0)
    {
      fprintf (stderr, "ptsur2sql: could not select database\n");
    }

  dir_search(argv[1]);
  mysql_close(conn);
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
	      printf("time_fix_alog: processing file: %s.%s\n", name, extension);
	      
	      /* open the necessary files and check for success */
	      char fin1_full_name[2000];
						
	      /* create a string containing the full path to the input alog file */
	      strcpy(fin1_full_name, directory);
	      strcat(strcat(fin1_full_name, "/"), ein[i]->d_name);

	      FILE  *fin1; 

	      if ((fin1 = fopen(fin1_full_name, "r")) == NULL)
		{
		  printf("time_fix_alog: can't open %s.\n", fin1_full_name);
		  return 1;
		}		

	      if(process_file(fin1))
		{
		  printf("time_fix_alog: failed to process file");
		}
	
	      /* close the files we used */
	      fclose(fin1);
  	
	    }
				
	}
      printf("ptsur2sql: leaving directory: '%s'\n", directory);		
      return 0;
    }
  else
    {
      printf("ptsur2sql: can't open the directory: '%s'", directory);
      return 1;
    }
	
  return 0;
}

	
/* 
 * given one file (*fin1), parse the ptsur data and import into the mysql
 * db no more frequently than once / sec
 */


int process_file(FILE *fin1)
{	
  char line1[MAXLINE];
  int len1;

  /* discard the first line (header info) */
  len1 = getline(fin1, line1, MAXLINE);

  /* while reading non zero lines, keep going */ 
  while ((len1 = getline(fin1, line1, MAXLINE)) > 0)
    {	
      int day, month, year;
      int hour, min, sec;

      int latdegf;
      double latminf;
      int londegf;
      double lonminf;


      int latdega;
      double latmina;
      int londega;
      double lonmina;

      double diff;
      double cog;
      double sog;
      double hdg;

      char unwanted[MAXLINE];
      
      sscanf(line1, "%d/%d/%d,%d:%d:%d,%d,%lf,%d,%lf,%d,%lf,%d,%lf,%lf,%lf,%lf,%lf,%s", &day, &month, &year, &hour, &min, &sec, &latdegf, &latminf, &londegf, &lonminf, &latdega, &latmina, &londega, &lonmina, &diff, &cog, &sog, &hdg, unwanted);


      /* debugging 
      printf("line: %s", line1);
      printf("day: %d\n", day);
      printf("month: %d\n", month);
      printf("year: %d\n", year);
      printf("hour: %d\n", hour);
      printf("min: %d\n", min);
      printf("sec: %d\n", sec);
      
      printf("latdegf: %d\n", latdegf);
      printf("latminf: %lf\n", latminf);
      printf("londegf: %d\n", londegf);
      printf("lonminf: %lf\n", lonminf);
      
      printf("sog: %lf\n", sog);
      printf("hdg: %lf\n", hdg);
      */

      double utctime = utcmktime(day, month, year, hour, min, sec);

      double lat = latdegf + latminf/60;
      double lon = -(londegf + lonminf/60); 

      char query[1000];
      
      sprintf(query, "INSERT INTO ge_data (data_vehicleid, data_cruiseid, data_time, data_lat, data_long, data_heading, data_speed, data_depth) VALUES ('%d','%d','%lf','%lf','%lf','%lf','%lf','0')", VEHICLE_ID, CRUISE_ID, utctime, lat, lon, hdg, sog);
      
      printf("query: %s\n", query);
      if (mysql_query(conn, query) != 0)
	{
	  printf ("..insert failed\n");
	}
      else
	{
	  printf("...success!\n");
	}
      
    }

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

/* takes UTC values for day, month, year (2 digit), hour, min, sec
   and returns unix timestamp. uses unix date function*/

double utcmktime(int day, int month, int year, int hour, int min, double sec)
{
  char command[200];
  sprintf(command, "date -u +\"%%s\" -d \"%02d/%02d/%02d %02d:%02d:%lf\"", day, month, year, hour, min, sec);

  //printf ("command: %s\n", command);
  
  double utctime;

  FILE *fp;
  fp = popen(command, "r");
  
  fscanf(fp, "%lf", &utctime);
  
  //printf("time: %lf\n", utctime);

  pclose(fp);
  return utctime;
} 
