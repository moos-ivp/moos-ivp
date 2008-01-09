/* t. schneider 1.3.08
 * tes@mit.edu
 * mit / whoi joint program in ocean engineering
 * use of this software outside of the schmidt laboratory at mit requires permission of the author.
 * 
 * see README for compilation and usage
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <math.h>
#include "MOOSLib.h"
#include "MOOSGeodesy.h"
#include <sstream>
#include <iomanip>
#include <iostream>
CMOOSGeodesy m_Geodesy;

using namespace std;

#define MAXLINE 100000  /* maximum input line size from file */
#define TIMESTAMPWIDTH  20      /* width of the timestamp in chars found in both in files */

int getline(FILE *, char [], int);
int addblanks(FILE *, int);
int process_file(FILE *, char *);
static int one (const struct dirent *);
int parse_file_name(char *, char *, char *);
int dir_search(const char *);

char * m_gps_varname;
int m_mode;

int main(int argc, char *argv[])
{       
  
    if (argc < 2)
    {
        printf("usage is (automatically recursive): addlatlong directory_with_alogs");
        return 1;
    }

    dir_search(argv[1]);

    return 0;
}

int dir_search(const char * directory)
{
    /* declare directory related vars */
    DIR *din;
    struct dirent **ein;
        
    printf("addlatlong: entering directory: '%s'\n", directory);
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
                    printf("addlatlong: processing file: %s.%s\n", name, extension);
                                                
                    /* open the necessary files and check for success */
                    char fin1_full_name[2000], fout1_full_name[2000];
                                                
                    /* create a string containing the full path to the input alog file */
                    strcpy(fin1_full_name, directory);
                    strcat(strcat(fin1_full_name, "/"), ein[i]->d_name);
                                                
                    FILE        *fin1; /* currently open alog */

                    if ((fin1 = fopen(fin1_full_name, "r")) == NULL)
                    {
                        printf("addlatlong: can't open %s.\n", fin1_full_name);
                        return 1;
                    }           

                    /* create a string with the full path to the output new alog file */
                    strcpy(fout1_full_name, directory);
                    strcat(strcat(strcat(strcat(fout1_full_name,"/"),name),"_latlong."),extension);
                  

                    /* if successful, do the file moves */
                    if(!process_file(fin1, fout1_full_name))
                    {
                        char mv_command[4000];
                        strcpy(mv_command, "mv ");
                        strcat(strcat(strcat(strcat(strcat(strcat(mv_command, fin1_full_name), " "), directory), "/"), name),"._alog");
                        printf("%s\n", mv_command);
                      
                        system(mv_command);

                    }
                                                
                    /* close the files we used */
                    fclose(fin1);

                }
        
            }
                                
        }
        printf("addlatlong: leaving directory: '%s'\n", directory);             
        return 0;
    }
    else
    {
        printf("addlatlong: can't open the directory: '%s'", directory);
        return 1;
    }
        
    return 0;
}

        
/* given two files, fin1 = *.alog file, fin2 = *.slog file (if exists) 
 * fout = new *_latlong.alog file,
 * process_file performs the actual parsing and output
 */


int process_file(FILE *fin1, char * fout1_full_name)
{       
    char line1[MAXLINE];
    int len1, len2; /* length of the lines from input file */
  
    double latorigin = 1000, longorigin = 1000;

    /* while reading non zero lines from *.alog, keep going until a valid lat_origin and long origin are found */ 
    while ((len1 = getline(fin1, line1, MAXLINE)) > 0 && (latorigin == 1000 || longorigin == 1000))
    {   
        /* parse the alog line into parts - timecode | variablename | device | value */                 
        char alog_varname[100], alog_device[100], alog_value[MAXLINE - 300];
        double alog_code;

        sscanf(line1, "%lf %s %s %s", &alog_code, alog_varname, alog_device, alog_value);
      
        if(MOOSStrCmp(alog_varname, "LAT_ORIGIN"))
        {
            printf("lat_origin: %s\n", alog_value);
            latorigin = atof(alog_value);
        }
      
        if(MOOSStrCmp(alog_varname, "LONG_ORIGIN"))
        {
            printf("long_origin: %s\n", alog_value);
            longorigin = atof(alog_value);
        }
        
        //printf("%s", line1);
    }

  

    rewind(fin1);

  
    FILE        *fout1; /* currently open output alog file */

    // initialize m_Geodesy
    if (!m_Geodesy.Initialise(latorigin, longorigin))
        return MOOSFail("Geodesy init failed.\n");

    std::ofstream fout;
    fout.open(fout1_full_name);

    /* generate the new file! */

      
    double last_x, last_x_time = -100;
    double last_y, last_y_time = -100;

    while ((len1 = getline(fin1, line1, MAXLINE)) > 0)
    {
        fout << line1;
     
        /* parse the alog line into parts - timecode | variablename | device | value */                 
        char alog_varname[100], alog_device[100], alog_value[MAXLINE - 300];
        double alog_code;
     

        sscanf(line1, "%lf %s %s %s", &alog_code, alog_varname, alog_device, alog_value);

        if(MOOSStrCmp(alog_varname, "NAV_X"))
        {
            last_x_time = alog_code;
            last_x = atof(alog_value);
            if(last_x_time == last_y_time)
            {
                double lat, lon;
                m_Geodesy.LocalGrid2LatLong(last_x, last_y, lat, lon);
            
                fout.setf(ios::left);
                fout.setf(ios::fixed);

                fout<<setw(15)<<setprecision(3)<<last_x_time<<' ';
                fout<<setw(20)<<"NAV_LATITUDE"<<' ';
                fout<<setw(10)<<"addlatlong"<<' ';
                fout<<setprecision(9)<<lat<<' ';
                fout<<endl;


                fout<<setw(15)<<setprecision(3)<<last_x_time<<' ';
                fout<<setw(20)<<"NAV_LONGITUDE"<<' ';
                fout<<setw(10)<<"addlatlong"<<' ';
                fout<<setprecision(9)<<lon<<' ';
                fout<<endl;

    
                last_x_time = -100;
                last_y_time = -100;
            }
        }
        if(MOOSStrCmp(alog_varname, "NAV_Y"))
        {
            last_y_time = alog_code;
            last_y = atof(alog_value);
        
            if(last_x_time == last_y_time)
            {
                double lat, lon;
                m_Geodesy.LocalGrid2LatLong(last_x, last_y, lat, lon);
            
                fout.setf(ios::left);
                fout.setf(ios::fixed);


                fout<<setw(15)<<setprecision(3)<<last_x_time<<' ';
                fout<<setw(20)<<"NAV_LATITUDE"<<' ';
                fout<<setw(10)<<"addlatlong"<<' ';
                fout<<setprecision(9)<<lat<<' ';
                fout<<endl;


                fout<<setw(15)<<setprecision(3)<<last_x_time<<' ';
                fout<<setw(20)<<"NAV_LONGITUDE"<<' ';
                fout<<setw(10)<<"addlatlong"<<' ';
                fout<<setprecision(9)<<lon<<' ';
                fout<<endl;

                last_x_time = -100;
                last_y_time = -100;
            }    
        }
    }
  
    printf("addlatlong: generated file: %s\n", fout1_full_name);
  
    fout.close();

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

