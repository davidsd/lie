#include  "lie.h"
#define local  static


local entry* level_vec=NULL;
local object level_vec_group=NULL;

local cmpfn_tp compare; 

local index split_vec(entry* a,index n)
{ entry split_val= *a,* x=a,* end=a+n,* last_high=x;
  while (++x<end) if (*x>split_val) swap(x,++last_high);
  swap(a,last_high); return last_high-a;
}

local index split_mat(entry** m,index n,index c)
{ entry* split_val= *m,** x=m,** end=m+n,** last_high=x;
  while (++x<end) if ((*compare)(*x,split_val,c)>0) swap_rows(x,++last_high);
  swap_rows(m,last_high); return last_high-m;
}

local void heapify_m(matrix* m,index i,index n)
{ index l,len=m->ncols; entry** a=m->elm-1; /* array~$[1:n]$ */
  while ((l=i<<1)<=n)
  { index d= (*compare)(a[i],a[l],len)<0 ? i : l;
    if (l<n && (*compare)(a[d],a[l+1],len)>0) d=l+1;
      /* index of minimum among $\{i,l,l+1\}$ */
    if (d==i) return; /* stop if heap condition was already satisfied */
    swap_rows(&a[i],&a[d]); i=d;
  }
}

local void heapify_p(poly* p,index i,index n)
{ index l,len=p->ncols; entry** a=p->elm-1; bigint** coef=p->coef-1;
  while ((l=i<<1)<=n)
  { index d= (*compare)(a[i],a[l],len)<0 ? i : l;
    if (l<n && (*compare)(a[d],a[l+1],len)>0) d=l+1;
      /* index of minimum among $\{i,l,l+1\}$ */
    if (d==i) return; /* stop if heap condition was already satisfied */
    swap_terms(a,coef,i,d); i=d;
  }
}

local void build_heap_m(matrix* m)
{ index i,n=m->nrows; for (i=n; i>0; i--) heapify_m(m,i,n); }

local void build_heap_p(poly* p)
{ index i,n=p->nrows; for (i=n; i>0; i--) heapify_p(p,i,n); }

local void heap_sort_m(matrix* m, cmpfn_tp criterion)
{ index i=m->nrows; entry** a=m->elm; if (i<2) return;
  compare=set_ordering(criterion,m->ncols,defaultgrp); build_heap_m(m);
  while (swap_rows(a,&a[--i]),i>1) heapify_m(m,1,i);
}

local void heap_sort_p(poly* p, cmpfn_tp criterion)
{ index i=p->nrows; entry** a=p->elm; bigint** coef=p->coef; if (i<2) return;
  compare=set_ordering(criterion,p->ncols,defaultgrp); build_heap_p(p);
  while (swap_terms(a,coef,0,--i),i>1) heapify_p(p,1,i);
}

cmp_tp lex_decr(entry* v,entry * w, index len)
{ while (len-->0) if (*v++!=*w++) return *--v > *--w ? 1 : -1; return 0; }

cmp_tp lex_incr(entry* v,entry * w, index len)
{ while (len-->0) if (*v++!=*w++) return *--v < *--w ? 1 : -1; return 0; }

cmp_tp deg_decr(entry* v,entry * w, index len)
{ index i; entry delta=0;
  for (i=0; i<len; i++) delta += v[i]-w[i];
  if (delta) return delta>0 ? 1 : -1;
  return lex_decr(v,w,len); /* for equal degree, revert to lexicographic */
}

cmp_tp deg_incr(entry* v,entry * w, index len)
{ index i; entry delta=0;
  for (i=0; i<len; i++) delta += v[i]-w[i];
  if (delta) return delta<0 ? 1 : -1;
  return lex_incr(v,w,len); /* for equal degree, revert to lexicographic */
}

cmpfn_tp set_ordering (cmpfn_tp criterion, index n, object g)
{ if (criterion!=height_decr && criterion!=height_incr) return criterion;
  if (g==NULL || n!=Lierank(g))
    return criterion==height_decr ? deg_decr : deg_incr; /* substitute */
  if (level_vec==NULL
     || int_eq_grp_grp(g,level_vec_group)==(object)bool_false)
  { if (level_vec!=NULL) freearr(level_vec);
    level_vec=Lv(g); level_vec_group=g;
  }
  return criterion;
}

cmp_tp height_decr(entry* v,entry * w, index len)
{ index i; entry delta=0;
  assert(level_vec!=NULL && Lierank(level_vec_group)==len);
  for (i=0; i<len; ++i) delta += (v[i]-w[i])*level_vec[i];
  if (delta) return delta>0 ? 1 : -1;
  return lex_decr(v,w,len); /* for equal level, revert to lexicographic */
}

cmp_tp height_incr(entry* v,entry * w, index len)
{ index i; entry delta=0;
  assert(level_vec!=NULL && Lierank(level_vec_group)==len);
  for (i=0; i<len; ++i) delta += (v[i]-w[i])*level_vec[i];
  if (delta) return delta<0 ? 1 : -1;
  return lex_incr(v,w,len); /* for equal level, revert to lexicographic */
}

void swap(entry* x,entry * y) { entry t= *x; *x= *y; *y=t; }
void swap_rows(entry** x,entry** y) { entry* t= *x; *x= *y; *y=t; }
void swap_terms(entry** w,bigint** coef,index i,index j)
{ swap_rows(&w[i],&w[j]);
  if (coef!=NULL) { bigint* t=coef[i]; coef[i]=coef[j]; coef[j]=t; }
}

void sortrow(entry* a, index n)
{ if (n>=3)
  { index i=split_vec(a,n); sortrow(a,i); sortrow(&a[i+1],n-i-1); }
  else if (n==2 && a[0]<a[1]) swap(a,a+1);
}

local void sort_matrix (entry** m,index n,index c)
{ if (n>=3)
  { index i=split_mat(m,n,c);
    sort_matrix(m,i,c); sort_matrix(&m[i+1],n-i-1,c);
  }
  else if (n==2 && (*compare)(m[0],m[1],c)<0) swap_rows(m,m+1);
}

void Qksortmat(matrix* m,cmpfn_tp criterion)
{ compare=set_ordering(criterion,m->ncols,defaultgrp);
  sort_matrix(m->elm,m->nrows,m->ncols);
}

matrix* Unique(matrix* m, cmpfn_tp criterion)
{ index len=m->ncols; register entry** to=m->elm,** from=to,** end=to+m->nrows;
  if (m->nrows<2) return m; heap_sort_m(m,criterion);
  while (!eqrow(*++from,*to,len)) if (++to==end-1) return m;
  while (++from<end) if (!eqrow(*from,*to,len)) swap_rows(++to,from);
  m->nrows=to+1-m->elm; return m;
}

poly* Reduce_pol(poly* p)
{ entry** expon=p->elm; bigint** coef=p->coef; index t=0,f=0,len=p->ncols;
  heap_sort_p(p,cmpfn);
    /* don't exclude cases~$<2$: we must catch $0$-polynomials */
  while (++f<p->nrows)
    if (coef[f]->size==0) clrshared(coef[f]); /* drop term with zero coef */
    else if (eqrow(expon[f],expon[t],len)) /* equal exponents: add coef's */
    { clrshared(coef[t]); clrshared(coef[f]);
      coef[t]=add(coef[t],coef[f]); setshared(coef[t]);
    }
    else /* now term at f replaces one at t as discriminating term */
    { if (coef[t]->size) t++; else clrshared(coef[t]); /* keep if nonzero */
      swap_terms(expon,coef,t,f); /* move term, preserve row separateness */
    }
  if (p->nrows!=0)
    /* |p| mights have no terms at all (e.g. from |alt_dom|). */
    if (coef[t]->size) t++; else clrshared(coef[t]); /* handle final term */
  else *coef=copybigint(null,NULL); /* safer not to introduce aliasing */
  if ((p->nrows=t)==0) /* then must keep last term; coef is cleared */
  { index i; p->nrows=1; setshared(*coef); /* |*coef| was |0| but not shared */
    for (i=0; i<len; i++) expon[0][i]=0; /* clear first exponent as well */
  }
  setsorted(p); return p;
}

index searchterm(poly* p, entry* t)
{ index l=0, u, len=p->ncols; entry** expon;
  cmpfn_tp cmp=set_ordering(cmpfn,len,defaultgrp);
  if (!issorted(p)) { p=Reduce_pol(p); }
  u=p->nrows; expon=p->elm;
  while (u-l>1)
  { index m=(u+l)/2; cmp_tp c=(*cmp)(expon[m],t,len);
    if (c<0) u=m;  else if (c>0) l=m+1;  else return m;
  }
  return  l<u && eqrow(expon[l],t,len) ? l : -1;
}


