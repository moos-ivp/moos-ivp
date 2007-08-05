/*
 *  nmea.c  - NMEA parser for umodem messages. 
 *              
 * 
 * Copyright (C) 2003  Matt Grund, WHOI
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA 
 *
 */

#include "nmea.h"

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
	if (0==strncmp(msg,"$CACFG,SRC",10)) {
		return(NMEA_ADDRESS);
	}
	if (0==strncmp(msg,"$CACFG,ASD",10)) {
		return(NMEA_ALWAYS);
	}
	if (0==strncmp(msg,"$CACFG,PTO",10)) {
		return(NMEA_CFGPTO);
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


void NMEA_OldStrArg(char *msg,int n,char *arg){

  int l;
  char *t;
  char str[NMEA_MAXSTR];

  strcpy(str,msg);

  t=strtok(str,",*");
  for(l=0;l<n;l++)
    t=strtok(NULL,",*");

  strcpy(arg,t);

}

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

  int ret,l;
  char *t;
  char str[NMEA_MAXSTR];

  strcpy(str,msg);

  t=strtok(str,",*");
  for(l=0;l<n;l++)
    t=strtok(NULL,",*");
  ret=atoi(t);
  
  return(ret);
}

long NMEA_LongArg(char *msg,int n){

  int l;
  long ret;
  char *t;
  char str[NMEA_MAXSTR];

  strcpy(str,msg);

  t=strtok(str,",*");
  for(l=0;l<n;l++)
    t=strtok(NULL,",*");
  ret=atol(t);
  
  return(ret);
}

float NMEA_FloatArg(char *msg,int n){

  int l;
  float ret;
  char *t;
  char str[NMEA_MAXSTR];

  strcpy(str,msg);

  t=strtok(str,",*");
  for(l=0;l<n;l++)
    t=strtok(NULL,",*");
  ret=(float)atof(t);
  
  return(ret);
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
    bs[3]='\0';
    buf[l]=strtoul(bs,NULL,16);
  }

  return (nbytes);
}

int NMEA_HexToData(char *hex, unsigned char *buf){

  int l,nbytes;
  char bs[3];

  nbytes = strlen(hex)/2;
  
  for (l=0;l<nbytes;l++){
    bs[0]=hex[l*2];
    bs[1]=hex[l*2+1];
    bs[3]='\0';
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
