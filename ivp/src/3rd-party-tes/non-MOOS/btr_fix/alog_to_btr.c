/* t. schneider 7.23.07
* tes@mit.edu
* mit / whoi joint program in ocean engineering
* use of this software outside of the schmidt laboratory at mit requires permission of the author.
* 
* This file takes standard input (assumed an *.alog file containing just PAEL_TIME and BTR_DATA), and outputs
* lines of the form:
*
* PAEL_TIME:BTR_DATA\n
*
* where PAEL_TIME is the value immediately proceeding the BTR_DATA in the corrected *.alog file.
* 
* ./alog_to_btr < filein.alog > fileout.btr
*
* fileout.btr can then be fed as the input to btr_fix
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>


#define MAXLINE 10000	/* maximum input line size from file */
#define TIMESTAMPWIDTH	20	/* width of the timestamp in chars found in both in files */

int getline(char line[], int maxline);

//length of the line retrieved from stdin
int len1;	

//time stamp of the last PAEL_TIME seen, broken into int.frac
int cur_PAEL_TIME_int = 0, cur_PAEL_TIME_frac = 0;

//moos_time_btr is the time offset for the latest btr value, moos_time_pael is the time offset
//for the last pael_time. these can be used to calculate the time between writing PAEL_TIME and BTR_DATA
int moos_time_btr = 0, moos_time_pael = 0;

//hold the line from standard input.
char line1[MAXLINE];

int main(int argc, char *argv[])
{	

	while ((len1 = getline(line1, MAXLINE)) > 0)
	{	
		/* parse the alog line into parts - code | variablename | device | value */			
		char alog_code[100], alog_varname[100], alog_device[100], alog_value[MAXLINE - 300];
		sscanf(line1, "%s %s %s %s", alog_code, alog_varname, alog_device, alog_value);

		/* if PAEL_TIME, update our data for the lastest PAEL_TIME */
		if (!strcmp(alog_varname, "PAEL_TIME"))
		{
			sscanf(alog_value, "%u.%u", &cur_PAEL_TIME_int, &cur_PAEL_TIME_frac);
			sscanf(alog_code, "%d", &moos_time_pael);

		}
		/* if BTR_DATA, write out btr line using last PAEL_TIME */
		if (!strcmp(alog_varname, "BTR_DATA"))
		{	
			sscanf(alog_code, "%d", &moos_time_btr);
			printf("%d.%06d:%s\n", cur_PAEL_TIME_int, cur_PAEL_TIME_frac, alog_value);
		}	
		
	}
}

int getline(char s[], int lim)
{
	int c, i;
	
	for(i=0; i<lim-1 && (c=getchar())!=EOF && c!='\n'; ++i)
		s[i] = c;
	if (c == '\n'){
		s[i] = c;
		++i;
	}
	s[i] = '\0';
	return i;
}
