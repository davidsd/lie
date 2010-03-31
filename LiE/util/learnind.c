/* This program produces an index file where the indexes
   point to the text belonging to the keywords. This program
   have to be executed after each update of the file LEARN
*/

#include "lie.h"

#define INDEXFIL "LEARN.ind"
#define LEARNFIL "LEARN"

#define KEYCHAR  '@'
#define NEWLINE  '\n'
#define BUFSIZE 2048

int main(void)
{ char indexfil[LABELLENGTH];
  char learnfil[LABELLENGTH];
  char buf[BUFSIZE+1];
  char *bufpt=buf,*bufpt_newline=buf;
  char key[KEYWORDLEN];
  long number_chars_read;
  int count=0;
  long location_end=0,location_start=-1,file_adress=0;
  FILE *indexpt,*learnpt;
  learn_index_tp learn;

  strcpy(indexfil,INDEXFIL); strcpy(learnfil,LEARNFIL);
  learnpt=fopen(learnfil,readmode);
  if (learnpt==NULL)
    { printf("File %s cannot be read.\n",learnfil); exit(1); }
  indexpt=fopen(indexfil,writemode);
  if (indexpt==NULL)
    { printf("File %s cannot be written.\n",indexfil); exit(1); }
  printf("Starting indexing file '%s'.\n",indexfil);
  while (!feof(learnpt)
	 && fseek(learnpt,file_adress,0)>=0
	 && (number_chars_read=fread(buf,sizeof(*buf),BUFSIZE,learnpt))!=0)
  { buf[number_chars_read]='\0'; location_start=-1;
    while ((bufpt=strchr(bufpt,KEYCHAR))!=0 && bufpt_newline!=NULL)
    { location_end=bufpt-buf;
      if (location_start>=0) /* skip the first time around */
      { strcpy(learn.keyword,key); learn.start=file_adress+location_start;
	learn.size=location_end-location_start; ++count;
	fwrite(&learn,sizeof(learn_index_tp),1,indexpt); /* write record */
      }
/* Scan for new key. Pattern: ... \n */
      if (bufpt!=NULL && (bufpt_newline=strchr(bufpt,NEWLINE))!=NULL)
      { location_start=bufpt_newline+1-buf; /* beginning of next line */
	do --bufpt_newline; while (isspace(*bufpt_newline)); /* erases CR */ 
	*++bufpt_newline='\0' ;
	if (strlen(bufpt+1) >= KEYWORDLEN)
	{ strncpy(key,bufpt+1,KEYWORDLEN-1); key[KEYWORDLEN-1]='\0';
	  printf("Key \"%s\" truncated\n to \"%s\".\n",bufpt+1,key);
	}
	else strcpy(key,bufpt+1);
	bufpt=&buf[location_start];
      }
    }
    file_adress=file_adress+location_end; bufpt_newline=bufpt=buf;
    if (number_chars_read<BUFSIZE) break;
  }
  printf("End of indexing. Final key '%s'.\nWrote %d learn entries.\n"
	,key,count);
  return 0;
}
