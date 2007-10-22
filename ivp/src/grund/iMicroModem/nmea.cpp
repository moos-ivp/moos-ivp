/*
 *  nmea.c  - NMEA parser for umodem messages. 
 */             

// iMicroModem MOOS Modem driver, was developed 
// by Matt Grund, Woods Hole Oceanographic Institution
//
// This code is licensed under a Creative Commons
// Attribution Non-Commercial Share-A-Like License,
// version 2.5.
//
// For more information, see the file License.html
// or the web site:
//
//  http://creativecommons.org/licenses/by-nc-sa/2.5/
//
// Copyright(c)2004, Matt Grund, WHOI. <mgrund@whoi.edu>
//
// some debugging by Alex Bahr, MIT abahr@mit.edu
// noted by lines containing abahr

#include "nmea.h"
#include <string.h>

//
//  Append Checksum, CR and NL to string
// 
void NMEA_AppendChecksum(char *str){

  char *c;
  char check[]="*  \r\n"; /* checksum template */
  int cs=0;

  /* compute checksum */
  c=str+1;                  /* skip dollar sign */
  while(*c) {
    cs ^= *(int *)(c++) ;   /* XOR of message */
    cs &= (0x0ff);          /* 8 bit, just to be sure */
  }

  if ((cs&(0x0f))>9)
    check[2] = 'A' + (cs&(0x0f)) - 10 ;
  else
    check[2] = '0' + (cs&(0x0f)) ;

  cs = cs >> 4 ;

  if ((cs&(0x0f))>9)
    check[1] = 'A' + (cs&(0x0f)) - 10 ;
  else
    check[1] = '0' + (cs&(0x0f)) ;

  strcat(str,check);
}

//
// Checks NMEA strings for correctness, checksum AND format
//
//    returns error codes for the following:
//   
//                1. no dollar sign
//                2. no comma (there must be at least one arg)
//                3. more than 1 star
//                4. bad terminator 
//                5. bad checksum
//

int NMEA_CheckChecksum(char *str){
		
	    char c0,c1;
        char *bot, *c;
     	int nargs=0,len,cs0,cs1=0,usechk=0;

		c = str ;
	
		/* check syntax */
		if (*c!='$')
			return NMEA_NODOLLAR ;
		
		while (*c){
			if (*c=='*')
				usechk++;         /*  a "*" means checksum in use  */
			if (*(c++)==',')    /*  one "," per argument */
				nargs++;
		}
		
		if (nargs==0)
			return NMEA_NOCOMMA ;
		
		if (usechk>1)
			return NMEA_STAR ;
		c = str ;
		len=strlen(c);
		bot=c+len-1;
	
		if (*(bot--)!='\n')
			return(NMEA_TERMERR);
	
		if (*(bot--)!='\r')
			return(NMEA_TERMERR);
	
		/* check checksum */
		if (usechk==1){
		
		c0 = *bot--;
		c1 = *bot--;
		
		if (*(bot)!='*')
			return(NMEA_CHKSUMERR);
		
		/* parse checksum digits */
		if ((c0>'/')&&(c0<':'))  /* 0-9 */
			cs0 = (int)c0 - 48 ;
		else if ((c0>'@')&&(c0<'G'))  /* A-F */
			cs0 = (int)c0 - 'A' + 10 ;
		else
			return(NMEA_CHKSUMERR); /* bad digit */
		
		if ((c1>'/')&&(c1<':'))  /* 0-9 */
			cs0 += 16 * ((int)c1 - 48) ;
		else if ((c1>'@')&&(c1<'G'))  /* A-F */
			cs0 += 16 * ((int)c1 - 'A' + 10) ;
		else
			return(NMEA_CHKSUMERR); /* bad digit */
		
		c++ ; /* skip the $ */
		while(*c!='*') {
			cs1 ^= *(int *)(c++) ;   /* XOR of message */
			cs1 &= (0x0ff);          /* 8 bit, just to be sure */
		}
		if (cs0 != cs1)
			return(NMEA_CHKSUMERR); /* bad checksum */
	}
	return(NMEA_NOERROR);
}


int NMEA_Scan(unsigned char *buf,unsigned int *buflen,char *msg){
	char *c,*m,*dol,*term;
	int nc;
	
	/* make sure buf a string */
	buf[*buflen]=0;
	c=(char *)buf;
	
	/* now scan for '$' */
	if (NULL==(dol=strchr(c,'$'))){  /* if no $ in string, flush it */
		memset(buf,0,*buflen);
		*buflen=0;
		msg[0]=0; /* return null string*/
		return(0);
	}
	
	/* found $, now scan for '\n' */
	if (NULL==(term=strchr(c,'\n'))){  /* if no '\n' in string, just return */
		msg[0]=0; /* return null string*/
		return(0);
	}
	
	/* now, dol/term point to first/last of msg */
	
	/* copy to msg */
	c=dol;
	m=msg;
	while (c<=term) *(m++)=*(c++); /* copy msg */
	*m='\0';
	nc=strlen(msg);
	
	/* pop msg off front of buffer  */
	m=(char *)buf;
	c=term+1;
	while(c<(char *)(buf+(*buflen))) *(m++)=*(c++);
	*buflen= *buflen - ((int)term-(int)buf) - 1;
	return(nc);
}


int NMEA_Type(char *msg){

	if (0==strncmp(msg,"$CAREV",6)) {
		if (0==strncmp(msg+14,"INIT",4))
			return (NMEA_REVERT);
		return (NMEA_READY);
	}
	if (0==strncmp(msg,"$CACFG",6)) {
		return(NMEA_CFG);
	}
	if (0==strncmp(msg,"$CAMPA",6)) {
		return(NMEA_APING);
	}
	if (0==strncmp(msg,"$CAMPC",6)) {
		return(NMEA_CPING);
	}
	if (0==strncmp(msg,"$CAMPR",6)) {
		return(NMEA_RPING);
	}
	if (0==strncmp(msg,"$CADQF",6)) {
		return(NMEA_QUALITY);
	}
	if (0==strncmp(msg,"$CATXP,2",8)) {
		return(NMEA_TXMINI);
	}
	if (0==strncmp(msg,"$CATXP",6)) {
		return(NMEA_TXPKT);
	}
	if (0==strncmp(msg,"$CATXF,2",8)) {
		return(NMEA_TXMINIDONE);
	}
	if (0==strncmp(msg,"$CATXF",6)) {
		return(NMEA_TXPKTDONE);
	}
	if (0==strncmp(msg,"$CACYC",6)) {
		return(NMEA_CI);
	}
	if (0==strncmp(msg,"$CATXD",6)) {
		return(NMEA_TXDACK);
	}
	if (0==strncmp(msg,"$CADBG",6)) {
		return(NMEA_DEBUG);
	}
	if (0==strncmp(msg,"$CAERR",6)) {
		return(NMEA_ERROR);
	}
	if (0==strncmp(msg,"$CARXA",6)) {
		return(NMEA_RXA);
	}
	if (0==strncmp(msg,"$CARXD",6)) {
		return(NMEA_RXD);
	}
	if (0==strncmp(msg,"$CADRQ",6)) {
		return(NMEA_DATREQ);
	}
	if (0==strncmp(msg,"$CAACK",6)) {
		return(NMEA_ACK);
	}
	if (0==strncmp(msg,"$CAMEA",6)) {
		return(NMEA_AEXTSEL);
	}
	if (0==strncmp(msg,"$CAMEC",6)) {
		return(NMEA_CEXTSEL);
	}
	if (0==strncmp(msg,"$CAMER",6)) {
		return(NMEA_REXTSEL);
	}
	if (0==strncmp(msg,"$CAMSA",6)) {
		return(NMEA_ASLP);
	}
	if (0==strncmp(msg,"$CAMSC",6)) {
		return(NMEA_CSLP);
	}
	if (0==strncmp(msg,"$CAMSR",6)) {
		return(NMEA_RSLP);
	}
	if (0==strncmp(msg,"$CADOP",6)) {
		return(NMEA_DOP);
	}
	if (0==strncmp(msg,"$CAMFD",6)) {
		return(NMEA_MFD);
	}
	if (0==strncmp(msg,"$SNCFR",6)) {
		return(NMEA_CFR);
	}
	if (0==strncmp(msg,"$SNPNT",6)) {
		return(NMEA_LBLPING);
	}
	if (0==strncmp(msg,"$SNTTA",6)) {
		return(NMEA_TTIMES);
	}
	if (0==strncmp(msg,"$SNMFD",6)) {
		return(NMEA_NAVMFD);
	}
	if (0==strncmp(msg,"$CAMSG,BAD_CRC,2",16)) {
		return(NMEA_BADMINI);
	}
	if (0==strncmp(msg,"$CAMSG,BAD_CRC,1",16)) {
		return(NMEA_BADPKT);
	}
	if (0==strncmp(msg,"$CAMSG,PACKET_TIMEOUT",21)) {
		return(NMEA_NOPKT);
	}
	if (0==strncmp(msg,"$CATOA",6)) {
		return(NMEA_TOA);
	}
	return(NMEA_UNKNOWN);
}

//
// returns number of arguments in NMEA message
//
int NMEA_GetNArgs(char *str)
{
	int l,len,val=0;

	len=strlen(str);
	for (l=0;l<len;l++)
		if (str[l]==',')
	       val++;
	return(val);
}

//
// Parses field "n", returns substring in "out"
// improved over strtok method - provides empty args
//
void NMEA_StrArg(char *str,int n, char *out)
{
     char *c1,*c2,*c;
     int l;
     
     strcpy(out,"");
     c=str;
     // find a delim
     while ((*c!=',')&&(*c!='*')&&(*c!='\0')) c++; // $CCCCC,
     c++; // skip the comma
     
     for (l=0;l<(n-1);l++){ // for preceding args
	     while ((*c!=',')&&(*c!='*')&&(*c!='\0')) c++; // skip preceding args
	     c++; // skip comma
     }
     c1=c; // points to first char of arg we want, or next comma	
     while ((*c!=',')&&(*c!='*')&&(*c!='\0')) c++; // skip to end 
     c2=c-1; // points to last char before comma
     if (c1<=c2)
	     strncat(out,c1,(c2-c1+1));
     else
	     out[0]='\0';
}

int NMEA_IntArg(char *msg,int n){

  char str[NMEA_MAXSTR];

  NMEA_StrArg(msg,n,str);

  if (strlen(str))
	  return(atoi(str));
  else
      return(NMEA_NAN);

}

long NMEA_LongArg(char *msg,int n){

  char str[NMEA_MAXSTR];

  NMEA_StrArg(msg,n,str);

  if (strlen(str))
	  return(atol(str));
  else
      return(NMEA_NAN);

}

float NMEA_FloatArg(char *msg,int n){

  char str[NMEA_MAXSTR];
  NMEA_StrArg(msg,n,str);

  if (strlen(str))
	  return((float)atof(str));
  else
      return((float)NMEA_NAN);

}

// added by abahr 21 August 2007

double NMEA_DoubleArg(char *msg,int n){

  char str[NMEA_MAXSTR];
  NMEA_StrArg(msg,n,str);

  if (strlen(str))
	  return((double)atof(str));
  else
      return((double)NMEA_NAN);

}



int NMEA_HexData(char *msg,int n, char *buf){


  int l,nbytes;
  char *t;
  char str[NMEA_MAXSTR],bs[3];

  strcpy(str,msg);

  t=strtok(str,",*");
  for(l=0;l<n;l++)
    t=strtok(NULL,",*");

  nbytes = strlen(t)/2;
  
  for (l=0;l<nbytes;l++){
    bs[0]=t[l*2];
    bs[1]=t[l*2+1];
    bs[2]='\0';
    buf[l]=strtoul(bs,NULL,16);
  }

  return (nbytes);
}

int NMEA_HexToData(char *hex, unsigned char *buf){

  int l,nbytes;
  char bs[4];

  nbytes = strlen(hex)/2;
  
  for (l=0;l<nbytes;l++){
    bs[0]=hex[l*2];
    bs[1]=hex[l*2+1];
    bs[2]='\0';
    buf[l]=strtoul(bs,NULL,16);
  }

  return (nbytes);
}

int NMEA_DataToHex(unsigned char *buf, int nbytes, char *hex){

  int l;
  char hexbyte[8];

  hex[0]=0;
  for (l=0;l<nbytes;l++){
	sprintf(hexbyte,"%02x",buf[l]);
	strcat(hex,hexbyte);
  }
  return (nbytes);
}
