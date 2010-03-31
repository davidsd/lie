#include "lie.h"

#define NRECS 80 /* Just enough for LiE 2.0 */
#define NCOLS 3

static char *directory_table[REPORT_LEN];
static int directory_pt = 0;

extern char *getenv();

/* Learn reads the indexfile belonging to the learnfile,
   searching it for keywords containing the given name as
   substring. For each match, the corresponding part of the
   learn file is copied to the result string.
*/

static void add_directory_table(char* name)
{   int i = 0, j = directory_pt;
    if (directory_pt >= REPORT_LEN) error("Learn index too large.\n");
    while (i < directory_pt && strcmp(directory_table[i], name) < 0)
	i++;
    if (i < directory_pt && !strcmp(directory_table[i], name))
	return;
    while (j > i) {
	directory_table[j] = directory_table[j - 1];
	j--;
    }
    if (!(*name)) error("Null name %d\n",i);
    directory_table[i] = name;
    directory_pt++;
}

static void printdirectory(void)
{
    int i;
    for (i = 0; i < directory_pt - 1; i++) {
	if(strlen(directory_table[i]) > KEYWORDLEN)
	  error("%s too long.\n",directory_table[i]);
	if (i % NCOLS == 0) Printf("\n");
	Printf("%-24.24s ", directory_table[i]);
    }
    Printf("\n");
}

boolean substring(key,name) char* name,* key;
{ char* keypt;
  if (!*name) return false;
  keypt = strchr(key,name[0]);
  while (keypt && strncmp(keypt,name,strlen(name)))
    keypt = strchr(keypt+1,name[0]);
  return keypt!=NULL;
}

void build_directory(learn,nitems) learn_index_tp* learn; long nitems;
{
    long i=0;
    for (i=0;i<nitems;i++) {
	if (!is_operator(learn[i].keyword)){
	    add_directory_table(learn[i].keyword);
	}
    }
}

/*
static learn_index_tp learn[NRECS];
*/

boolean Learn(name) char* name;
{
    FILE *indexpt,*learnpt;
    static long nitems;
    entry i,totalsize=0;

    short foundlearn[NRECS]; /* indexes of learn which satisfy search
			      criterium */
    short foundpt = 0;
    char *result;
    char *resultpt;

    char tmpfile[L_tmpnam];

    char *t = name;

    static learn_index_tp  *learn; 
    if (!learn)
	learn = (learn_index_tp*)
	malloc((unsigned long) (sizeof(learn_index_tp) * NRECS));
    if (!learn) error("No memory available.\n");

/***************************************************************
*  open index file					       *
***************************************************************/

    if (!*learnfil || !*learnind)
    error("Learn command is not implemented.\n");
    indexpt = fopen(learnind,readmode);
    if (!indexpt) error("File %s cannot be opened.\n",learnind);

/***************************************************************
*  read index						       *
***************************************************************/

    if (!nitems)
	nitems = fread(learn,sizeof(learn_index_tp),NRECS,indexpt);
    fclose(indexpt);
    if (nitems >= NRECS) error("Indexfile too large.\n");
    if (*name=='\0' || strcmp(name,"index")==0) {
	if (directory_pt == 0) build_directory(learn,nitems);
	printdirectory();
	return true;
    }


/***************************************************************
*  Make uppercase                                              *
***************************************************************/

    while (*t) {
	*t = isupper(*t)? tolower(*t): *t;
	t++;
    }

/***************************************************************
*  Search						       *
***************************************************************/

    foundpt = 0;
    for (i=0;i<nitems;i++) {
	if (substring(learn[i].keyword,name)) foundlearn[foundpt++] = i;
    }
    if (!foundpt) /* error("Info about %s is not available.\n",name)*/
	return false;

    tmpnam(tmpfile);
    cur_out = fopen(tmpfile, "w");
    if (cur_out==NULL) cur_out = stdout;

/***************************************************************
*  Compute size output string				       *
***************************************************************/

    for (i=0; i<foundpt; i++) 
	totalsize += (long) learn[foundlearn[i]].size;
    result = (char*) malloc(totalsize+foundpt*(KEYWORDLEN+2)+sizeof(char));
    if (!result) error("No memory available.\n");

/***************************************************************
*  Fill text string					       *
***************************************************************/

    resultpt = result;
    learnpt = fopen(learnfil,readmode);
    if (!learnpt) error("File %s cannot be opened.\n",learnfil);
    for (i=0;i<foundpt;i++) {
    /*
	sprintf(resultpt,"%s:\n",
	learn[foundlearn[i]].keyword);
	resultpt = resultpt +
	(long) strlen(learn[foundlearn[i]].keyword)+ (long) 2;
    */
	fseek(learnpt,learn[foundlearn[i]].start,0);
	fread(resultpt,sizeof(char),
	learn[foundlearn[i]].size,learnpt);
	resultpt = resultpt + (long) learn[foundlearn[i]].size;
    }
    *resultpt= '\0';
    Printf("%s\n",result);
    free(result);
    fclose(learnpt);
    if (cur_out == stdout) return true;

    fclose(cur_out);
    invoke_prog(pager,tmpfile);
    remove(tmpfile);
    cur_out = stdout;
    return true;
}

entry exec_learn(char *name)
{
    FILE *indexpt;
    short nitems,i;
    char *t = name;

    learn_index_tp learn[NRECS];

    /* opening of the files */
    if (!*learnfil || !*learnind)
    error("Exec command is not implemented.\n");
    indexpt = fopen(learnind,readmode);
    if (!indexpt)
	error("File %s cannot be opened.\n",learnind);
    /* end open files */
    rewind(indexpt);
    nitems = fread(learn,sizeof(learn_index_tp),NRECS,indexpt);
    fclose(indexpt);
    if (nitems > NRECS) error("Indexfile too large.\n");
    i = 0;
    /* printf("learn:%s name: %s\n",learn[i].keyword,name); */
    /* Convert name to lowercase */
    while (*t) {
	*t = isupper(*t)? tolower(*t): *t;
	t++;
    }
    /* Lineair search */
    while(i < NRECS && strcmp(learn[i].keyword,name)) i++;
    if (i==NRECS) return -1; /* Not found */
    return learn[i].start;
}
