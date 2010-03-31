#include "lie.h"

/* copy array of entry's of length n */
void copyrow(entry* from,entry* to,index n)
{ entry* lim=to+n; while (to<lim) *to++= *from++; }

/* compare arrays of entry's of length n */
boolean eqrow(v,w,n) register entry* v,* w; index n;
{entry* lim=w+n; while (w<lim) if (*w++!= *v++) return(false); return(true); }

/* add multiple of array to array of entry's of length n: v+=f*w */
void add_xrow_to(v,f,w,n) register entry* v,f,* w; index n;
{entry* lim=w+n; while (w<lim) *v++ += f*(*w++); }

/* add arrays of entry's of length n: x=v+w */
void addrow(v,w,x,n) register entry* v,* w,* x; index n;
{entry* lim=x+n; while (x<lim) *x++ = *v++ + *w++; }

/* subtract arrays of entry's of length n: x=v-w */
void subrow(v,w,x,n) register entry* v,* w,* x; index n;
{entry* lim=x+n; while (x<lim) *x++ = *v++ - *w++; }

/* subtract arrays of entry's of length n: x=v-w, but only if positive result */
boolean pos_subrow(v,w,x,n) register entry* v,* w,* x; index n;
{ entry* lim=x+n;
  while (x<lim) if((*x++ = *v++ - *w++)<0) return false; return true;
}

/* inner product */
entry inprow(v,w,n) register entry* v,* w; index n;
{entry* lim=w+n,sum=0; while (w<lim) sum+=(*w++)*(*v++); return sum;}

matrix *Transpose(m) matrix* m;
{ register entry i,j,** elm,** telm; matrix* t = mkmatrix(m->ncols, m->nrows);
  elm = m->elm; telm = t->elm;
  for (i=0; i<m->nrows; i++)
  { entry* mijptr= *elm++; for (j=0; j<m->ncols; j++) telm[j][i] = *mijptr++; }
  return t;
}

void mulmatmatelm(entry** a,entry** b,entry** c, index l, index m, index n)
{ register index j; index i, k; entry sum,* cikptr; register entry* aijptr;
  for (i = 0; i<l; i++)
  { cikptr= *c++;
    for (k=0; k<n; k++)
    { sum=0; aijptr=a[i];
      for (j=0; j<m; j++) sum += (*aijptr++)*b[j][k];
      *cikptr++ = sum;
    }
  }
}

void mulvecmatelm(entry* v,entry** b,entry* w, index m, index n)
{ register index i; index j; entry sum; register entry* viptr;
  for (j=0; j<n; j++)
  { sum=0; viptr=v;
    for (i=0; i<m; i++) sum += (*viptr++)*b[i][j];
    *w++ = sum;
  }
}

void mulmatvecelm(entry** a,entry* v,entry* w, index m, index n)
{ register index j; index i; entry sum; register entry* aijptr,* vjptr;
  for (i=0; i<m; i++)
  { sum=0; vjptr=v; aijptr= *a++;
    for (j=0; j<n; j++) sum += (*aijptr++)*(*vjptr++);
    *w++ = sum;
  }
}

matrix* Matmult(a, b) matrix* a,* b; /* multiply matrices */
{ matrix* m;
  if (a->ncols!=b->nrows) error ("Product of incompatible matrices\n");
  m = mkmatrix(a->nrows, b->ncols);
  mulmatmatelm(a->elm,b->elm,m->elm,a->nrows,a->ncols,b->ncols);
  return m;
}

matrix* Blockmat(a,b) matrix* a,* b;
{ index i=a->nrows,j,k=a->ncols,l=b->ncols;
  matrix* c=mkmatrix(i+b->nrows,k+l); entry** p=c->elm,** q=a->elm,* pi,* qi;
  while (i-->0)
  { pi= *p++; qi= *q++;
    j=k; while (j-->0) *pi++= *qi++; j=l; while (j-->0) *pi++=0;
  }
  q=b->elm; i=b->nrows;
  while (i-->0)
  { pi= *p++; qi= *q++;
    j=k; while (j-->0) *pi++=0; j=l; while (j-->0) *pi++= *qi++;
  }
  return c;
}

void printarr(a,n) entry* a; index n;
{ Printf("[");
  if (n>0) while(Printf("%ld",(long)(*a++)),--n>0) Printf(",");
  Printf("]");
}
