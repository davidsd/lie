#include  "lie.h"
#define local  static
#define backup(i) (i=(i<=2 ? 0 : i-2))
#define Bh(x,y)   (copyrow(x,v,s),copyrow(y,w,s),Bruhat(v,w))
#define P(x,y,e,d)  (copyrow(x,v,s),copyrow(y,w,s),KL(v,w,e,d,s))

entry simp_inprod(entry* x,entry * y,simpgrp* g)
{ index i,j, r=g->lierank;
  entry* norm=(simp_proots(g),g->root_norm->compon);
  entry** cartan=g->cartan->elm; entry result=0;
  for (j=0; j<r; ++j)
  { entry s=0; for (i=0; i<r; ++i) s += x[i] * cartan[i][j];
    result += s * norm[j]*y[j];
  }
  return result;
}

entry Inprod(entry* x,entry * y)
{ if (type_of(grp)==SIMPGRP) return simp_inprod(x,y,&grp->s);
  { index i,t=0,result=0;
    for (i=0; i<grp->g.ncomp; ++i)
    { simpgrp* g=Liecomp(grp,i);
      result+=simp_inprod(&x[t],&y[t],g); t+=g->lierank;
    }
    return result;
  }
}

entry simp_norm(entry* alpha, simpgrp* g)
{ index i,r=g->lierank; entry level=0,result; boolean neg;
  for (i=0; i<r; ++i) level+=alpha[i]; /* compute level of |alpha| */
  neg=level<0;
  if (neg) { level= -level; for(i=0; i<r; ++i) alpha[i]= -alpha[i]; }
  i=find_root(alpha,level,g); assert(i>=0); result=g->root_norm->compon[i];
  if (neg)  for(i=0; i<r; ++i) alpha[i]= -alpha[i]; /* restore |alpha| */
  return result;
}

entry Norm(entry* alpha)
{ if (type_of(grp)==SIMPGRP) return simp_norm(alpha,&grp->s);
  if (grp->g.ncomp==1) return simp_norm(alpha,Liecomp(grp,0));
  { index i,j,t=0;
    for (i=0; i<grp->g.ncomp; ++i)
    { simpgrp* g=Liecomp(grp,i); index r=g->lierank;
      for (j=0; j<r; ++j) 
	if (alpha[t+j]!=0) return simp_norm(&alpha[t],g);
      t+=r;
    }
    assert(false); return 0; /* zero vector is not a root */
  }
}

entry Cart_inprod(entry* x, entry* alpha)
{ return Inprod(x,alpha)/Norm(alpha); }

local matrix* simp_refl(entry* alpha,simpgrp* g,matrix* m, index offset)
{ index i,r=g->lierank; entry* row=mkintarray(2*r),* col=&row[r];
  
  { index norm_alpha=simp_norm(alpha,g); entry* norm=g->root_norm->compon;
    mulvecmatelm(alpha,g->cartan->elm,row,r,r); /* |alpha| on weight basis */
    copyrow(alpha,col,r);
    for (i=0; i<r; ++i) col[i]=(col[i]*norm[i])/norm_alpha;
    /* $\rf\alpha$ on coroot basis */
  }
  for (i=0; i<r; ++i) add_xrow_to(&m->elm[offset+i][offset],-col[i],row,r);
  freearr(row); return m;
}

matrix* Reflection(entry* alpha)
{ index i,j,t=0; matrix* R=mat_id(Lierank(grp));
  if (type_of(grp)==SIMPGRP) return simp_refl(alpha,&grp->s,R,0);
  if (grp->g.ncomp==1) return simp_refl(alpha,Liecomp(grp,0),R,0);
  for (i=0; i<grp->g.ncomp; ++i)
  { simpgrp* g=Liecomp(grp,i); index r=g->lierank;
    for (j=0; j<r; ++j) 
      if (alpha[t+j]!=0) return simp_refl(&alpha[t],Liecomp(grp,i),R,t);
    t+=r;
  }
  assert(false); return R; /* zero vector is not a root */
}

index simp_make_dominant(entry* lambda, simpgrp* g)
{ index i=0,l=0, r=g->lierank;
  do
  { while (lambda[i]>=0)  if (++i==r) return l;
      /* find first negative entry */
    simp_w_refl(lambda,i,g); ++l;
    backup(i); /* reflect and count, then back up */
  } while(true);
}

index make_dominant(entry* lambda)
{ index i=0,l=0; index s=Ssrank(grp); /* do not affect the toral part */
  if (s==0) return 0; /* this trivial case must be taken out */
  do
  { while (lambda[i]>=0) if (++i==s) return l;
    w_refl(lambda,i); ++l; backup(i);
  } while(true);
}

local index dom_length(entry* lambda)
{ index l,s=Ssrank(grp); entry* x=mkintarray(s);  copyrow(lambda,x,s);
  l=make_dominant(x); freearr(x); return l;
}

vector* Dominant(vector* lambda)
{ vector* result=copyvector(lambda);
  make_dominant(result->compon); return result;
}

index Length(vector* w)
{ index i,s=Ssrank(grp),l=0; entry* x=mkintarray(s),wi;
  for (i=0; i<s; ++i) x[i]=1; /* |x=rho| */
  for (i=0; i<w->ncomp; ++i)
    if ((wi=w->compon[i]-1)>=0)
    { if (x[wi]>0) ++l;  else --l; w_refl(x,wi); }
  freearr(x); return l;
}

vector* Wwordv(entry* lambda, index l)
{ index i=0,j=0,s=Ssrank(grp); entry* x=mkintarray(s);
  vector* result=mkvector(l>=0 ? l : dom_length(lambda));
  copyrow(lambda,x,s); if (s==0) { freearr(x); return result; }
  do
  { while (x[i]>=0)  if (++i==s) { freearr(x); return result; }
    w_refl(x,i); result->compon[j++]= i+1; backup(i);
  } while(true);
}

vector* Wwordm(matrix* m)
{ index i,l=0,s=Ssrank(grp);
  entry* x=mkintarray(2*s),* y=&x[s];
  vector* result;
  for (i=0; i<s; ++i) y[i]=1; /* |y=rho| */
  mulvecmatelm(y,m->elm,x,s,s); /* |x=rho*@t$m\v_{s\times s}$@>| */
  i=0;
  if (s>0) do
  { while (x[i]>=0)  if (++i==s) goto finish;
    w_refl(x,i); w_refl(y,i); ++l;
    backup(i);
  } while(true);
finish: result=Wwordv(y,l); freearr(x);
  
  { matrix* check=Weyl_mat(result); index r=Lierank(grp);
    for (i=0; i<r; ++i) if (!eqrow(m->elm[i],check->elm[i],r))
      error("Matrix does not correspond to a Weyl group element.\n");
    freemem(check);
  }
  return result;
}

vector* Canonical(vector* w)
{ index i,l=w->ncomp,s=Ssrank(grp),wi;
  entry* x=mkintarray(s); vector* result;
  for (i=0; i<s; ++i) x[i]=1; /* |x=rho| */
  for (i=l-1; i>=0; --i) /* apply reverse of |w| to |x| */
    if ((wi=w->compon[i]-1)>=0) { w_refl(x,wi); if (x[wi]>0) l-=2; }
     else --l;
  result=Wwordv(x,l); freearr(x); return result;
}

matrix* Canonicals(matrix* m)
{ index k,n=m->nrows,s=Ssrank(grp); matrix* result=mkmatrix(n,m->ncols);
  entry* x=mkintarray(s);
  for (k=0; k<n; ++k)
  { entry* w= m->elm[k]; index i,l=m->ncols,wi; vector* t;
    for (i=0; i<s; ++i) x[i]=1;
    for (i=l-1; i>=0; --i)
      if ((wi=w[i]-1)<0) --l;  else { if (x[wi]<0) l-=2; w_refl(x,wi); }
    t=Wwordv(x,l); copyrow(t->compon,result->elm[k],l); freemem(t);
    for (i=l; i<m->ncols; ++i) result->elm[k][i]=0; /* fill out with 0's */
  }
  freearr(x); return result;
}

matrix* Filter_dom_m(matrix* m)
{ index i,j,s=Ssrank(grp),count=0; entry* inx=mkintarray(m->nrows);
  for (i=0; i<m->nrows; ++i) /* get indices of dominant rows into |inx| */
  { for (j=0; j<s; ++j)  if (m->elm[i][j]<0) break;
    if (j==s) inx[count++]=i;
  }
  { index r=m->ncols; matrix* result=mkmatrix(count,r);
    for (i=0; i<count; ++i) copyrow(m->elm[inx[i]],result->elm[i],r);
    freearr(inx); return result;
  }
}

poly* Filter_dom(poly* p)
{ index i,j,s=Ssrank(grp),count=0; entry* inx=mkintarray(p->nrows);
  for (i=0; i<p->nrows; ++i) /* get indices of dominant rows into |inx| */
  { for (j=0; j<s; ++j)  if (p->elm[i][j]<0) break;
    if (j==s) inx[count++]=i;
  }
  if (count==0) { freearr(inx); return poly_null(p->ncols); }
  { index r=p->ncols; poly* result=mkpoly(count,r);
    for (i=0; i<count; ++i)
    { copyrow(p->elm[inx[i]],result->elm[i],r);
      result->coef[i]=p->coef[inx[i]]; setshared(result->coef[i]);
    }
    freearr(inx); return result;
  }
}

local entry* fix_vec (vector* I, index s)
{ index i; entry* x=mkintarray(s);
  for (i=0; i<s; i++) x[i]=1; /* initialise to positive entries */
  for (i=0; i<I->ncomp; i++)
  { entry e=I->compon[i];
    if (e<=0 || e>s) error("Reflection %ld is out of range.\n",(long)e);
    x[e-1]=0; /* now reflection $r_e$ stabilises |x| */
  }
  return x;
}

vector* L_red(vector* L, entry* w, index lw)
{ index i,s=Ssrank(grp),l=lw;
    /* current length of word; decreases during reduction */
  entry* x=fix_vec(L,s),* y=mkintarray(s); /* current vector and temporary */
  for (i=0; i<lw; i++)
    
    { entry wi=w[i]-1; /* internally, reflections are numbered $0,\ldots,s-1$ */
      if (w[i]==0) --l; /* ignore zero entry */
      else if (x[wi]==0) { w[i]=0; --l; } /* |wi| stabilises |x|, delete it */
      else if (w_refl(x,wi),x[wi]>0)
        /* act on~$x$ with $w_i$; test if length decreased */
        
        { index j=i;  copyrow(x,y,s); /* make a temporary copy of~|x| */
          do
            if (w[--j]!=0)
              if (y[w[j]-1]<0) w_refl(y,w[j]-1);  else break;
          while (true);
          w[j]=w[i]=0; l-=2; /* cancel reflections by exchange condition */
        }
    }
  freearr(x); freearr(y);
  
  { vector* result=mkvector(l); entry* res=result->compon;
    for (i=0; i<lw; i++)  if (w[i]!=0) *res++=w[i];
    return result;
  }
}

vector* Reduce (vector* v)
{ vector* empty=mkvector(0),* result; index lw=v->ncomp;
  entry* w=mkintarray(lw);
  copyrow(v->compon,w,lw); result=L_red(empty,w,lw);
  freemem(empty); freearr(w); return result;
}

vector* LR_red(vector* L,entry* w,index lw,vector* R)
{ 
  { vector* t=L_red(L,w,lw);
    lw=t->ncomp; copyrow(t->compon,w,lw); freemem(t);
  }
  { index i,l=lw; entry* x=fix_vec(R,Ssrank(grp));
    for (i=lw-1; i>=0; i--) /* see if |wi| can be eliminated */
    { entry wi=w[i]-1;
      if (x[wi]==0) { w[i]=0; --l; } /* |wi| stabilises |x|, delete it */
      else w_refl(x,wi); /* we have |x[wi]>0|; the length increases */
    }
    freearr(x);
    
    { vector* result=mkvector(l); entry* res=result->compon;
      for (i=0; i<lw; i++)  if (w[i]!=0) *res++=w[i];
      return result;
    }
  }
}

local boolean Bruhat(entry* x,entry* y) /* modifies arguments */
{ index i,s=Ssrank(grp), d; entry* delta=mkintarray(s);
  
  { matrix* ic=Icartan(); index dc=Detcartan(); entry* t=mkintarray(s);
    subrow(x,y,t,s); mulvecmatelm(t,ic->elm,delta,s,s);
    freemem(ic); freearr(t);
    for (d=0, i=0; i<s; ++i)
      if ((delta[i]/=dc)<0) { freearr(delta); return false; } 
      else d+= delta[i];
  }
  for (i=0; d>0; backup(i))
  { while (y[i]>=0) ++i; /* find first |y[i]<0|; it exists */
    
    { w_refl(y,i); delta[i]-=y[i]; d-=y[i];
      if (x[i]<0) { w_refl(x,i); delta[i]+=x[i]; d+=x[i]; }
    }
    if (delta[i]<0) { freearr(delta); return false; }
      /* because we no longer have $y\prec x$ */
  }
  freearr(delta);
  return true; /* |delta==0|, so |x| and |y| have become equal */
}

boolean Bh_leq(vector* v,vector* w)
{ index i,s=Ssrank(grp); entry* x=mkintarray(2*s),* y=&x[s]; boolean result;
  for (i=0; i<s; ++i) x[i]=y[i]=1;
  Waction(x,v); Waction(y,w); /* $x\K\rho\cdot v$ and $y\K\rho\cdot w$ */
  result = Bruhat(x,y); freearr(x); return result;
}

matrix* Bh_desc(entry* w, index l) /* |w| is reduced of length |l| */
{ entry* inx; index s=Ssrank(grp),n_desc=0;
  if (l==0) return mkmatrix(0,0); /* avoid attempting a $0\times-1$ matrix */
  
  { index i,j; entry* x=(inx=mkintarray(l+s),&inx[l]);
    for (i=0; i<l; ++i)
    { for (j=0; j<s; ++j) x[j]=1; /* |x=rho| */
      for (j=0; j<l; ++j)  if (j!=i)
      { entry wj=w[j]-1; if (x[wj]>0) w_refl(x,wj); else break; }
      if (j==l) inx[n_desc++]=i; /* if reduced, record |i| */
    }
  }
  
  { matrix* result=mkmatrix(n_desc,l-1); index i,j;
    for (i=0; i<n_desc; ++i)
    { entry* row= result->elm[i];
      for (j=0; j<l; ++j) if (j!=inx[i]) *row++=w[j];
    }
    freearr(inx); return result;
  }
}

matrix* Bh_desc_rel(entry* w, index l, entry* lwb)
{ entry* inx; index s=Ssrank(grp),n_desc=0;
  if (l==0) return mkmatrix(0,0); /* avoid attempting a $0\times-1$ matrix */
  
  { index i,j; entry* x=(inx=mkintarray(l+2*s),&inx[l]), *y=&x[s];
    for (i=0; i<l; ++i)
    { for (j=0; j<s; ++j) x[j]=1; /* |x=rho| */
      for (j=0; j<l; ++j)  if (j!=i)
      { entry wj=w[j]-1; if (x[wj]>0) w_refl(x,wj); else break; }
      if (j==l && (copyrow(lwb,y,s),Bruhat(y,x))) inx[n_desc++]=i;
    }
  }
  
  { matrix* result=mkmatrix(n_desc,l-1); index i,j;
    for (i=0; i<n_desc; ++i)
    { entry* row= result->elm[i];
      for (j=0; j<l; ++j) if (j!=inx[i]) *row++=w[j];
    }
    freearr(inx); return result;
  }
}

local void Bh_descs(matrix** m, entry* lwb, index s)
{ vector* word=Wwordv((*m)->elm[0],-1); entry* w=word->compon;
  index i,j,k,l=word->ncomp,n_desc=0;
  matrix* descs=mkmatrix(((*m)->nrows)*l+1,s);
  entry* x=mkintarray(2*s),* y=&x[s]; /* working arrays */
  for (k=0; ; w=(word=Wwordv((*m)->elm[k],l))->compon)
  { 
    for (i=0; i<l; ++i)
    { for (j=0; j<s; ++j) x[j]=1; /* |x=rho| */
      for (j=l-1; j>=0; --j)  if (j!=i)
      { entry wj=w[j]-1; if (x[wj]>0) w_refl(x,wj); else break; }
      if (j<0 && (copyrow(lwb,y,s),copyrow(x,descs->elm[n_desc],s),Bruhat(y,x)))
        ++n_desc;
    }
 
    freemem(word);
    if (++k==(*m)->nrows) break;
  }
  descs->nrows=n_desc;
  freearr(x); freemem(*m); *m=Unique(descs,cmpfn);
}

static poly* q,* q_1;
   /* |q| respectively |q-1|, when initialised */
void make_q (void)
{ (q=mkpoly(1,1))->elm[0][0]=1; *q->coef=one; setshared(q);
    /* $X^{[1]}$ means |q| */
  q_1= Add_pol_pol(q,poly_one(1),true); setshared(q_1);
    /* $X^{[1]}-X^{[0]}$ means |q-1| */
}
void clear_q (void) 
{ clrshared(q); freepol(q); clrshared(q_1); freepol(q_1); }

local poly* R(entry* x, entry* y, index k, index d, index s)
  /* modifies |x| and |y| */
{ index i; entry* v=mkintarray(2*s),* w=&v[s];
  poly* sum=poly_null(1);
  while (d>0) /* |d==length(y)-length(x)| */
  { if (!Bh(x,y)) { freearr(v);return sum;}
    
    for (i=0; ; backup(i))
    { do { if (x[i]<0 && y[i]<0) break;} while (++i<s);
      if (i==s) break;
      w_refl(x,i); w_refl(y,i);
    }
    
    for (i=0; y[i]>=0; ++i) {}
    
    { w_refl(y,i); copyrow(x,v,s); copyrow(y,w,s);
      sum=Add_pol_pol(sum,Mul_pol_pol(q_1,R(v,w,k,d-1,s)),false);
    }
    w_refl(x,i); d-=2; ++k;
      /* repeat with $x\K xs_i$, $y\K ys_i$, adjusted~|d|,
         and increased~|k| */
  }
  freearr(v);
  if (d==0 && eqrow(x,y,s)) /* if |x==y|, add final $q^k$ */
  { poly* q_k=copypoly(q); q_k->elm[0][0]=k;
    sum= Add_pol_pol(sum,q_k,false);
  }
  return sum;
}

poly* Rpoly(vector* x,vector* y)
{ index i,s=Ssrank(grp); entry* v=mkintarray(2*s),* w=&v[s]; poly* result;
  for (i=0; i<s; ++i) v[i]=w[i]=1;
  Waction(v,x); Waction(w,y);
  result=R(v,w,0,dom_length(w)-dom_length(v),s); freearr(v); return result;
}

local poly* KL(entry* x, entry* y, index e, index d, index s)
{ index i; entry* v=mkintarray(2*s),* w=&v[s]; poly* result;
  if (d<0 || !Bh(x,y)) { freearr(v); return poly_null(1); }
  if (d<3)  triv_result: /* return $q^e$ when |l(y)-l(x)<3| */
    { poly* qe=mkpoly(1,1); qe->coef[0]=one; qe->elm[0][0]=e;
      freearr(v); return qe;
    }
  
  do
  { for (i=0; ; backup(i))
    { do { if (x[i]>0 && y[i]<0) break; } while (++i<s);
      if (i==s) break;
      w_refl(x,i); if(--d<3) goto triv_result;
    }
    for (i=0; i<s; ++i)
      if (y[i]<0) /* then also |x[i]<0|, because of the loop above */
        if (w_refl(y,i),Bh(x,y)) w_refl(y,i);  else { w_refl(x,i); break; }
  } while (i<s); 
  for (i=0; y[i]>=0; ++i) {}
    /* find $i$ with $ys_i<y$; it exists since |y!=e| */
  w_refl(y,i); --d; /* put $y\K ys_i$, maintain |d==l(y)-l(x)| */

  
  { matrix* level=mkmatrix(1,s); index j,c=1; /* |c==(l(y)-l(z)+1)/2| */
    
    { poly* Pxy=P(x,y,e+1,d);
      w_refl(x,i); result=Add_pol_pol(P(x,y,e,d+1),Pxy,false); w_refl(x,i);
    }
    copyrow(y,level->elm[0],s); Bh_descs(&level,x,s);
    --d; /* |d==l(z)-l(x)| */
    do
    { for (j=0; j<level->nrows; ++j)
      { entry* z=level->elm[j];
        if (z[i]<0) 
                   { poly* Pzy=P(z,y,0,2*c-1);
                     if (Pzy->nrows!=0 && Pzy->elm[0][0]==c-1 && Pzy->coef[0]->size!=0)
                       /* |mu(z,y)!=0| */
                     { bigint* mu=sub(null,Pzy->coef[0]); freepol(Pzy);
                       result=Addmul_pol_pol_bin(result,P(x,z,e+c,d),mu);
                     }
                   }
  
      }
      if (d<2) { freemem(level); break;} /* we cannot go down 2 more levels */
      Bh_descs(&level,x,s); Bh_descs(&level,x,s); d-=2; ++c;
    } while(true);
  }
  freearr(v); return result;
}

poly* KLpoly(vector* x,vector* y)
{ index i,s=Ssrank(grp); entry* v=mkintarray(2*s),* w=v+s; poly* result;
  for (i=0; i<s; ++i) v[i]=w[i]=1;
  Waction(v,x); Waction(w,y);
  result=KL(v,w,0,dom_length(w)-dom_length(v),s); freearr(v); return result;
}

