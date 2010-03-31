#include  "lie.h"
#define local  static


local entry* simp_level_vec(simpgrp* g)
{ index i,r=g->lierank; entry* res=mkintarray(r),* ones=mkintarray(r);
  for (i=0; i<r; ++i) ones[i]=1;
  mulmatvecelm(simp_icart(g)->elm,ones,res,r,r); freearr(ones); return res;
}

local matrix* simp_domw(entry* lambda, simpgrp* g, vector** levels)
{ index r=g->lierank;
  matrix* posroots=Matmult(simp_proots(g),simp_Cartan(g));
  entry* x=mkintarray(r);
  matrix* weights=mkmatrix(EXTEND,r);
  vector* wt_levels;
  
  { entry* rho_hat=simp_level_vec(g); index i,lev=0; 
    for (i=0; i<r; ++i) lev+=lambda[i]*rho_hat[i]; freearr(rho_hat);
    wt_levels=mkvector(lev/simp_detcart(g)+2);
  }
  
  { index lev,d,k;
    index lev_start=1,end=1; /* start and end of current level */
  
    copyrow(lambda,weights->elm[0],r); wt_levels->compon[0]=0;
    wt_levels->compon[1]=1;
  
    for (lev=1; lev<wt_levels->ncomp-1; wt_levels->compon[++lev]=lev_start=end)
      for (k=0,d=1; k<posroots->nrows; ++k) /* try roots by increasing level */
        if (k==g->level->compon[d] && ++d>lev) break;
  	/* now root $\alpha_k$ has level |d| */
        else
        
        { index w;
            /* index of a dominant weight $\mu\below\lambda$ at level |lev-d| */
          for (w=wt_levels->compon[lev-d]; w<wt_levels->compon[lev-d+1]; ++w)
            if (pos_subrow(weights->elm[w],posroots->elm[k],x,r))
              /* |x=mu-@t$\alpha_k$@>| must be dominant */
              
              { index i; 
                for (i=lev_start; i<end; ++i) 
                  if (eqrow(x,weights->elm[i],r)) goto next_wt;
                if (end==weights->rowsize)
                  { weights->nrows=end; weights=extendmat(weights); }
                copyrow(x,weights->elm[end++],r);
              next_wt: {}
              }
        }
    weights->nrows=end;
  }
  if (levels!=NULL) *levels=wt_levels;  else freemem(wt_levels);
  freearr(x); freemem(posroots); return weights;
}

local index locate(entry* mu,entry** m,entry* rho_hat,entry* level,simpgrp* g)
{ index l=0,i,r=g->lierank; entry* lambda=m[0]; /* the highest weight */
  for (i=0; i<r; ++i) l += (lambda[i]-mu[i])*rho_hat[i];
    /* level of |mu| below |lambda| */
  if ((l/=simp_detcart(g))>=0)
    for (i=level[l]; i<level[l+1]; ++i) 
      if (eqrow(mu,m[i],r)) return i;
  return -1; /* weight |mu| is not found if we get here */
}

local void gather_roots
  (entry* mu, entry** posr_weight, simpgrp* g, entry* mults)
{ index i,j,k,l,r=g->lierank,n=g->roots->nrows;
  entry** posr=g->roots->elm;
  for (i=0; i<n; ++i) mults[i]=1;
  for (l=1; l<g->level->ncomp-1; ++l)
    /* traverse all root levels except the highest */
    for (j=0; j<r; ++j)
      if (mu[j]==0)
	
	for (i=g->level->compon[l-1]; i<g->level->compon[l]; ++i)
	  if (mults[i]!=0)
	  { entry e=-posr_weight[i][j];
	    if (e>0)
	    { ignore_intr(); posr[i][j]+=e; k=find_root(posr[i],l+e,g);
	      posr[i][j]-=e; allow_intr(); assert(k>=0);
	      mults[k]+=mults[i]; mults[i]=0;
	    }
	  }
}

local long c(entry* mu, simpgrp* g)
{ index a,r=g->lierank,i,j;
  entry** ic=simp_icart(g)->elm,* n=g->root_norm->compon;
  for (a=0,j=0; j<r; ++j)
  { entry s=0;
    for (i=0; i<r; ++i) s+=(mu[i]+1)*ic[i][j];
    a+=s*n[j]*(mu[j]+1);
  }
  return a;
}


bigint* simp_dim_irr(entry* lambda, simpgrp* g)
{ index i,j,r=g->lierank;
  bigint* denom=copybigint(one,NULL),* numer=copybigint(one,NULL);
  matrix* posr=simp_proots(g); entry* norm=g->root_norm->compon;
  for (i=0; i<posr->nrows; ++i)
  { entry* alpha=posr->elm[i]; entry den=0,num=0;
    for (j=0; j<r; ++j)
      { den += alpha[j]*norm[j]; num += alpha[j]*norm[j]*(lambda[j]+1); }
    denom=mul1(denom,den); numer=mul1(numer,num);
  }
  return quotient(numer,denom);
}

bigint* DimIrr(entry* lambda)
{ if (type_of(grp)==SIMPGRP) return simp_dim_irr(lambda,&grp->s);
  if (simpgroup(grp)) return simp_dim_irr(lambda,Liecomp(grp,0));
  { index i,s=0; bigint* res=one;
    for (i=0; i<grp->g.ncomp; ++i)
    { simpgrp* g=Liecomp(grp,i); 
      res=mult(res,simp_dim_irr(&lambda[s],g));
      s+=g->lierank;
    }
    return res;
  }
}

bigint* Dim(poly* p)
{ index i,r=p->ncols; bigint* res=null; entry* lambda= mkintarray(r);
  for (i=0; i<p->nrows; ++i)
  { copyrow (p->elm[i],lambda,r); make_dominant(lambda);
    res=add(res,mult(p->coef[i],DimIrr(lambda)));
  }
  freearr(lambda); return res;
}

matrix* Domweights(vector* lambda)
{ if (type_of(grp)==SIMPGRP) return simp_domw(lambda->compon,&grp->s,NULL);
  if (simpgroup(grp)) return simp_domw(lambda->compon,grp->g.liecomp[0],NULL);
  { index i,j,n=grp->g.ncomp,r=Lierank(grp);
    matrix** M=alloc_array(matrix*,n);
    for (i=0,j=0; i<n; j+=grp->g.liecomp[i++]->lierank)
      M[i]=simp_domw(&lambda->compon[j],grp->g.liecomp[i],NULL);
    return Cartes_prod(M,n,&lambda->compon[j],r);
  }
}

matrix* Cartes_prod(matrix** M,index n,entry* toral,index r)
{ index i=0,j=0; entry* pos,* offset,* lambda; matrix* result;
  
  { index i, s=0, p=1;
      /* sum of row sizes and product of column sizes of the |M[i]| */
    pos=mkintarray(2*n+r),offset=&pos[n],lambda=&offset[n];
    for (i=0; i<n; ++i)
      { pos[i]=0; offset[i]=s; s+=M[i]->ncols; p*=M[i]->nrows; }
    result = mkmatrix(p,r);
    copyrow(toral,&lambda[s],r-s); /* toral part of working vector */
  }
  do /* now |i| indicates first simple factor whose entries in |lambda|
	need to be updated */
  { for (;i<n;++i) copyrow(M[i]->elm[pos[i]],&lambda[offset[i]],M[i]->ncols);
      /* update |lambda| */
    copyrow(lambda,result->elm[j++],r);
      /* copy full vector |lambda| to result */
    while (--i>=0 && ++pos[i] == M[i]->nrows) pos[i]=0;
      /* advance |pos| to match |j| */
  } while (i>=0); /* full circle when |pos| returns to all zeroes */
  freearr(pos); { while (++i<n) freemem(M[i]);} freearr(M); return(result);
}

poly* simp_domchar(entry* lambda,entry* muo,simpgrp* g)
{ index k;
  vector* wt_levels; poly* dom_w= mat2pol(simp_domw(lambda,g,&wt_levels));
  
  matrix* roots=Matmult(simp_proots(g),simp_Cartan(g));
    /* positive roots on weight basis */
  long c_lambda=c(lambda,g), det=simp_detcart(g);
  entry* mults=mkintarray(g->roots->nrows); 
  
  index r=g->lierank; 
  entry* x=mkintarray(2*r),* y=&x[r]; 
  
  entry* rho_hat=simp_level_vec(g);
  
  index last_row=  muo==NULL ? dom_w->nrows-1
    : locate(muo,dom_w->elm,rho_hat,wt_levels->compon,g);


  dom_w->coef[0]=one; /* initialise first multiplicity to 1 */
  for (k=1; k<=last_row; ++k) 
                         { index i; entry* mu=dom_w->elm[k];
                           bigint* sum=null; long denominator=(c_lambda-c(mu,g))/(2*det);
                           gather_roots(mu,roots->elm,g,mults);
                             /* compute |mults| for stabiliser group $W_\mu$ */
                           for (i=0; i<roots->nrows; ++i)
                             if (mults[i]>0)
                             { entry* alpha=roots->elm[i];
                               
                               { index k; entry* a=g->roots->elm[i]; entry inp=0,d=0;
                                 entry* norm=g->root_norm->compon;
                                 for (k=0; k<r; ++k) /* set |d=(alpha,alpha)| and |inp=(mu,alpha)| */
                                   { d+=alpha[k]*norm[k]*a[k]; inp+=mu[k]*norm[k]*a[k]; }
                                 copyrow(mu,x,r); /* initialise |x=mu| */
                                 do
                                 { add_xrow_to(x,1,alpha,r);
                                   copyrow(x,y,r); simp_make_dominant(y,g);
                                   if ((k=locate(y,dom_w->elm,rho_hat,wt_levels->compon,g))<0)
                                     break; /* not found */
                                   sum=add(sum,mul1(copybigint(dom_w->coef[k],NULL),mults[i]*(inp+=d)));
                                 } while(true);
                               }
                             }
                           dom_w->coef[k]=quotient(sum,entry2bigint(denominator));
                           setshared(dom_w->coef[k]);
                         }
  freemem(wt_levels); 
                      freemem(roots); freearr(mults);
                      
                      freearr(x);
                      
                      freearr(rho_hat);
  if (muo==NULL) return dom_w; 
  else 
       { poly* mul=poly_null(0); /* polynomial in |0| indeterminates */
         if (last_row<0) return mul;
         mul=copypoly(mul); mul->coef[0]=dom_w->coef[last_row];
         setshared(mul->coef[0]); freemem(dom_w); return mul;
       }
}

poly* Domchar_irr(entry* lambda,entry* muo)
{ index i,j,r=Lierank(grp),s=Ssrank(grp); poly* result;
  if (type_of(grp)==SIMPGRP) return simp_domchar(lambda,muo,&grp->s);
  if (simpgroup(grp)) return simp_domchar(lambda,muo,Liecomp(grp,0));
  
  if (muo==NULL)
  { result=mkpoly(1,r-s);
    copyrow(&lambda[s],result->elm[0],r-s); *result->coef=one;
  }
  else
    if (eqrow(&lambda[s],&muo[s],r-s)) result=poly_one(0); 
    else return poly_null(0);
  for (i=grp->g.ncomp-1,j=s; i>=0; --i)
    /* traverse components in reverse order */
  { simpgrp* g=grp->g.liecomp[i];  j-=g->lierank;
    result= Disjunct_mul_pol_pol
      (simp_domchar(&lambda[j],muo==NULL?NULL:&muo[j],g),result);
  }
  return result;
}

poly* Domchar_p(poly* p)
{ index i,r=p->ncols; poly* result=poly_null(r);
  for (i=0; i<p->nrows; ++i)
  { testdom(p->elm[i],grp);
    result=Addmul_pol_pol_bin(result,Domchar_irr(p->elm[i],NULL),p->coef[i]);
  }
  return result;
}

entry* Lv(object g)
{ if (type_of(g)==SIMPGRP) return simp_level_vec(&g->s);
  if (simpgroup(g)) return simp_level_vec(Liecomp(g,0));
  { index i,j=0,r=Lierank(g); entry* result=mkintarray(r);
    for(i=0; i<g->g.ncomp; ++i)
    { simpgrp* s=g->g.liecomp[i]; entry* l=simp_level_vec(s);
      copyrow(l,&result[j],s->lierank); freearr(l); j+=s->lierank;
    }
    for(i=0; i<g->g.toraldim; ++i) result[j+i]=0;
    return result;
  }
}

