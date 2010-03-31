#include  "lie.h"
#define local  static
#define ACCMIN   8  


static poly* sorted,* pos_acc,* neg_acc;
  

local cmpfn_tp sav_cmpfn; /* to store suspended |cmpfn| */
local object sav_dfgrp; 

local simpgrp* the_g;
local entry* cur_expon;


void wt_init(index r)
{ sorted=copypoly(poly_null(r));
  pos_acc=mkpoly(ACCMIN,r); neg_acc=mkpoly(ACCMIN,r);
  pos_acc->nrows=0; neg_acc->nrows=0;
}

void wt_ins(entry* wt, bigint* c, boolean neg)
{ if (c->size==0) { freemem(c); return; }
  { index i=searchterm(sorted,wt);
    if (i>=0) 
            { clrshared(sorted->coef[i]);
              sorted->coef[i]= (neg ? sub : add)(sorted->coef[i],c);
              setshared(sorted->coef[i]);
            }
    else
    
    { poly** acc= neg ? &neg_acc : &pos_acc;
      index i=(*acc)->nrows;
      if (i==(*acc)->rowsize)
      { sorted=Add_pol_pol(sorted,*acc,neg);
        *acc=mkpoly(Max(sorted->nrows,ACCMIN),sorted->ncols); i=0;
      }
      copyrow(wt,(*acc)->elm[i],sorted->ncols); (*acc)->coef[i++]=c; setshared(c);
      (*acc)->nrows=i;
    }
  }
}

poly* wt_collect(void)
{ if (pos_acc->nrows>0) sorted=Add_pol_pol(sorted,pos_acc,false);
  else freemem(pos_acc);
  if (neg_acc->nrows>0) sorted=Add_pol_pol(sorted,neg_acc,true);
  else freemem(neg_acc);
  { poly* result=sorted; sorted=NULL; return result; }
}

local void set_weight_sorting(object g)
{ sav_cmpfn=cmpfn; sav_dfgrp=defaultgrp; cmpfn=height_decr; defaultgrp=g;
}

local poly* decompose_character(poly* ch)
{ wt_init(ch->ncols); /* for building result */
  while (ch->coef[0]->size!=0) /* i.e., |while (ch!=0)| */
  { bigint* c=ch->coef[0];
    if (c->size<0)
      
      { cmpfn=sav_cmpfn; defaultgrp=sav_dfgrp;
        error ("Non-virtual decomposition failed.\n");
      }
    
    { wt_ins(ch->elm[0],c,false); /* contribute weight to result */
      c=copybigint(c,NULL); c->size= -c->size;
      ch=Addmul_pol_pol_bin(ch,Domchar_irr(ch->elm[0],NULL),c);
    }
  }
  { poly* result=wt_collect();
    
    { cmpfn=sav_cmpfn; defaultgrp=sav_dfgrp; clrsorted(result); }
    return result;
  }
}

void char_init(object g) { set_weight_sorting(g); wt_init(Lierank(g)); }
poly* char_decomp(void) { return decompose_character(wt_collect()); }
poly* Decomp(poly* p)
{ poly* q=copypoly(p); freemem(p); clrsorted(q);
  set_weight_sorting(grp);
  return decompose_character(Reduce_pol(q));
}

local void add_decomp_wt(entry* mu)
{ index i,r=the_g->lierank; boolean neg;
  for (i=0; i<r; ++i) cur_expon[i]=mu[i]+1;
  neg=simp_make_dominant(cur_expon,the_g)%2!=0; /* apply |alt_dom| action */
  for (i=0; i<r; ++i) if (--(cur_expon[i])<0)
    return; /* subtract $\rho$ and quit unless result is dominant */
  wt_ins(cur_expon,one,neg); /* and insert into result */
}

local poly* simp_vdecomp_irr(entry* lambda, simpgrp* g)
{ the_g=g; wt_init(g->lierank);
Weylloopinit(g); Weylloop(add_decomp_wt,lambda); Weylloopexit();
  return wt_collect();
}

local poly* vdecomp_irr(entry* lambda)
{ if (type_of(grp)==SIMPGRP) return simp_vdecomp_irr(lambda,&grp->s);
  if (simpgroup(grp)) return simp_vdecomp_irr(lambda,Liecomp(grp,0));
  { poly* result; index i;
    
    { index td=grp->g.toraldim;  lambda+=Ssrank(grp);
    result=mkpoly(1,td); copyrow(lambda,*result->elm,td); *result->coef=one;
    }
    for (i=grp->g.ncomp-1; i>=0; --i)
      /* traverse simple components in reverse order */
    { simpgrp* g=Liecomp(grp,i); lambda-=g->lierank;
      result= Disjunct_mul_pol_pol(simp_vdecomp_irr(lambda,g),result);
    }
    return result;
  }
}

poly* Vdecomp(poly* p)
{ index i,r=Lierank(grp); poly* result=poly_null(r);
  cur_expon=mkintarray(r); /* large enough */
  for (i=0; i<p->nrows; ++i)
    result=Addmul_pol_pol_bin(result,vdecomp_irr(p->elm[i]),p->coef[i]);
  freearr(cur_expon); return result;
}

