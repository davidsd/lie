#define local  static
#include  "lie.h"

void simp_w_refl(entry* w, index i, simpgrp* g)
{ index r=g->lierank; entry wi=w[i];
  if (wi==0) return; /* weight is on reflection hyperplane */
  { if (i>0) w[i-1]+=wi; } w[i]= -wi;
  { if (i<r-1) w[i+1]+=wi; } /* $A_n$ action */
  switch (g->lietype)
  {	 case 'B':  if (i==r-2) w[i+1]+=wi; 
  break; case 'C':  if (i==r-1) w[i-1]+=wi; 
  break; case 'D': if (i>=r-3) if (i==r-1) { w[i-2]+=wi; w[i-1]-=wi; }
			       else if (i==r-2) w[i+1]-=wi;
                                    else w[i+2]+=wi; 
  break; case 'E': if (i<4)
    { if (i<2) { w[i+1]-=wi; w[i+2]+=wi; }  else w[i-2]+=wi;
      if (i==1 || i==2) w[i-1]-=wi;
    } 
  break; case 'F':  if (i==1) w[2]+=wi; 
  break; case 'G':  if (i==1) w[0]+=2*wi;
  }
}

local void simp_rt_refl (entry* w,index i,simpgrp* g)
{ index r=g->lierank; entry c= -w[i]+(i>0?w[i-1]:0)+(i<r-1?w[i+1]:0);
  switch (g->lietype)
  {	 case 'B':  if (i==r-1) c+=w[i-1];
        /* long root neighbour of short root contributes twice */
  break; case 'C':  if (i==r-2) c+=w[i+1]; /* idem */
  break; case 'D': if (i>=r-3)
   /* correct for different adjacencies at end of diagram */
         if (i==r-1) c+=w[i-2]-w[i-1];
           /* attach node $r$ to $r-2$ rather than to $r-1$ */
	 else if (i==r-2) c-=w[i+1]; /* detach $r-1$ from $r$ */
              else c+=w[i+2]; /* and attach $r-2$ to $r$ */
  break; case 'E': if (i<4) /* similar adjustments here */
    {   if (i<2) c+=w[i+2]-w[i+1];
           /* nodes 1,2 neighbour 3,4 respectively rather than 2,3 */
        else c+=w[i-2]; /* for nodes 3,4, add neighbour 1,2, respectively */
        if (i==1 || i==2) c-=w[i-1];
          /* and for nodes 2,3, remove neighbour 1,2, respectively */
     }
  break; case 'F':  if (i==2) c+=w[1]; /* like types $B_n$, $C_n$ */
  break; case 'G':  if (i==0) c+=2*w[1];
   /* now long root contributes 3 times */
  }
  w[i]=c;
}

void w_refl(entry* lambda, index wi)
{ if (type_of(grp)==SIMPGRP) simp_w_refl(lambda,wi,&grp->s);
  else if (simpgroup(grp)) simp_w_refl(lambda,wi,Liecomp(grp,0));
  else
  { index i,d,offset=0;
    for (i=0; wi>=(d=Liecomp(grp,i)->lierank); ++i) { offset+=d; wi-=d; }
    simp_w_refl(lambda+offset,wi,Liecomp(grp,i));
  }
}

void Waction(entry* lambda, vector* word)
{ index i; entry* w=word->compon;
  for (i=0; i<word->ncomp; ++i) 
    if (w[i]!=0) w_refl(lambda,w[i]-1);
}

void Wrtaction(entry* alpha, vector* word)
{ index i; entry* w=word->compon;
  for (i=0; i<word->ncomp; ++i) if(w[i]!=0)
    { index wi=w[i]-1;
      if (type_of(grp)==SIMPGRP) simp_rt_refl(alpha,wi,&grp->s);
      else if (simpgroup(grp)) simp_rt_refl(alpha,wi,Liecomp(grp,0));
      else
      { index j,d,offset=0;
	for (j=0; wi>=(d=Liecomp(grp,j)->lierank); ++j)
          { offset+=d; wi-=d; }
	simp_rt_refl(alpha+offset,wi,Liecomp(grp,j));
      }
    }
}

matrix* simp_Weylmat(vector* word, simpgrp* g)
{ index i,j,r=g->lierank; matrix* res=mkmatrix(r,r);
  entry** m=res->elm,* w=word->compon;
  for (i=0; i<r; ++i)
  { for (j=0; j<r; ++j) m[i][j]= i==j;
    for (j=0; j<word->ncomp; ++j) 
      if(w[j]!=0) simp_w_refl(m[i],w[j]-1,g);
  }
  return res;
}

matrix* Weyl_mat(vector* word)
{ index i,j,r=Lierank(grp); matrix* res=mkmatrix(r,r); entry** m=res->elm;
  for (i=0; i<r; ++i)
  { for (j=0; j<r; ++j) m[i][j]= i==j;
    Waction(m[i],word);
  }
  return res;
}

matrix* Weyl_rt_mat(vector* word)
{ index i,j,r=Ssrank(grp); matrix* res=mkmatrix(r,r); entry** m=res->elm;
  for (i=0; i<r; ++i)
  { for (j=0; j<r; ++j) m[i][j]= i==j;
    Wrtaction(m[i],word);
  }
  return res;
}

local bigint* simp_worder(bigint* num, simpgrp* g)
{ index i,r=g->lierank; bigint* result=num;

  i=r;  while (i>1) result=mul1(result,i--);
  switch (g->lietype)
  {	 case 'A':  result=mul1(result,r+1); 
  break; case 'B':
	 case 'C':  for (i=0; i<r; ++i) result = mul1(result,2); 
  break; case 'D':  for (i=1; i<r; ++i) result = mul1(result,2); 
  break; case 'E':  result = mul1(result,r==6 ? 72 : r==7 ? 576 : 17280); 
  break; case 'F':  result = mul1(result,48); 
  break; case 'G':  result = mul1(result,6);
  }
  return result;
}

bigint* Worder(object grp)
{ index i; bigint* result=copybigint(one,NULL);
  if (type_of(grp)==SIMPGRP) return simp_worder(result,&grp->s);
  for (i=0; i<grp->g.ncomp; ++i) result = simp_worder(result,Liecomp(grp,i));
  return result;
}

bigint* sub_Worder(vector* v)
{ index i,j,s=Ssrank(grp), n=v->ncomp; matrix* roots=mkmatrix(n,s);
  entry** m=roots->elm; group* h; bigint* result;
  if (n==0) { freemem(roots); return one; }
  for (i=0; i<n; ++i) /* select rows od an identity matrix */
  { entry* mij= *m++,vi=v->compon[i]-1;
    for (j=0; j<s; ++j) *(mij++)= (j==vi);
  }
  h=Carttype(roots); freemem(roots);
  result= Worder((object)h); freemem(h); return(result);
}

local bigint* simp_stabsize(entry* v, simpgrp* g)
{ object sav_grp=grp; vector* I; bigint* result;
  index i,nz=0,r=g->lierank;
  for (i=0; i<r; ++i)  if (v[i]==0) nz++; /* count non-zero coordinates */
  if (nz==0) return one;
  if (nz==r) return simp_worder(copybigint(one,NULL),g);
  I=mkvector(nz);
  for (i=0,nz=0; i<r; ++i)  if (v[i]==0) I->compon[nz++]=i+1;
  grp=(object)g; result=sub_Worder(I); grp=sav_grp;
  freemem(I); return result;
}

bigint* simp_worbitsize(entry* w, simpgrp* g)
 /* |w| is assumed to be dominant */
{ return quotient(simp_worder(copybigint(one,NULL),g),simp_stabsize(w,g)); }

bigint* Orbitsize(entry* w)
{ index i,d,s=Ssrank(grp); entry* x=mkintarray(s),* y=x; bigint* result=one;
  copyrow(w,x,s); make_dominant(x);
  if (type_of(grp)==SIMPGRP) return simp_worbitsize(x,&grp->s);
  for (i=0; i<grp->g.ncomp; ++i,y+=d)
  { simpgrp* g=Liecomp(grp,i); d=g->lierank;
    result=mult(result,simp_worbitsize(y,g));
  }
  freearr(x); return result;
}

matrix* Weyl_orbit(entry* v, matrix** orbit_graph)
{ index i,j,k,r=Lierank(grp),s=Ssrank(grp);
  matrix* result; entry** m;
  index level_start=0, level_end=1, cur=1;
  
  { entry* lambda=mkintarray(r); 
    copyrow(v,lambda,r); make_dominant(lambda);
    result=mkmatrix(bigint2entry(Orbitsize(lambda)),r);
    copyrow(lambda,result->elm[0],r); freearr(lambda);
    if (orbit_graph!=NULL) *orbit_graph=mkmatrix(result->nrows,s);
  }
  m=result->elm;
  while (level_start<level_end)
  { 
    for (k=level_start; k<level_end; ++k)
      for (i=0; i<s; ++i)
        if (m[k][i]>0) /* only strictly cross walls, and from dominant side */
        { w_refl(m[k],i);
          for (j=level_end; j<cur; ++j)
            if (eqrow(m[k],m[j],s)) break;
          if (orbit_graph!=NULL)
          { (*orbit_graph)->elm[k][i]=j; (*orbit_graph)->elm[j][i]=k; }
          if (j==cur)
    	{ assert(cur<result->nrows);
              copyrow(m[k],m[cur++],r);
            }
          w_refl(m[k],i);
        }
        else if (m[k][i]==0 && orbit_graph!=NULL) (*orbit_graph)->elm[k][i]=k;
    level_start=level_end; level_end=cur;
  }
  return result;
}

matrix* Weyl_root_orbit(entry* v)
{ index i,j,r=Lierank(grp),s=Ssrank(grp);
  entry* x=mkintarray(r); matrix* orbit, *result; entry** m;
  index dc=Detcartan();
  mulvecmatelm(v,Cartan()->elm,x,s,r);
  orbit=Weyl_orbit(x,NULL);
result=mkmatrix(orbit->nrows,s); m=result->elm;
  mulmatmatelm(orbit->elm,Icartan()->elm,m,orbit->nrows,s,s);
  freemem(orbit);
  for (i=0; i<result->nrows; ++i) 
    for (j=0; j<s; ++j) m[i][j]/=dc;
  return result;
}

poly* Worbit_p(poly* p)
{ index i,k=0,l=0,r=p->ncols; poly* result; entry** res;
  p=copypoly(p); for (i=0; i<p->nrows; ++i) make_dominant(p->elm[i]);
  Reduce_pol(p);
  for (i=0; i<p->nrows; ++i)
    if ((l += bigint2entry(Orbitsize(p->elm[i])))<0)
      error ("That's too large an orbit");
  result=mkpoly(l,p->ncols); res=result->elm;
  for (i=0; i<p->nrows; ++i)
  { index j; matrix* orbit=Weyl_orbit(p->elm[i],NULL); entry** x=orbit->elm;
    for (j=0; j<orbit->nrows; ++j)
    { result->coef[k]=p->coef[i]; setshared(p->coef[i]);
      copyrow(*x++,res[k++],r);
    }
    freemem(orbit);
  }
  assert(k==result->nrows);
  return result; /* not sorted, but rows are unique */
}

poly* alt_Wsum(poly* p)
{ index i,k=0,r=p->ncols; poly* result; entry** res,*rho=mkintarray(r);
  p=Alt_dom(p); for (i=0; i<r; ++i) rho[i]=1;
  for (i=0; i<p->nrows; ++i) add_xrow_to(p->elm[i],1,rho,r);
  result=mkpoly(p->nrows*bigint2entry(Worder(grp)),r); res=result->elm;
  for (i=0; i<p->nrows; ++i)
  { index j,l; matrix* orbit=Weyl_orbit(p->elm[i],NULL); entry** x=orbit->elm;
    bigint* c=p->coef[i],* min_c=sub(null,c);
    for (j=0; j<orbit->nrows; ++j)
    { subrow(*x,rho,res[k],r); l=make_dominant(*x++)%2;
      result->coef[k]= l ? min_c : c; setshared(result->coef[k]); ++k;
    }
    freemem(orbit);
  }
  freearr(rho);
  assert(k==result->nrows);
  return result; /* not sorted, but rows are unique */
}

