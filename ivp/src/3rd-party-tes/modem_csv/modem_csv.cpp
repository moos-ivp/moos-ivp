/* t. schneider 8.24.07
 * tes@mit.edu
 * mit / whoi joint program in ocean engineering
 * use of this software outside of the schmidt laboratory at mit requires permission of the author.
 * 
 * see README in this directory for details on this script
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>

#define MAXLINE 10000   /* maximum input line size from file */
#define TIMESTAMPWIDTH  20      /* width of the timestamp in chars found in both in files */

int getline(FILE *, char [], int);
int addblanks(FILE *, int);
int process_file(FILE *, FILE *, double);
static int one (const struct dirent *);
int parse_file_name(char *, char *, char *);
int dir_search(const char *);

int m_modem_id;
double m_time_offset;

int main(int argc, char *argv[])
{       
    if (argc < 2)
    {   
        printf("modem_csv: missing arguments. syntax is './modem_csv directory_with_alog_files modem_id [time offset]'\n");
        return 1;
    }

    m_modem_id = atoi(argv[2]);
  
    m_time_offset = 0;

    if (argc > 3) 
        m_time_offset = atof(argv[3]);
    

    printf("modem_csv: using modem id = %d, time offset = %lf.\n", m_modem_id, m_time_offset); 
        
    dir_search(argv[1]);

    return 0;
}

int dir_search(const char * directory)
{
        
    /* declare directory related vars */
    DIR *din;
    struct dirent **ein;
        
    printf("modem_csv: entering directory: '%s'\n", directory);
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
                    printf("modem_csv: processing file: %s.%s\n", name, extension);
                                                
                    /* open the necessary files and check for success */
                    char fin1_full_name[2000], fout_full_name[2000];
                                                
                    /* create a string containing the full path to the input alog file */
                    strcpy(fin1_full_name, directory);
                    strcat(strcat(fin1_full_name, "/"), ein[i]->d_name);
                                                
                    FILE        *fin1; /* currently open alog */


                    if ((fin1 = fopen(fin1_full_name, "r")) == NULL)
                    {
                        printf("modem_csv: can't open %s.\n", fin1_full_name);
                        return 1;
                    }           
                  

                    /* extract start time from the alog file (4 lines in) */
                    double start_time; 

                    char line1[MAXLINE], tempa[100], tempb[100];
                    int len1;  
                  
                    int j;
                    for (j = 0; j < 4; j++)
                    {
                        len1 = getline(fin1, line1, MAXLINE);
                    }
                  
                    sscanf(line1, "%s %s %lf", tempa, tempb, &start_time);

                    start_time += m_time_offset;
                  
                    printf("modem_csv: unix epoch for start_time (including offset): %lf\n", start_time);

                    time_t t = (time_t) start_time;
                    struct tm * ptm = gmtime(&t);
                  
                    /* create a string with the full path to the output new alog file */
                    sprintf(fout_full_name, "NMEA-%4d-%02d-%02d-%02d-%02d-%02d.CSV", (ptm->tm_year) + 1900, \
                            (ptm->tm_mon) + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
                                                
                    FILE        *fout; /* currently open output file */

                    if ((fout = fopen(fout_full_name, "w")) == NULL)
                    {
                        printf("modem_csv: can't open %s.\n", fout_full_name);
                        return 1;
                    }
                    /* all files opened ok */   
                        
                    process_file(fin1, fout, start_time);
                        
                    printf("modem_csv: generated file: %s\n", fout_full_name);
                                                
                    /* close the files we used */
                    fclose(fin1);
                    fclose(fout);
                }
                        
            }
                                
        }
        printf("modem_csv: leaving directory: '%s'\n", directory);              
        return 0;
    }
    else
    {
        printf("modem_csv: can't open the directory: '%s'", directory);
        return 1;
    }
        
    return 0;
}

        
/* given two files, fin1 = *.alog file, fout = new *.csv file,
 * process_file performs the actual parsing and output
 */


int process_file(FILE *fin1, FILE *fout, double start_time)
{       

    char line1[MAXLINE];
    int len1; /* length of the lines from input file */
        

    /* while reading non zero lines from *.alog, keep going */ 
    while ((len1 = getline(fin1, line1, MAXLINE)) > 0)
    {   
        /* parse the alog line into parts - timecode | variablename | device | value */                 
        char alog_varname[100], alog_device[100], alog_value[MAXLINE - 300];
        double alog_code;

        sscanf(line1, "%lf %s %s %s", &alog_code, alog_varname, alog_device, alog_value);

        /* handle messages from the computer */
        if(!strcmp(alog_varname, "MICROMODEM_TO_MODEM"))
        {

            time_t t = (time_t) (start_time + alog_code);
            struct tm * ptm = gmtime(&t);

            fprintf(fout, "%d,%02d/%02d/%02d,%02d:%02d:%02d,0x0,%s\n", m_modem_id, (ptm->tm_mon) + 1, ptm->tm_mday, (ptm->tm_year) + 1900, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, alog_value);
        }

        /* handle messages from the water */
        if(!strcmp(alog_varname, "MICROMODEM_RAW"))
        {

            time_t t = (time_t) (start_time + alog_code);
            struct tm * ptm = gmtime(&t);


            /* we have no way of knowing the incoming modem id */
            int in_modem_id = 0;
            fprintf(fout, "%d,%02d/%02d/%02d,%02d:%02d:%02d,0x0,%s\n", in_modem_id, (ptm->tm_mon) + 1, ptm->tm_mday, (ptm->tm_year) + 1900, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, alog_value);
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

