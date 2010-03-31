#include "lie.h"

#ifdef __STDC__
static void simp_contragr(entry* y,entry* x,simpgrp* g);
#endif

/* Find the contragredient representation of y; store in x */
static void simp_contragr(y,x,g) entry* y,* x; simpgrp* g;
{ register index i; index r=g->lierank;
  switch (g->lietype)
  {	 case 'A': for (i=0; i<r; i++) x[i]=y[r-i-1]; /* An: reverse */
  break; case 'D':
    if (r%2) { copyrow(y,x,r-2); x[r-2]=y[r-1]; x[r-1]=y[r-2]; } /* D-odd */
    else copyrow(y,x,r);
  break; case 'E':
    if (r==6)
    { *x++=y[5]; *x++=y[1]; *x++=y[4]; *x++=y[3]; *x++=y[2]; *x++=y[0]; } /* E6 */
    else copyrow(y,x,r);
  break; default: copyrow(y,x,r);
  }
}

vector* Contragr(v,grp) vector* v; object grp;
{ vector* contr=mkvector(v->ncomp); entry* y=v->compon,* x=contr->compon;
  if (type_of(grp)==SIMPGRP) simp_contragr(y,x,&grp->s);
  else if (simpgroup(grp)) simp_contragr(y,x,Liecomp(grp,0));
  else
  { index i,n=grp->g.ncomp,d;
    for (i=0; i<n; i++,x+=d,y+=d)
    { simpgrp* g=Liecomp(grp,i); d=g->lierank; simp_contragr(y,x,g); }
    for (i=0; i<grp->g.toraldim; i++) *x++= -*y++; /* negate torus part */
  }
  return contr;
}

poly* Contragr_p(p,grp) poly* p; object grp;
{ poly* contr=mkpoly(p->nrows,p->ncols); index i;
  bigint** c=p->coef,** cc=contr->coef; 
  for (i=0; i<p->nrows; i++)
  { entry* y=p->elm[i],* x=contr->elm[i];
    setshared(*c); *cc++ = *c++;
    if (type_of(grp)==SIMPGRP) simp_contragr(y,x,&grp->s);
    else if (simpgroup(grp)) simp_contragr(y,x,Liecomp(grp,0));
    else
    { index i,n=grp->g.ncomp,d;
      for (i=0; i<n; i++,x+=d,y+=d)
      { simpgrp* g=Liecomp(grp,i); d=g->lierank; simp_contragr(y,x,g); }
      for (i=0; i<grp->g.toraldim; i++) *x++= -*y++; /* negate torus part */
    }
  }
  return contr;
}
