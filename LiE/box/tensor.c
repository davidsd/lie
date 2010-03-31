#include  "lie.h"
#define local  static


local simpgrp* the_g;
local entry* lamrho,* cur_expon;
local bigint* cur_mult;

local entry* goal;
local bigint* totmul;


local void add_tensor_wt(entry* mu)
{ index k,r=the_g->lierank; boolean neg;
  addrow(lamrho,mu,cur_expon,r); /* |cur_expon=mu+lambda+rho| */
  neg=simp_make_dominant(cur_expon,the_g)%2!=0; /* apply |alt_dom| action */
  for (k=0; k<r; ++k) if (--(cur_expon[k])<0)
    return; /* subtract |rho| and quit unless result is dominant */
  wt_ins(cur_expon,cur_mult,neg); /* and insert into result */
}

local void add_goal_wt(entry* mu)
{ index r=the_g->lierank; boolean neg;
  addrow(lamrho,mu,cur_expon,r); /* |cur_expon=mu+lambda+rho| */
  neg=simp_make_dominant(cur_expon,the_g)%2!=0; /* apply |alt_dom| action */
  if (!eqrow(cur_expon,goal,r)) return; /* quit unless |cur_expon==goal| */
  totmul= neg ? sub(totmul,cur_mult) : add(totmul,cur_mult);
    /* incorporate in |totmul| */
}

local poly* simp_tensor_irr(entry* lambda,entry* mu,entry* nu,simpgrp* g)
{ poly* result; index i,r=g->lierank;
  the_g=g; testdom(lambda,(object)g); testdom(mu,(object)g);
  cur_expon=mkintarray(r);
  if (nu!=NULL) 
              { goal=mkintarray(r); copyrow(nu,goal,r); /* |goal=nu| */
                for (i=0; i<r; ++i) ++goal[i];  /* |goal+=rho| */
              }
  else goal=NULL;
  
  { bigint* deg_lam=simp_dim_irr(lambda,g),* deg_mu=simp_dim_irr(mu,g);
    if (cmp(deg_lam,deg_mu)<0) { entry* t=lambda; lambda=mu; mu=t; }
    freemem(deg_lam); freemem(deg_mu);
  }
  
  { lamrho=mkintarray(r); copyrow(lambda,lamrho,r); /* |lamrho=lambda| */
    for (i=0; i<r; ++i) ++lamrho[i]; /* |lamrho+=rho| */
  }
  if (nu!=NULL) { totmul=null; setshared(null); }  else wt_init(r);

  
  { poly* domchar=simp_domchar(mu,NULL,g); /* compute dominant character */
    Weylloopinit(g);
    for (i=0; i<domchar->nrows; ++i) /* traverse dominant weights in character */
    { cur_mult=domchar->coef[i];
        /* |cur_mult| is alias for the relevant multiplicity (shared) */
      Weylloop(nu ? add_goal_wt : add_tensor_wt,domchar->elm[i]);
    }
    Weylloopexit();
    freemem(domchar);
  }

  freearr(cur_expon); freearr(lamrho);
  if (nu==NULL) return wt_collect();
   else 
        { freearr(goal); result=mkpoly(1,0); result->coef[0]=totmul;
          setshared(totmul); return result;
        }
}

local poly* tensor_irr(entry* lambda,entry* mu,entry* nu)
{ if (type_of(grp)==SIMPGRP) return simp_tensor_irr(lambda,mu,nu,&grp->s);
  if (simpgroup(grp)) return simp_tensor_irr(lambda,mu,nu,Liecomp(grp,0));
  { poly* result;
    index s=Ssrank(grp),td=grp->g.toraldim; /* size of toral part */
    
    { lambda+=s; mu+=s; /* move to start of toral part */
      if (nu==NULL)
      { result=mkpoly(1,td);
        addrow(lambda,mu,*result->elm,td); *result->coef=one;
      }
      else
      { entry* lm=mkintarray(td); boolean correct_weight;
        addrow(lambda,mu,lm,td); nu+=s; /* move to toral part of weight */
        correct_weight=eqrow(lm,nu,td); freearr(lm);
        if (correct_weight) result=poly_one(0);  else return poly_null(0);
      }
    }
    
    { index i;
      for (i=grp->g.ncomp-1; i>=0; --i)
      { simpgrp* g=Liecomp(grp,i); index d=g->lierank;
        lambda-=d; mu-=d;  if (nu!=NULL) nu-=d;
          /* move back to previous component */
        result= Disjunct_mul_pol_pol(simp_tensor_irr(lambda,mu,nu,g),result);
      }
    }
    return result;
  }
}

poly* Tensor(poly* p,poly* q)
{ index i,j; poly* ans=poly_null(Lierank(grp));
  for (i=0; i<p->nrows; ++i)
    for (j=0; j<q->nrows; ++j)
      ans=Addmul_pol_pol_bin(ans,tensor_irr(p->elm[i],q->elm[j],NULL)
				,mult(p->coef[i],q->coef[j]));
  return ans;
}

bigint* Tensor_coef(poly* p, poly* q,vector* nu)
{ index i,j; bigint* ans=null;
  for (i=0; i<p->nrows; ++i)
    for (j=0; j<q->nrows; ++j)
    { poly* res=tensor_irr(p->elm[i],q->elm[j],nu->compon);
      ans=add(ans,mult(res->coef[0],mult(p->coef[i],q->coef[j])));
      freepol(res);
    }
  return ans;
}

poly* Ptensor(index n, poly* p)
{ poly* x,* y;
  if (n==0) return poly_one(Lierank(grp));
  if (n==1) return p;
  x=p; setshared(p); /* protect |p| against |freepol| */
  do { y=Tensor(x,p); freepol(x); x=y; } while (--n>1);
  clrshared(p); /* now |p| needs no more protection */
  return x;
}

