/* This program produces an index file where the indexes
   point to the text belonging to the keywords. This program
   have to be executed after each update of the file INFOFILE
*/

#include "lie.h"

#define BUFSIZE 2048

#define INDEXFIL "INFO.ind"
#define INFOFIL "INFO"

#define KEYCHAR  '@'
#define NEWLINE  '\n'
#define OPEN '('
#define CLOSE ')'

objtype type_code(char* name)
{ if (!strncmp(name, "int",3)) return (INTEGER);
  if (!strncmp(name, "vec",3)) return (VECTOR);
  if (!strncmp(name, "mat",3)) return (MATRIX);
  if (!strncmp(name, "grp",3)) return (GROUP);
  if (!strncmp(name, "vid",3)) return (VOID);
  if (!strncmp(name, "tex",3)) return (TEKST);
  if (!strncmp(name, "bin",3)) return (BIGINT);
  if (!strncmp(name, "pol",3)) return (POLY);
  return (ERROR);
}

static void make_key(char* start,char* key)
{ if (strlen(start) >= KEYWORDLEN)
  { strncpy(key,start,KEYWORDLEN-1); key[KEYWORDLEN-1] = '\0';
    printf("Key \"%s\" truncated\n to \"%s\".\n",start,key);
  }
  else strcpy(key,start);
}

/* scan any parents specified after CLOSE in heading of function */
static void add_parents(char* start,info_index_tp* inf)
{ int i=0; char* pt=strchr(start,KEYCHAR); char key[KEYWORDLEN];
  if (start[0]=='\0') /* NEWLINE directly after CLOSE */
    { inf->directory=false; inf->parents.n=0; return; }
  if (pt==start) /* KEYCHAR directly after CLOSE */
    { inf->directory=true;  inf->parents.n=0; return; }
  while (i<N_PARENTS && pt!=NULL)
  { *pt='\0'; /* replace KEYCHAR by '\0' */
    make_key(start,key); strcpy(inf->parents.p[i++],key);
    pt=strchr(start=pt+1,KEYCHAR);
  }
  if (strlen(start)>0)
    /* last name not followed by KEYCHAR: the key is not a dir */
  { if (i<N_PARENTS)
    { make_key(start,key); strcpy(inf->parents.p[i],key);
      inf->directory=false; ++i;
    }
    else
    { printf("Maximum number of parents (=%d) exceeded.\n",i); exit(1); }
  }
  else inf->directory=true;
  inf->parents.n=i;
}

int main(void)
{ char indexfil[LABELLENGTH];
  char infofil[LABELLENGTH];
  char buf[BUFSIZE+1];
  char key[KEYWORDLEN];
  long number_chars_read;
  int i,count=0;
  FILE *indexpt,*infopt;
  info_index_tp info; /* current info record */

  strcpy(indexfil,INDEXFIL);
  indexpt=fopen(indexfil,writemode);
  if (indexpt==NULL)
    { printf("File %s cannot be written.\n",indexfil); exit(1); }
  for (i=0; i<MAX_INFO_FILES; ++i) /* traverse INFO files */
  { char* bufpt=buf,* bufpt_newline=buf,* bufpt_open=buf,* bufpt_close=buf;
    long file_adress=0; /* offset of start of current buffer in file */
    sprintf(infofil,"%s.%d",INFOFIL,i);
    if ((infopt=fopen(infofil,readmode))==0)
      if (i==0) { printf("File %s cannot be read.\n",infofil); exit(1); }
      else continue; /* skip non-existent INFO files after INFO.0 */
    printf("Indexing file '%s'.\n",infofil);
/* repeatedly read in a buffer: */
    while (  !feof(infopt)
	  && fseek(infopt,file_adress,0)==0 /* read on after prev item */
	  && (number_chars_read=fread(buf,sizeof(*buf),BUFSIZE,infopt))!=0
	  )
    { long location_start= -1, location_end= -1; /* offsets into buffer */
      buf[number_chars_read]='\0';
      while (  (bufpt=strchr(bufpt,KEYCHAR))!=0
	    && bufpt_newline!=NULL && bufpt_close!=NULL && bufpt_open!=NULL )
      { location_end=bufpt-buf; /* offset of next key into buffer */
	if (location_start>=0) /* skip the first time around */
	{ strcpy(info.keyword,key);
	  info.start=file_adress+location_start;
	  info.size =location_end-location_start;
	  info.seq=i; count++;
	  fwrite(&info,sizeof(info_index_tp),1,indexpt); /* write record */
	}
/* Scan for new key. Pattern: ... ( ... ) ... \n */
	if (  (bufpt_open=strchr(bufpt,OPEN))==NULL
	   || (bufpt_close=strchr(bufpt_open,CLOSE))==NULL
	   || (bufpt_newline=strchr(bufpt,NEWLINE))==NULL
	   ) continue; /* no complete header line present in buffer */
	*bufpt_open='\0'; make_key(bufpt+1,key); *bufpt_open=OPEN;
	bufpt=bufpt_open;
	if (bufpt[1]==CLOSE) { info.narg=0; bufpt++; } /* @f() */
	else /* scan types */ 
	{ int k=0;
	  while (k<6 && (*bufpt!=CLOSE))
	  { info.t[k++]=type_code(++bufpt); bufpt=(bufpt+3); }
	    /* each type has EXACTLY 3 characters, separator is not read */
	  info.narg=k;
	}
	if (*bufpt != CLOSE)
	{ printf("Too many arguments (=%d) found for %s\n"
		,info.narg,key);
	  exit(1);
	}
	bufpt=bufpt_newline+1;
	location_start=bufpt-buf; /* advance to next line */
	do --bufpt_newline; while (isspace(*bufpt_newline)); /* erases CR */ 
	*++bufpt_newline='\0';
	add_parents(bufpt_close+1,&info);
      } /* end of key scanning loop */
      file_adress=file_adress + location_end;
      bufpt_newline=bufpt=bufpt_close=bufpt_open=buf;
      if (number_chars_read<BUFSIZE) break;
    } /* end of buffer reading loop */
    fclose(infopt);
  } /* end of loop over info files */
  printf("End of indexing. Final key '%s'.\nWrote %d info entries.\n"
	,key,count);
  return 0;
}
