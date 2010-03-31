#include "lie.h"
/* Is concerned with storing and restoring binary matrices */

#define SUFFIX ".a"

typedef struct {
	unsigned long size;
	char name[KEYWORDLEN], dirname[KEYWORDLEN]; 
    } header_type;

static matrix *read_format_mat(matrix* store)
{
    entry **ptr;
    entry *elm;
    boolean okay = (type_of(store) == MATRIX);
    int r = store->nrows, c = store->ncols;
    if (!okay) error("File has not the right (matrix) format.\n");
    store->elm =  ptr = (entry**) (store + 1);
    elm = (entry *) (ptr + r);
    for (; r; r--, ptr++, elm += c)
	*ptr = elm;
    return copymatrix(store);
}


static tekst *read_format_string(tekst *store)
{
    boolean okay = (type_of(store) == TEKST);
    if (!okay) error("File has not the right (string) format.\n");
    store->string = (char*) (store + 1);
    return copytekst(store);
}

static void error_file(char* task,char* filename)
{
    error("Error %s file %s.\n",task,filename);
}

static unsigned long matlen(unsigned long lr,unsigned long lc)
{
    return sizeof(matrix) + (lr?lr:1L) * sizeof(entry*)
    + lr * lc * sizeof(entry)
    + (lr?lr:1L) * sizeof(bigint*);
}

static header_type *pack_object
  (object m,char* dirname, char* name, unsigned long* size)
{
    unsigned long n = type_of(m) == MATRIX?
	matlen((unsigned long) m->m.nrows, m->m.ncols):
	sizeof(tekst) + m->t.len + 1;
    header_type* result = (header_type*)malloc(sizeof(header_type)+n);
    memcpy(&result[1],m, n); /* copy |m| after header */
    result->size = n;
    strcpy(result->dirname,dirname);
    strcpy(result->name,name);
    *size = n;
    return result;
}

void Objectwrite(object m,group* g,char* t)
{
    unsigned long size; char* s=grp2str(g);
    header_type *store = pack_object(m,s,t,&size);
    FILE *fp;
    char filename[LABELLENGTH];
    strcpy(filename,infofil);
    strcat(filename,SUFFIX);
    fp = fopen(filename,"a");
    if (fp==NULL) error_file("opening",filename);
    if (fwrite(store, size+sizeof(header_type), 1, fp)==0)
	error_file("writing",filename);
    fclose(fp);
    freem(s); free(store);
}

object Objectread(group *g, char* name)
{
    object result,store;
    header_type header;
    char* dirname = grp2str(g);
    char filename[LABELLENGTH];
    FILE *fp;
    strcpy(filename, infofil);
    strcat(filename,SUFFIX);
    fp = fopen(filename,"r");
    if (fp==NULL) error_file("opening",filename);
    do /* search name and dirname */
    { if (fread(&header, sizeof(header_type), 1 ,fp)==0)
	if (feof(fp)) { fclose(fp); return (object) NULL;}
	else error_file("reading",filename);
      if (strcmp(header.name,name)==0 && strcmp(header.dirname,dirname)==0)
	break; 
      if (fseek(fp,header.size,SEEK_CUR)!=0) /* advance past item */
	error("Failed to find %s/%s in file %s.\n",dirname,name,filename);
    } while (true);
    store = (object) safe_alloc(header.size);
    if (fread((char*) store, 1, header.size, fp)==0)
	    error_file("reading",filename);
    result = (type_of(store)== MATRIX) ?
	(object) read_format_mat((matrix*) store):
	(object) read_format_string((tekst*) store);
    freem(dirname); free(store);
    fclose(fp);
    return result;
}

void Objectsave(object m, char* t)
{
    unsigned long size;
    header_type *store = pack_object( m, "",t,&size);
    FILE *fp;
    fp = fopen(t,"w");
    if (fp==NULL) error_file("opening",t);
    if (fwrite(store, size+sizeof(header_type), 1, fp)==0) 
	error_file("writing",t);
    fclose(fp);
    free(store);
}

object Objectget(char* name)
{
    object result,store;
    header_type header;
    FILE *fp;
    fp = fopen(name,"r");
    if (fp==NULL) error_file("opening",name);

    if (fread(&header, sizeof(header_type), 1 ,fp)==0)
	error_file("reading header of",name);

    if ((store = (object) malloc(header.size))==NULL) 
	error("No memory available");

    if (fread((char*) store, 1, header.size, fp)==0) 
	error_file("reading",name);

    result = (type_of(store)== MATRIX) ?
	(object) read_format_mat((matrix*) store):
	(object)read_format_string((tekst*) store);

    free(store);
    fclose(fp);
    return result;
}
