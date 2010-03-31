#include  "lie.h"
#define local  static

boolean wronggroup(char lietype,index rank)
{ return lietype=='T' ? rank<0
       : lietype=='A' ? rank<1
     : lietype=='B' ? rank<2
       : lietype=='C' ? rank<2
       : lietype=='D' ? rank<3
     : lietype=='E' ? rank<6 || rank>8
       : lietype=='F' ? rank!=4
       : rank!=2;
}

boolean simpgroup(object g)
{ return (g->g.toraldim==0 && g->g.ncomp==1); }

index Lierank(object grp)
{ index i,r;  if (type_of(grp)==SIMPGRP) return grp->s.lierank;

  r=grp->g.toraldim; 
  for (i=0; i<grp->g.ncomp; ++i) r += (Liecomp(grp, i))->lierank;
  return r;
}

index Ssrank(object g) /* Semisimple rank */
{ index i,r=0;  if (type_of(g)==SIMPGRP) return g->s.lierank;
  for (i=0; i<g->g.ncomp; ++i) r += (Liecomp(g,i))->lierank;
  return r;
}

matrix* simp_Cartan(simpgrp* g)
{ if (g->cartan!=NULL) return g->cartan;
  { entry r=g->lierank; matrix* cartan=g->cartan=mat_null(r,r);
    entry** m=cartan->elm; 
    setlonglife(cartan); /* make Cartan matrix permanent */
    
    { index i; m[0][0]=2;
      for (i=1; i<r; ++i) { m[i][i]=2; m[i-1][i]=m[i][i-1]= -1; }
    }
    switch (g->lietype)
    {	    case 'B': m[r-2][r-1]= -2;
    break;  case 'C': m[r-1][r-2]= -2;
    break;  case 'D': m[r-3][r-1]=m[r-1][r-3]= -1;
		      m[r-2][r-1]=m[r-1][r-2]=0;
    break;  case 'E': m[0][1]=m[1][0]=m[1][2]=m[2][1]=0;
		      m[0][2]=m[2][0]=m[1][3]=m[3][1]= -1;
    break;  case 'F': m[1][2]= -2;
    break;  case 'G': m[1][0]= -3;
    }
    return cartan;
  }
}

matrix* Cartan(void)
{ if (type_of(grp)==SIMPGRP) return simp_Cartan(&grp->s);
  if (simpgroup(grp)) return simp_Cartan(Liecomp(grp,0));
  { index i,j, t=0;
    matrix* cartan=mat_null(Ssrank(grp),Lierank(grp));
    for (i=0; i<grp->g.ncomp; ++i)
    { index r=Liecomp(grp,i)->lierank;
      entry** c=simp_Cartan(Liecomp(grp,i))->elm;
      for (j=0; j<r; ++j) copyrow(c[j],&cartan->elm[t+j][t],r);
      t+=r;
    }
    return cartan;
  }
}

entry simp_detcart(simpgrp* g)
{ char t=g->lietype; index r=g->lierank; 
  return t=='A' ? r+1
       : t=='B' || t=='C' ? 2
       : t=='D' ? 4
       : t=='E' ? 9-r
       : 1;
}

entry Detcartan(void)
{ if (type_of(grp)==SIMPGRP) return simp_detcart(&grp->s);
  { index i; entry result=1;
    for (i=0; i<grp->g.ncomp; ++i) result *= simp_detcart(Liecomp(grp,i));
    return result;
  }
}

matrix* simp_icart(simpgrp* g)
{ if (g->icartan) return g->icartan;
  { index i, j, r=g->lierank;
    matrix* icartan=g->icartan=mkmatrix(r,r); entry** m=icartan->elm;
     setlonglife(icartan); /* permanent data */
    switch (g->lietype)
    {	      case 'A':
      for (i=0; i<r; ++i)  for (j=0; j<=i; ++j)
	m[i][j]=m[j][i]=(r-i)*(j+1);
     break; case 'B':
      for (i=0; i<r; ++i)  for (j=0; j<=i; ++j) m[i][j]=m[j][i]=2*(j+1);
      for (i=0; i<r; ++i) m[r-1][i]=i+1;
     break; case 'C':
      for (i=0; i<r; ++i)  for (j=0; j<=i; ++j) m[i][j]=m[j][i]=2*(j+1);
      for (i=0; i<r; ++i) m[i][r-1]=i+1;
     break; case 'D':
      for (i=0; i<r-2; ++i)  for (j=0; j<=i; ++j) m[i][j]=m[j][i]=4*(j+1);
      for (i=0; i<r-2; ++i) m[r-1][i]=m[r-2][i]=m[i][r-1]=m[i][r-2]=2*(i+1);
      m[r-1][r-1]=m[r-2][r-2]=r; m[r-1][r-2]=m[r-2][r-1]=r-2;
     break; case 'E':
      m[0][0]=4; m[1][0]=m[0][1]=r-3; m[0][2]=m[2][0]=r-1;
      m[1][1]=r; m[1][2]=m[2][1]=2*r-6; m[2][2]=2*r-2;
      for (i=1; i<r-2; ++i)  for (j=0; j<3; ++j) m[r-i][j]=m[j][r-i]=(j+2)*i;
      for (i=1; i<r-2; ++i)  for (j=1; j<=i; ++j)
	m[r-i][r-j]=m[r-j][r-i]=(9-r+i)*j;
     break; case 'F':
      for (i=1; i<4; ++i)  for (j=1; j<4; ++j) m[r-i][j-1]=i*j;
      m[1][2]=8;
      for (i=0; i<3; ++i) m[0][i]=m[r-i-1][3]=i+2;
      m[0][3]=2;
     break; case 'G': m[0][0]=m[1][1]=2; m[0][1]=1; m[1][0]=3;
    }
    return icartan;
  }
}

matrix* Icartan(void)
{ if (simpgroup(grp)) return simp_icart(Liecomp(grp,0));
  { matrix* result=mat_null(Lierank(grp),Ssrank(grp)); entry** m=result->elm;
    index k,t=0;
    entry det=Detcartan(); /* product of determinants of simple factors */
    for (k=0; k<grp->g.ncomp; ++k)
    { simpgrp* g=Liecomp(grp,k);
      index i,j,r=g->lierank;
      entry** a=simp_icart(g)->elm;
      entry f=det/simp_detcart(g); /* multiplication factor */
      for (i=0; i<r; ++i)  for (j=0; j<r; ++j) m[t+i][t+j]=f*a[i][j];
      t+=r;
    }
    return result;
  }
}

local entry* simp_exponents (simpgrp* g)
{ if (g->exponents!=NULL) return g->exponents->compon;
  { static entry
      exp_E[3][7] = {{4,5,7,8,11},{5,7,9,11,13,17},{7,11,13,17,19,23,29}}
    , exp_F4[3] = {5,7,11};
    index i,r=g->lierank; entry* e=(g->exponents=mkvector(r))->compon; 
    setlonglife(g->exponents); e[0]=1;
    switch (g->lietype)
    {	 case 'A': /* $1,2,3,\ldots,r$ */
      for (i=1; i<r; ++i) e[i]=i+1; 
    break; case 'B': case 'C': /* $1,3,5,\ldots,2r-1$ */
      for (i=1; i<r; ++i) e[i]=2*i+1; 
    break; case 'D': /* $1,3,5,\ldots,r-2,r-1,r,\ldots,2r-3$
                     or $1,3,5,\ldots,r-1,r-1,\ldots,2r-3$ */
	for (i=0; 2*i+1<r; ++i) { e[i]=2*i+1; e[r-i-1]=2*(r-i)-3; }
	if (2*i+1==r) e[i]=r-1; 
    break; case 'E': copyrow(exp_E[r-6],&e[1],r-1);
    break; case 'F': copyrow(exp_F4,&e[1],3);
    break; case 'G': e[1]=5;
    }
    return e;
  }
}

vector* Exponents(object grp)
{ if (type_of(grp)==SIMPGRP)
    { simp_exponents(&grp->s); return grp->s.exponents; }
  if (simpgroup(grp))
    { simp_exponents(Liecomp(grp,0)); return Liecomp(grp,0)->exponents; }
  { index i,t=0; vector* v=mkvector(Lierank(grp)); entry* e=v->compon;
    { for (i=0; i<grp->g.ncomp; ++i)
      { simpgrp* g=Liecomp(grp,i); index r=g->lierank; 
	copyrow(simp_exponents(g),&e[t],r); t+=r;
      }
      for (i=0; i<grp->g.toraldim; ++i) e[t+i]=0;
    }
    return v;
  }
}

index simp_numproots(simpgrp* g)
{ index r=g->lierank; return r*(1+simp_exponents(g)[r-1])/2; }

index Numproots(object grp) /* should really return bigint */
{ if (type_of(grp)==SIMPGRP) return simp_numproots(&grp->s);
  { index i,d=0; 
    for (i=0; i<grp->g.ncomp; ++i) d += simp_numproots(Liecomp(grp,i));
    return d;
  }
}

matrix* simp_proots(simpgrp* g)
{ if (g->roots!=NULL) return g->roots;
  { index r=g->lierank,l,i,last_root;
    entry** cartan=simp_Cartan(g)->elm;
    entry** posr=(g->roots=mkmatrix(simp_numproots(g),r))->elm;
    entry* level=(g->level=mkvector(simp_exponents(g)[r-1]+1))->compon;
    entry* norm=(g->root_norm=mkvector(g->roots->nrows))->compon;
    entry* alpha_wt=mkintarray(r);
      /* space to convert roots to weight coordinates */
    setlonglife(g->roots),
    setlonglife(g->level),
    setlonglife(g->root_norm); /* permanent data */
    
    { index i,j;  for (i=0; i<r; ++i)  for (j=0; j<r; ++j) posr[i][j] = i==j;
      level[0]=0; last_root=r;
      for (i=0; i<r; ++i) norm[i]=1; /* norms are mostly |1| */
      switch (g->lietype) /* here are the exceptions */
      {	    case 'B': 
        for (i=0; i<r-1; ++i) norm[i]=2; /* $2,2,\ldots,2,1$ */
       break; case 'C': norm[r-1]=2; /* $1,1,\ldots,1,2$ */
       break; case 'F': norm[0]=norm[1]=2; /* $2,2,1,1$ */
       break; case 'G': norm[1]=3; /* $ 1,3$ */
      }
    }
    for (l=0; last_root>level[l]; ++l)
    { level[l+1]=last_root; /* set beginning of a new level */
      for (i=level[l]; i<level[l+1]; ++i)
	
	{ index j,k; entry* alpha=posr[i];  mulvecmatelm(alpha,cartan,alpha_wt,r,r);
	    /* get values $\<\alpha,\alpha_j>$ */
	  for (j=0; j<r; ++j) /* try all fundamental roots */
	  { entry new_norm; 
	    
	    { if (alpha_wt[j]<0) /* then $\alpha+\alpha_j$ is a root; find its norm */
	        if (norm[j]==norm[i]) new_norm=norm[j]; /* |alpha_wt[j]==-1| */
	        else new_norm=1; /* regardless of |alpha_wt[j]| */
	      else if (norm[i]>1 || norm[j]>1) continue; /* both roots must be short now */
	      else if (strchr("ADE",g->lietype)!=NULL) continue;
	        /* but long roots must exist */
	      else if (alpha_wt[j]>0)
	        if (g->lietype!='G'||alpha_wt[j]!=1) continue;  else new_norm=3;
	        /* $[2,1]\to[3,1]$ for $G_2$ */
	      else if (alpha[j]==0) continue;
	        /* $\alpha-\alpha_j$ should not have a negative entry */
	      else
	      { 
	        { --alpha[j]; 
	          for (k=level[l-1]; k<level[l]; ++k) 
	            if (eqrow(posr[k],alpha,r)) break;
	          ++alpha[j]; 
	          if (k==level[l]) continue;
	        }
	      new_norm=2; }
	    }
	    ++alpha[j]; /* temporarily set $\alpha\K\alpha+\alpha_j$ */
	    for (k=level[l+1]; k<last_root; ++k)
	      if (eqrow(posr[k],alpha,r)) break;
	      /* if already present, don't add it */
	    if (k==last_root)
	      { norm[last_root]=new_norm; copyrow(alpha,posr[last_root++],r); }
	    --alpha[j]; /* restore |alpha| */
	  }
	}
    }
    freearr(alpha_wt); return g->roots;
  }
}

matrix* Posroots(object grp)
{ if (type_of(grp)==SIMPGRP) return simp_proots(&grp->s);
  if (simpgroup(grp)) return simp_proots(Liecomp(grp,0));
  { index i,j,t1=0,t2=0;
    matrix* result=mat_null(Numproots(grp),Ssrank(grp));
    entry** m=result->elm;
    for (i=0; i<grp->g.ncomp; ++i)
    { matrix* posr=simp_proots(Liecomp(grp,i));
      index r=Liecomp(grp,i)->lierank;
      for (j=0; j<posr->nrows; ++j) copyrow(posr->elm[j],&m[t1+j][t2],r);
      t1+=posr->nrows; t2+=r;
    }
    return result;
  }
}

vector* Highroot(simpgrp* g)
{ matrix* posr=simp_proots(g); index r=g->lierank; vector* high=mkvector(r);
  copyrow(posr->elm[posr->nrows-1],high->compon,r); return high;
}

vector* Simproot_norms(object grp)
{ if (type_of(grp)==SIMPGRP)
    { simp_proots(&grp->s); return grp->s.root_norm; }
  { index i; for (i=0; i<grp->g.ncomp; ++i) simp_proots(Liecomp(grp,i)); }
  if (grp->g.ncomp==1) return Liecomp(grp,0)->root_norm;
  { index i,t=0; vector* result=mkvector(Ssrank(grp));
    for (i=0; i<grp->g.ncomp; ++i)
    { simpgrp* g=Liecomp(grp,i); index r=g->lierank;
      copyrow(g->root_norm->compon,&result->compon[t],r); t+=r;
    }
    return result;
  }
}

static void set_simp_adjoint(entry* dst,simpgrp* g)
{ index r=g->lierank; vector* high=Highroot(g);
  mulvecmatelm(high->compon,g->cartan->elm,dst,r,r); freemem(high);
}

poly* Adjoint(object grp)
{ index i,j,r=Lierank(grp)
  ,n=type_of(grp)==SIMPGRP ? 1: grp->g.ncomp+(grp->g.toraldim!=0);
  poly* adj= mkpoly(n,r);
  for (i=0; i<n; ++i)
  { adj->coef[i]=one; for (j=0; j<r; ++j) adj->elm[i][j]=0; }
  if (type_of(grp)==SIMPGRP) set_simp_adjoint(adj->elm[0],&grp->s);
  else
  { index offs=0; simpgrp* g;
    for (i=0; i<grp->g.ncomp; offs+=g->lierank,++i)
      set_simp_adjoint(&adj->elm[i][offs],g=Liecomp(grp,i));
    if (grp->g.toraldim!=0)
    { adj->coef[i]=entry2bigint(grp->g.toraldim);
      setshared(adj->coef[i]);
    }
  }
  return adj;
}

entry Dimgrp(object grp)
{ return Lierank(grp) + 2*Numproots(grp); }

matrix* Center(object grp)
{ index i,j,R=Lierank(grp),n_gen;
  
  for (n_gen=grp->g.toraldim,i=0; i<grp->g.ncomp; ++i)
  { simpgrp* g=Liecomp(grp,i);
    if (simp_detcart(g)>1) n_gen+=1+(g->lietype=='D' && g->lierank%2==0);
  }
  { matrix* res=mat_null(n_gen,R+1); entry** m=res->elm; index k=0,s=0;
    for (j=0; j<grp->g.ncomp; ++j)
    { simpgrp* g=Liecomp(grp,j); index n=g->lierank; entry d=simp_detcart(g);
      if (d>1)
      { 
        switch (g->lietype)
        {      case 'A':  for (i=0; i<n; ++i) m[k][s+i]=i+1;
        	/* $[1,2,3,\ldots,n]$; $d=n+1$ */ 
        break; case 'B': m[k][s+n-1]=1; /* $[0,0,\ldots,0,1]$; $d=2$ */ 
        break; case 'C':  for (i=0; i<n; i+=2) m[k][s+i]=1;
        	/* $[1,0,1,0,\ldots]$; $d=2$ */ 
        break; case 'D': 
                      { m[k][s+n-2]=m[k][s+n-1]=1;
                        if (n%2==1)  for (i=0; i<n; i+=2) m[k][s+i]+=2;
                          /* $[2,0,2,\ldots,2,1,3]$; $d=4$ */
                        else
                        { d=2; m[k++][R]=d; /* $[0,0,\ldots,0,1,1]$; $d=2$ */
                          for (i=0; i<n; i+=2) m[k][s+i]=1; /* $[1,0,1,\ldots,1,0]$; $d=2$ */
                        }
                      }
         
        break; case 'E':
          if (n==7)
            { m[k][s+1]=m[k][s+4]=m[k][s+6]=1; } /* $[0,1,0,0,1,0,1]$; $d=2$ */
          else { m[k][s]=m[k][s+4]=1; m[k][s+2]=m[k][s+5]=2; }
             /* $[1,0,2,0,1,2]$; $d=3$ */
        }
	m[k++][R]=d; /* insert denominator for last generator, and advance */
      }
      s+=n; /* advance offset into semisimple elements */
    }
    
    for (i=0; i<grp->g.toraldim; ++i) m[k++][s+i]=1;
    assert(k==n_gen); return res;
  }
}

index find_root(entry* alpha, entry level, simpgrp* g)
{ index i,r=g->lierank; matrix* posr=simp_proots(g);
  for (i=g->level->compon[level-1]; i<g->level->compon[level]; ++i)
    if (eqrow(alpha,posr->elm[i],r)) return i;
  return -1; /* not found */
}

local boolean simp_isroot(entry* alpha, simpgrp* g)
{ index i,r=g->lierank; entry level=0; boolean neg,result=false;
  for(i=0; i<r; ++i) level+=alpha[i]; /* compute level of |alpha| */
  neg=level<0; /* if |neg| holds, |alpha| can only be a negative root */
  if (neg) { level= -level; for(i=0; i<r; ++i) alpha[i]= -alpha[i]; }
  result=find_root(alpha,level,g)>=0;
  if (neg)  for(i=0; i<r; ++i) alpha[i]= -alpha[i]; /* restore |alpha| */
  return result;
}

boolean isroot(entry* alpha)
{ index n_parts=0, i,j;
  if (type_of(grp)==SIMPGRP) return simp_isroot(alpha,&grp->s);
  if (grp->g.ncomp==1) return simp_isroot(alpha,Liecomp(grp,0));
  for (i=0; i<grp->g.ncomp; ++i)
  { simpgrp* g=Liecomp(grp,i); index r=g->lierank;
    for (j=0; j<r; ++j) if (alpha[j]!=0)
      if (n_parts>0 || !simp_isroot(alpha,g)) return false; 
      else { ++n_parts; break; }
    alpha+=r;
  }
  return n_parts==1; /* |alpha| is root if supported on 1 simple component */
}

void checkroot(entry* alpha)
{ if (!isroot(alpha))
  { printarr(alpha,Ssrank(grp)); error (" is not a root.\n"); }
}

boolean isposroot(entry* alpha)
{ index i,s=Ssrank(grp); 
  for (i=0; i<s; ++i)  if (alpha[i]!=0) return alpha[i]>0;
  assert(false); return false; /* to avoid compiler warnings */
}

