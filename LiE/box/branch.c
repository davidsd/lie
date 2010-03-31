#include "lie.h"

#ifdef __STDC__
static void add_spec_wt(entry* v);
static poly* simp_spec_irr(entry* lambda,entry* t,simpgrp* g);
static poly* spec_irr(entry* lambda,entry* t,object G);
static void add_branch_wt(entry* v);
static poly* simp_branch_irr(entry* lambda,entry** m,simpgrp* g);
static poly* branch_irr(entry* lambda,entry** m,object G);
#endif

static index r;   /* the lie rank of the (main) group */
static entry* h;   /* the semisimple element whose spectrum is analysed */
static entry ord;  /* the order of the semisimple element */
static bigint** spec; /* the coefficients accumulating the spectrum */
static bigint* multi; /* multiplicity for add_spec_wt and add_branch_wt */

/* the call of Weylloop below repeatedly calls on the following procedure
   `add_spec_wt', with `v' referring to a weight in the current Weyl orbit
*/
static void add_spec_wt(v) entry* v;
{ entry i=inprow(v,h,r)%ord; if (i<0) i+=ord;
  { bigint** c=spec+i; clrshared(*c); *c=add(*c,multi); setshared(*c); }
}

/* compute multiplicities of the ord different eigenvalues of toral_elt
   (namely $e^{2k\pi i/ord}$ for k=0,..,ord-1, where toral_elt is an element
   of the torus of order ord = toral_elt[r]) on module of weight lambda
*/
static poly* simp_spec_irr(lambda,t,g) entry* lambda,* t; simpgrp* g;
{ poly* domchar=simp_domchar(lambda,NULL,g),* result=mkpoly(ord,1); index i;
  Weylloopinit(g); r=g->lierank; h=t; spec=result->coef;
  for (i=0; i<ord; i++) { spec[i]=null; result->elm[i][0]=i; }
  for (i=0; i<domchar->nrows; i++) /* traverse all dominant weights */
  { multi=domchar->coef[i];
    Weylloop(add_spec_wt,domchar->elm[i]); /* collect induced dominant weights */
  }
  freepol(domchar); Weylloopexit();
  return result;
}

static poly* spec_irr(lambda,t,G) entry* lambda,* t; object G;
{ index i,j,S=Ssrank(G),td=G->g.toraldim; poly* x; entry exp;
  lambda+=S; t+=S;
  x=mkpoly(1,1); *x->coef=one;
  exp=inprow(lambda,h,td)%ord; if (exp<0) exp+=ord; x->elm[0][0]=exp;
  for (i=G->g.ncomp-1; i>=0; i--)
  { simpgrp* g=Liecomp(G,i); index d=g->lierank; lambda-=d; t-=d;
    x=Mul_pol_pol(simp_spec_irr(lambda,t,g),x); /* tensor in T1 */
    for (j=0; j<x->nrows; j++) x->elm[j][0] %=ord; x=Reduce_pol(x);
  }
  return x;
}

poly* Spectrum(p,toral_elt) poly* p; vector* toral_elt;
{ entry* t=toral_elt->compon,** lambda=p->elm; index i,r=toral_elt->ncomp-1;
  poly* result=poly_null(1); ord=t[r]; /* initialise ord once and for all */
  for (i=0; i<p->nrows; i++) /* lambda= &(p->elm[i]) */
    result=Addmul_pol_pol_bin(result,spec_irr(*lambda++,t,grp),p->coef[i]);
  return result;
}

static index rsub   /* the lie rank of the "sub" group */
	   , ssub;  /* the semisimple rank of the "sub" group */
static entry** resmat /* m->elm */
	  ,* add_wt; /* induced weigth on h, used by add_branch_wt */

/* Branching is calculated as follows.
   First the dominant weights and their multiplicities are calculated in the
   "source" group `g' by Freudenthals recursion. Then for each dominant
   weight found its Weyl orbit is traversed, and to all weigths so obtained
   the linear map `m' is applied to obtain a weight for the "destination"
   group `h'. Then those image weights that are dominant (for `h') are col-
   lected, and the result is decomposed as a sum of modules by `char_decomp'.
*/

/* the call of Weylloop below repeatedly calls on the following procedure
   `add_branch_wt', with `v' referring to a weight in the current Weyl orbit
*/
static void add_branch_wt(v) entry* v;
{ register index i; mulvecmatelm(v,resmat,add_wt,r,rsub); /* add_wt=v*m */
  for (i=0; i<ssub; i++) if (add_wt[i]<0) return; /* skip if not dominant */
  wt_ins(add_wt,multi,false);  /* collect weights with multiplicity */
}

/* branch irreducible module `lambda' to group `grp' from simple group `g'
   via linear map `m' */
static poly* simp_branch_irr(lambda,m,g) entry* lambda,** m; simpgrp* g;
{ poly* domchar; index i; r=g->lierank; resmat=m;
  char_init(grp); Weylloopinit(g);
  domchar=simp_domchar(lambda,NULL,g); /* compute dominant part character */
  for (i=0; i<domchar->nrows; i++) /* traverse all dominant weights */
  { multi=domchar->coef[i]; /* this is already shared */
    Weylloop(add_branch_wt,domchar->elm[i]); /* collect induced dominant weights */
  }
  freepol(domchar); Weylloopexit();
  return char_decomp(); /* compute grp-decomposition (non virtual) */
}

/* branch irreducible module `lambda' to group `grp' from composite group `G'
   via linear map `m' */
static poly* branch_irr(lambda,m,G) entry* lambda,** m; object G;
{ index i,S=Ssrank(G),td=G->g.toraldim,r=Lierank(grp); poly* x;
  if (type_of(G)==SIMPGRP) return simp_branch_irr(lambda,m,&G->s);
  if (simpgroup(G)) return simp_branch_irr(lambda,m,Liecomp(G,0));
  lambda+=S; m+=S;
  x=mkpoly(1,r); mulvecmatelm(lambda,m,*x->elm,td,r);
  *x->coef=one; x=Alt_dom(x); /* ensure dominant */
  for (i=G->g.ncomp-1; i>=0; i--)
  { simpgrp* g=Liecomp(G,i); index d=g->lierank; lambda-=d; m-=d;
    { poly* y=simp_branch_irr(lambda,m,g),* z=Tensor(y,x); /* tensor in grp */
      freepol(x); freepol(y); x=z;
    }
  }
  return x;
}

/* the following provides a direct entry point for branching irreducibles */
poly* Branch_irr(lambda,m,G) entry* lambda,** m; object G;
{ poly* ans; rsub=Lierank(grp); ssub=Ssrank(grp); add_wt=mkintarray(rsub);
  ans=branch_irr(lambda,m,G); freearr(add_wt); return ans;
}

/* branch module `p' to group `grp' from group `G' via linear map `m' */
poly* Branch(p,m,G) poly* p; entry** m; object G;
{ index i; entry** lambda=p->elm; poly* ans=poly_null(rsub=Lierank(grp));
  ssub=Ssrank(grp); add_wt=mkintarray(rsub);
  for (i=0; i<p->nrows; i++) /* lambda= &(p->elm[i]) */
    ans=Addmul_pol_pol_bin(ans,branch_irr(*lambda++,m,G),p->coef[i]);
  freearr(add_wt); return ans;
}

poly* Collect(p,iresmat,d,g) poly* p; matrix* iresmat; entry d; object g;
{ index i,j,r=Lierank(grp),s=Ssrank(g);
  poly* dc=Domchar_p(p),* result;
  entry** ires=iresmat->elm,** dom_ch=dc->elm,* add_wt=mkintarray(r);

  char_init(g);
  for (i=0; i<dc->nrows; i++)
  { mulvecmatelm(*dom_ch++,ires,add_wt,r,r);	   /* lift to the group g */
    for (j=0; j<s; j++) if (add_wt[j]<0) goto nxt_i; /* skip non-dominant */
    if (d!=1) for (j=0; j<r; j++)
    { if (add_wt[j]%d) error("Collected weight not integral.\n"); 
      add_wt[j]/=d;
    }
    wt_ins(add_wt,dc->coef[i],false); /* add to char */
nxt_i: ;
  }
  grp=g; /* decomp for containing group, while Domchar was for subgroup */
  result=char_decomp(); freearr(add_wt); freepol(dc); return result;
}
