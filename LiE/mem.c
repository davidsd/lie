#include  "lie.h"
#define hash(p) ((unsigned long)(p)%hash_mod)
#define set_common_fields(x,t) ((x)->type=t,(x)->nref=0)
#define EXTBIG  16 \
  


long chunks = 0; /* number of objects currently allocated */
/*static void **ptr;*/ /* now globally defined */
void **ptr;
unsigned long maxptrs=0; /* initialised elsewhere */

static boolean *marked;
unsigned long gccrit;

static unsigned long hash_mod; 

static simpgrp* simpgrplist=NULL;

/* <lie-py> */
static unsigned int *pyobj;

void setpyobj(object obj) {
  long i;
  i = findaddr(obj);
  pyobj[i]++;
}

void clrpyobj(object obj) {
  long i;
  i = findaddr(obj);
  if(!pyobj[i]) {
    gc_print_obj(obj);
    fatal("pyobj underflow!");
  }
  pyobj[i]--;
}

void markpyobj(void) {
  int i;
  for (i=0; i<maxptrs; i++) {
    if (ptr[i]!=NULL && pyobj[i])
      mark(ptr[i]);
  }
}
/* </lie-py> */

void initmem(void)
{ long i;
  ptr = (void**) malloc(sizeof(void*) * maxptrs);
  if (ptr==NULL) fatal("Insufficient memory to allocate object table.\n");
  marked = (boolean*) malloc(sizeof(boolean) * maxptrs);
  if (marked==NULL) fatal("Insufficient memory to allocate mark table.\n");
  /* <lie-py> */
  pyobj = (unsigned int*) malloc(sizeof(unsigned int) * maxptrs);
  if (pyobj==NULL) fatal("Insufficient memory to allocate pyobj table.\n");
  /* </lie-py> */
  for (i=0; i<maxptrs; i++) { ptr[i]=NULL; marked[i]=false;
    /* <lie-py> */ pyobj[i]=0; /* </lie-py> */ }
  gccrit=maxptrs-GCCRIT;
  hash_mod = (maxptrs-2)|0x1; /* slightly less than |maxptrs|, and odd */
}

void newmem(long newval)
{ unsigned long maxptrs0=maxptrs;
  void** ptr0=ptr; boolean* marked0=marked; /* handles for old values */
  /* <lie-py> */
  unsigned int* pyobj0=pyobj;
  /* </lie-py> */
  if ((maxptrs=newval)<=GCCRIT)
  
  { maxptrs=maxptrs0;
    error("You can't lower maxobjects from %ld to %ld.\n"
         ,(long)maxptrs,(long)newval);
  }
  initmem();
  { long k;
    for (k=0; k<maxptrs0; k++)
      if (ptr0[k]!=NULL) /* copy all non-null pointers */
      { long h;  
                 { long i; h=hash(ptr0[k]);
                   for (i=0; i<maxptrs; i++) /* find an empty slot; try |maxptrs| times */
                     if (ptr[h]==NULL) break; /* found */
                     else if (++h>=maxptrs) h=0; /* try next slot, wrapping around */
                   if (i==maxptrs)
                   { free(ptr); ptr=ptr0; free(marked); marked=marked0;
                     maxptrs=maxptrs0; /* reset to old values */
                     error("You currently cannot decrease 'maxobjects' below %ld.\n"
                          ,chunks);
                   }
                 }
        ptr[h]=ptr0[k]; /* copy pointer to empty slot */
        marked[h]=false; /* make new pointer unmarked */
        /* <lie-py> */
        if (pyobj0[k]) {
          pyobj[h]=pyobj0[k];
        }
        /* </lie-py> */
      }
  }
  if (!redirected_input)
    Printf("New object table of size %ld.\n",(long)maxptrs);
  free(ptr0); free(marked0); /* release the old tables */
  /* <lie-py> */
  free(pyobj0);
  /* </lie-py> */
}

long findaddr0(void* p)
{ if (p==NULL) return -1;
  
  { long i, h = hash(p);
    for (i=0; i<maxptrs; i++)
      if (ptr[h]==p) return h;  else if (++h>=maxptrs) h=0;
  }
  return -1;
}

long findaddr(void* p)
{ if (p!=NULL)
  
  { long i, h = hash(p);
    for (i=0; i<maxptrs; i++)
      if (ptr[h]==p) return h;  else if (++h>=maxptrs) h=0;
  }
     /* try to give a description of this stranger */

  c_print_objs();
  gc_print_obj((object) p);
  fatal(" findaddr: called with unknown address %p %s\n", p, type_tag(p));
  return -1;
}

static struct { objtype num; char str[4];} type_name [] =
{ { UNKNOWN, "unk" }
, { TEKST, "tex" }
, { INTEGER, "int"  }
, { VECTOR, "vec" }
, { MATRIX, "mat" }
, { BIGINT, "bin" }
, { POLY, "pol" }
, { GROUP, "grp" }
, { VOID, "vid" }
, { ARGTYPE, "arg" }
, { GRPDFT, "(g)" }
};

objtype type_code(char* name)
{ int i;
  for (i=0; i<array_size(type_name); ++i)
    if (strcmp(name,type_name[i].str)==0) return type_name[i].num;
  return ERROR;
}

char* code_obj_type(objtype t)
{ int i;
  for (i=0; i<array_size(type_name); ++i)
    if (t==type_name[i].num) return type_name[i].str;
  return NULL;
}

boolean real_type(objtype type)
{ return type==INTEGER || type==VECTOR || type==MATRIX
      || type==GROUP || type==BIGINT || type==TEKST || type==POLY;
}

void* safe_alloc(size_t size)
{ void* result = malloc(size);
  if (result==NULL)
    error("Can't allocate %lu more bytes\n", (unsigned long)size);
  return result;
}

void* allocmem(size_t size)
{ void* result = safe_alloc(size);
  { long h; 
    
    { long i;
      h=hash(result);
      for (i=0; i<maxptrs; i++)
        if (ptr[h]==NULL) break;  else if (++h>=maxptrs) h=0;
      if (i==maxptrs) {
        /*error("Object table overflow (%ld). Try increasing 'maxobjects'.\n"
          ,chunks); */
        /* we're just going to dynamically grow the object table */
        newmem(3*maxptrs/2); /* increase maxptrs by 50% */
        free(result);
        return allocmem(size); /* safe to call again now */
      }
    }
    ignore_intr(); /* don't interrupt while updating |ptr| */
    ptr[h]=result; ++chunks; allow_intr();
  }
  return result;
}

void freem(void* addr)
{ long i=findaddr(addr); /* locate the pointer; it should be present */
  ignore_intr();
  ptr[i]=NULL; /* remove pointer from the table */
  --chunks;
  free(addr); /* the actual release of the memory */
  allow_intr();
}

void freep(poly* addr)
{ index j;
  for (j=0; j<addr->nrows; j++)
  { object c=(object) addr->coef[j];
    assert(isshared(c)); clrshared(c); freemem(c);
  }
  freemem(addr);
}

entry* mkintarray(index n)
{ if (n>max_obj_size/sizeof(entry))
    error("Cannot create internal array of %ld entries", (long)n);
  return alloc_array(entry,n);
}

intcel* (mkintcel)(entry n  with_line_and_file)
{
    intcel *i;
    i = (intcel*) allocmem(sizeof(intcel));
    set_common_fields(i,INTEGER);
    i->intval = n;
    return i;
}

bigint* (mkbigint)(long size  with_line_and_file)
{
  bigint *result;
  if (size>SHRT_MAX) error("Big integer too big\n");
  result = (bigint*)allocmem(sizeof(bigint)+size*sizeof(digit));
  set_common_fields(result,BIGINT);
  result->allocsize = result->size = size;
  result->data = (digit *)&result[1];
  return result;
}

bigint* copybigint(bigint* from, bigint* to)
{
  int n = abs(from->size); digit *f, *t;
  if (to==NULL) to = mkbigint(n);
  else if (to->allocsize<n) { freemem(to); to=mkbigint(n); }
  to->size = from->size;
  for (f=from->data, t=to->data; n>0; --n) *t++=*f++;
  return to;
}

bigint *extendbigint(bigint* old)
{ bigint *new;
  if (old->allocsize>SHRT_MAX-EXTBIG) error("Big integer too big\n");
  new = mkbigint(old->allocsize + EXTBIG);
  copybigint(old, new); freemem(old); return new;
}

vector* (mkvector)(index n  with_line_and_file)
{
    vector *v;
    if (n > (max_obj_size-sizeof(vector))/sizeof(entry))
	error("Cannot handle a vector with %ld entries.\n",(long)n);
    v=(vector*)allocmem(sizeof(vector)+n*sizeof(entry));
    set_common_fields(v,VECTOR);
    v->size = v->ncomp = n; v->compon = (entry*) &v[1];
    return v;
}

vector* copyvector(vector *src)
{ vector* result=mkvector(src->ncomp);
  copyrow(src->compon,result->compon,src->ncomp);
  return result;
}

matrix* (mkmatrix)(index r,index c  with_line_and_file)
{
  index i;
  matrix *m;
  size_t size=sizeof(matrix)
    + (r==0 ? 1 : r)*(sizeof(bigint*)+sizeof(entry*))
    + r*c*sizeof(entry);

  if (size > max_obj_size)
      error("Cannot handle a %ld by %ld matrix\n",(long)r,(long)c);
  m = (matrix*)allocmem(size);
  set_common_fields(m,MATRIX);
m->rowsize = m->nrows = r; m->ncols = c;
  m->null = (bigint**) NULL;

  m->elm=(entry**) &m[1]; /* start of row pointer block */
  m->elm[0]=(entry*) &m->elm[r]; /* start of entry block */
  for (i=1; i<r; ++i) m->elm[i]=&m->elm[i-1][c]; /* remaining row pointers */
  return m;
}

poly* (mkpoly)(index r,index c  with_line_and_file)
{
  index i;
  poly  *p; size_t size;
  boolean  is_null_poly = false;

  if (r == 0) { is_null_poly = true; r = 1; } /* avoid empty polynomial */

  size=sizeof(matrix)
    + r*(sizeof(bigint*)+sizeof(entry*)+c*sizeof(entry));

  if (size > max_obj_size)
      error("Cannot handle a %ld by %ld polynomial\n",(long)r,(long)c);
  p = (poly*)allocmem(size);
  set_common_fields(p,POLY);
p->rowsize = p->nrows = r; p->ncols = c;
  p->elm=(entry**) &p[1]; /* start of row pointer block */
  p->elm[0]=(entry*) &p->elm[r]; /* start of entry block */
  p->coef = (bigint**) &p->elm[0][r*c]; /* start of coefficient block */
  for (i=1; i<r; ++i)
  { p->elm[i]=&p->elm[i-1][c];
    p->coef[i]=NULL;
  }
  if (is_null_poly)
  { p->coef[0] = null;
      for (i=0; i<c;i++) p->elm[0][i] = 0;
  }
  return p;
}

matrix* copymatrix(matrix* old)
{ index i; matrix* new = mkmatrix(old->nrows,old->ncols);
  for (i=0; i<old->nrows; ++i) copyrow(old->elm[i],new->elm[i],old->ncols);
  return new;
}

poly* copypoly(poly* old)
{ index i; poly* new = mkpoly(old->nrows,old->ncols);
  for (i=0; i<old->nrows; ++i)
  { new->coef[i]=old->coef[i], setshared(new->coef[i]);
    copyrow(old->elm[i],new->elm[i],old->ncols);
  }
  return new;
}

matrix* extendmat(matrix* old)
{ index i;
  matrix* new= mkmatrix(3*old->rowsize/2+1, old->ncols);
  for (i=0; i<old->nrows; ++i) copyrow(old->elm[i],new->elm[i],old->ncols);
  new->nrows=old->nrows;
  freemem(old); return new;
}

poly* extendpoly(poly* old)
{ index i; poly* new=mkpoly(3*old->rowsize/2+1,old->ncols);
  for (i=0; i<old->nrows; ++i)
  { new->coef[i]=old->coef[i],setshared(new->coef[i]);
    copyrow(old->elm[i],new->elm[i],old->ncols);
  }
  new->nrows=old->nrows;
  freepol(old); return new;
}

simpgrp* (mksimpgrp)(char type, index rank  with_line_and_file)
{
  simpgrp *grp, **loc;
  
  { for (loc=&simpgrplist; *loc!=NULL; loc=&(*loc)->nextgrp)
      if ((*loc)->lietype==type && (*loc)->lierank==rank) return *loc;
  }
  grp = (simpgrp *) allocmem(sizeof(simpgrp));
  set_common_fields(grp,SIMPGRP);
  setlonglife(grp); /* simple group will not be garbage collected */
  grp->lietype = type;
  grp->lierank = rank;
  grp->exponents = grp->level = grp->root_norm = NULL;
  grp->cartan = grp->icartan = grp->roots = NULL;
  grp->nextgrp = NULL; /* this group will be last in |simpgrplist| */
  return *loc = grp; /* add group to end of |simpgrplist| and return it */
}

group* (mkgroup)(index ncomp  with_line_and_file)
{
    group *grp;
    grp = (group*) allocmem(sizeof(group)+ncomp*sizeof(simpgrp*));
    set_common_fields(grp,GROUP);
    grp->toraldim = 0;
    grp->ncomp = ncomp;
    grp->liecomp = (simpgrp**)(&grp[1]);
    return grp;
}

tekst* (mktekst)(index n  with_line_and_file)
{
  tekst *t;
  t = (tekst*)allocmem(sizeof(tekst)+n+1);
  set_common_fields(t,TEKST);
  t->len = n;
  t->string = (char *) (&t[1]);
  t->string[n] = '\0';
  return t;
}

tekst* copytekst(tekst* o)
{
  index n = o->len;
  tekst *result = mktekst(n);
  strncpy(result->string,o->string,n);
  return result;
}

object mkobject(symblst s)
{ return s->class == BLOCK ? s->data.val : cpobject(s->data.val); }

object cpobject(object o)
{ switch (type_of(o))
  {
  case INTEGER:return (object)mkintcel(o->i.intval);
  case BIGINT: return (object)copybigint(&o->b,NULL);
  case VECTOR: return (object)copyvector(&o->v);
  case MATRIX: return (object)copymatrix(&o->m);
  case POLY:   return (object)copypoly(&o->pl);
  case GROUP:  
               { group* g=&o->g; index i,n=g->ncomp;
                 group* result=mkgroup(n);
                 result->toraldim=g->toraldim;
                 for (i=0; i<n; i++) result->liecomp[i] = g->liecomp[i];
                 return (object) result;
               }
  case TEKST:  return (object)copytekst(&o->t);
  default: error("Illegal result type %d\n",type_of(o)); return NULL;
  }
}

void mark_expression(symblst s,int n)
{ int i;
  for (i = 0; i < n; i++)
    if (s[i].class == VALUE) mark(s[i].data.val);
}

static void mark0(void* addr)
{ if (addr!=NULL) marked[findaddr(addr)] = true; }

static boolean markobj(object obj) /* true if it was already marked */
{ boolean was_marked = false;
  long i;
  if (obj==NULL || islonglife(obj)) return true;
  
  { i=findaddr(obj);
    if (type_of(obj)==POLY)
    { long k;
      for (k=0; k< obj->pl.nrows; k++) markobj((object) (obj->pl.coef[k]));
    }
  }
  if (marked[i]) was_marked=true,++obj->i.nref ;
  else marked[i]=true,obj->i.nref=1;
  return was_marked;
}

void mark(object obj)
{ if (markobj(obj)) return;
  switch (type_of(obj))
  { case SIMPGRP:
    markobj((object)obj->s.cartan);
    markobj((object)obj->s.icartan);
    markobj((object)obj->s.roots);
    markobj((object)obj->s.exponents);
    markobj((object)obj->s.level);
    markobj((object)obj->s.root_norm);
  break;
    case GROUP:
    { int i;
      for (i=0; i<obj->g.ncomp; ++i) mark((object)Liecomp(obj,i));
    }
  }
}

extern symblst topsym;

void gc(void)
{
  long   i;
  strtype name0 = label->name;
  
  { symblst     v;
    symblst last_v=top_definitions;
    label->name = match("garbage_collection",false); /* indicate current activity */
    mark(defaultgrp);
    mark_defaultgrp_stack();
    mark((object) topsym);
    for (v=top_definitions; v!=NULL; v=v->next)
    { mark0(v);
      if (v->class==VALUE) mark(v->data.val);
      else if (v->class == FUNCTION)
      { funclst f = v->data.func;
        mark0(f);
        mark_expression(f->start_nodes, f->n_nodes);
  	/* mark stored function body */
      }
      last_v=v;
    }
    mark_tree();
    /* <lie-py> */
    markpyobj();
    /* <lie-py> */
  }
  for (i = 0; i<maxptrs; i++)
    if (marked[i]) marked[i] = false;
    else if (ptr[i]!=NULL && !islonglife(ptr[i])) freem(ptr[i]);
  label->name = name0; /* restore value */
}

void for_all_objects(void (*f)(object))
{ long i;
  for (i=0; i<maxptrs; i++)
    if (ptr[i]!=NULL) f(ptr[i]);
}

void printobjectinfo(object obj) {}

void share_error(object o)
{
  fatal("Reference count underflow");
}

