

#define  TYPEMASK 0xFF
#define  type_of(obj) (((object)(obj))->any.type & TYPEMASK)
#define  type_tag(obj) (code_obj_type(type_of(obj)))

#define  SORTED    0x800   /* whether polynomial or matrix is sorted */
#define  issorted(obj)   ((((object)(obj))->any.type & SORTED)!=0)
#define  setsorted(obj)  (((object)(obj))->any.type |= SORTED)
#define  clrsorted(obj)  (((object)(obj))->any.type &= ~SORTED)

#define  LONGLIFE USHRT_MAX /* ref count that marks value as persistent */

#define  refcount(x)   ((object)(x))->any.nref
#define  isshared(x)   ((x)==NULL ? 0 : refcount(x))
#define  setshared(x)  ((x)==NULL || refcount(x)==LONGLIFE ? 0 : ++refcount(x))
#define  clrshared(x)  ((x)==NULL || refcount(x)==LONGLIFE ? 0 : \
                  refcount(x)==0 ? share_error((object)(x)) : --refcount(x))

#define  setlonglife(x) (refcount(x)=LONGLIFE)
#define  islonglife(x)  ((x)!=NULL && refcount(x)==LONGLIFE)

#define  alloc_array(type_arg,size) \
  ((type_arg*)safe_alloc((size)*sizeof(type_arg)))
#define  freearr(addr) (free(addr))

#define  freemem(addr) (refcount(addr)==0 ? freem(addr),0 : 0)
#define  freepol(addr) (refcount(addr)==0 ? freep(addr),0 : 0)

#define  with_line_and_file 

#define  Liecomp(grp,i) ((grp)->g.liecomp[i])

void initmem(void);

void newmem(long newval);

long findaddr0(void* p);
long findaddr(void* p);

objtype type_code(char* name);
char* code_obj_type(objtype t);
boolean real_type(objtype type);

void* safe_alloc(size_t size);
void* allocmem(size_t size);
void freem(void* addr);

void freep(poly* addr);

entry* mkintarray(index n);

intcel* (mkintcel)(entry n  with_line_and_file);
bigint *(mkbigint)(long size  with_line_and_file);

bigint* copybigint(bigint* from, bigint* to);
bigint* extendbigint(bigint* old);

vector* (mkvector)(index n  with_line_and_file);
vector* copyvector(vector *src);

matrix* (mkmatrix)(index r,index c  with_line_and_file);
poly* (mkpoly)(index r,index c  with_line_and_file);

matrix* copymatrix(matrix* old);
poly* copypoly(poly* old);
matrix* extendmat(matrix* old);
poly* extendpoly(poly* old);

simpgrp* (mksimpgrp)(char type, index rank  with_line_and_file);

group* (mkgroup)(index ncomp  with_line_and_file);

tekst* (mktekst)(index n  with_line_and_file);

tekst* copytekst(tekst* o);

object mkobject(symblst s);
object cpobject(object o);

void mark_expression(symblst s,int n);

void mark(object obj);

void gc(void);

void for_all_objects(void (*f)(object));

void printobjectinfo(object obj);
