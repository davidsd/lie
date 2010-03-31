#include  "lie.h"

bigint* fac(index n)
{ bigint* f=copybigint(one,NULL); while (n>1) f=mul1(f,n--); return f; }

static index n_parts(index n)
{ index i,k,np; entry* c=mkintarray(n+1); /* coefficients */
  if (n>121) error("Too many partitions to generate.\n");
  for (i=0; i<=n; ++i) c[i]=1; /* initialise to ${1\over1-X}$ */
  for (i=2; i<=n; ++i)
    for (k=i; k<=n; ++k) c[k]+=c[k-i]; /* multiply by ${1\over1-X^i}$ */
  np=c[n]; freearr(c); return np;
}

bigint* n_tableaux(entry* lambda, index l)
{ index i,j,k=0; entry* h; bigint* res=copybigint(one,NULL);
  do  if (--l<=0) return one; 
  while (lambda[l]==0); /* find last non-zero part */
  h=mkintarray(lambda[0]); 
  for(j=0; j<lambda[0]; ++j) h[j]=0; /* accumulated column heigths */
  for(i=l; i>=0; --i)
    
    { entry li=lambda[i]-1;
      for(j=0; j<=li; ++j) res=mul1(res,++k); /* part of factorial */
      for(j=0; j<=li; ++j) div1(res,(++h[j])+li-j); /* divide by hook lengths */
    }
  freearr(h); return res;
}

bigint* Classord(entry* kappa, index l)
{ index prev=0,i=0,j,n=0,k,f=1; bigint* x=copybigint(one,NULL);
  while (i<l && (k=kappa[i++])>0)
  { for (j=0; j<k; ++j) x=mul1(x,++n);
      /* extend $\Card\kappa!$ in numerator */
    div1(x,k); /* contribution to $k^{c_k(\kappa)}$ in denominator */
    if (k!=prev) { f=1; prev=k;}  /* this case applies the first time */
    else div1(x,++f); /* contribution to $c_k(\kappa)!$  in denominator */
  }
  return x;
}

index check_part(entry* lambda, index l)
{ index i,sum=0;
  for (i=0; i<l; ++i)
    if (lambda[i]<0) error("Negative entry in partition.\n");
    else if (i>0 && lambda[i]>lambda[i-1])
      error("Increasing entries in partition.\n");
    else sum+=lambda[i];
  return sum;
}

vector* check_tabl(vector* v)
{ vector* shape; entry* t=v->compon,* sh; index i,d,n=v->ncomp, max=0;
  
  for(i=0; i<n; ++i)
    if( (d=t[i])<=0 ) error ("Non-positive number in tableau.\n");
    else if (d>max) max=d;
  shape=mkvector(max); sh=shape->compon;  for(i=0; i<max; ++i) sh[i]=0;
  
  for(i=0; i<n; ++i)
    if( ++sh[d=t[i]-1], d>0 && sh[d]>sh[d-1] )
      Printf("%ld at position %ld ",(long)(d+1),(long)(i+1)),
      error("violates tableau condition.\n");
  return shape;
}

boolean Nextperm(entry* w, index n)
{ index i,j;
  
  { if (n<=1) return false;
    for (i=n-2; w[i]>=w[i+1]; --i) /* find last ascent |(i,i+1)| */
      if (i==0) return false; /* there is none */
    for (j=n-1; w[i]>=w[j]; --j) {} /* find maximal |j>i| with |w[i]<w[j]| */
  }
  swap(&w[i],&w[j]);
    /* now we have |w[i]<=w[i+1]>=@t$\ldots$@>>w[j]>=@t$\ldots$@>>=w[n-1]| */
  for (i++,j=n-1; i<j; i++,j--) swap(&w[i],&w[j]); /* reverse $w[i+1:n-1]$ */
  return true;
}

boolean Nextpart(entry* lambda, index l)
{ index i,avail=0; entry k;
  
  { while (l>0 && lambda[l-1]==0) l--;
    for (i=l-1; i>=0 && lambda[i]==1; --i) ++avail;
    if (i<0) return false; /* quit if 1's only */
  }
  k=(lambda[i++]-=1); ++avail; /* decrease the last part possible */
  
  { while (avail>k) avail-=(lambda[i++]=k); /* distribute |avail| */
    lambda[i++]=avail; /* last part may be smaller */
    while (i<l) lambda[i++]=0; /* fill with zeros */
  }
  return true;
}

boolean Nexttableau(entry* t, index n)
{ index i,r; index* lambda,* skew;
  
  { index c=n;  /* sufficiently large starting value */
    if (n==1) return false;
  
    lambda=alloc_array(index,2*n+1); skew=&lambda[n];
      /* |lambda| and |skew| are |1|-based */
    for (i=2*n; i>0; --i) lambda[i]=0; /* clear |lambda| and |skew| */
    for (i=0; i<n; ++i) ++lambda[t[i]]; /* set $\lambda=\mathop{\rm sh}(T)$ */
  
    for (i=n-1; i>=0; --i)
    { --lambda[r=t[i]]; ++skew[r];
      if (lambda[r]>c) goto found;  else c=lambda[r];
    }
    freearr(lambda); return false; /* final tableau */
  
  found: {}
  }
  
  { do ++r; while (skew[r]==0 || lambda[r]==lambda[r-1]);
      /* find row for first changing entry */
    t[i++]=r; --skew[r]; /* replace |t[i]|, update skew */
    for (r=1; r<=n; ++r) 
      while (skew[r]-->0) t[i++]=r; /* distribute remaining squares */
    freearr(lambda);
  }
  return true;
}

matrix* Permutations(entry* v,index n)
{ index N=1; entry* w=mkintarray(n); copyrow(v,w,n); sortrow(w,n);
  { index i=0,j=n-1; while (i<j) swap(&w[i++],&w[j--]); }
    /* increasing order */
  
  { index i=0, mult=1;
    while (++i<n) { N*=i+1; if (w[i]>w[i-1]) mult=1; else N /= ++mult; }
  }
  { matrix* result=mkmatrix(N,n); index i=0;
    do copyrow(w,result->elm[i++],n); while (Nextperm(w,n));
    freearr(w); return result;
  }
}

matrix* Partitions(index n)
{ matrix* result=mkmatrix(n_parts(n),n);
  if (n>0)
  { entry* lambda=mkintarray(n),** res=result->elm; index i=0,j;
    lambda[0]=n; 
    for(j=1;j<n;j++) lambda[j]=0; /* initialise |lambda| to $[n,0,0,\ldots]$ */
    do copyrow(lambda,res[i++],n); while(Nextpart(lambda,n));
    freearr(lambda);
  }
  return result;
}

matrix* Tableaux(entry* lambda, index l)
{ bigint* nt=n_tableaux(lambda,l); index n=check_part(lambda,l);
  matrix* result=mkmatrix(bigint2entry(nt),n);
  entry** res=result->elm,* t=mkintarray(n);
  freemem(nt);

  
  { index i=0,j,k;
    for (j=1; j<=l; ++j) for (k=lambda[j-1]; k>0; --k) t[i++]=j;
  }
  { index i=0; do copyrow(t,res[i++],n); while(Nexttableau(t,n)); }
  freearr(t); return result;
}

vector* Trans_part(entry* lambda, index l)
{ index i,j=0;
  vector* result=mkvector(l ? lambda[0] : 0); entry* res=result->compon;
  for (i=l-1; i>=0; --i)
    while (j<lambda[i]) res[j++]=i+1;
  return result;
}

entry Sign_part(entry* lambda, index l)
{ index i,s=0; /* count non-zero even parts: */
  for (i=0; i<l && lambda[i]>0; ++i) if (lambda[i]%2==0) s++;
  return s%2 ? -1 : 1;
}

void Robinson_Schensted (entry* P, entry* Q, index n, entry* sigma)
{ index j;
  for(j=n-1; j>=0; --j)
  { entry r=Q[j]; index i=n;
    while ( P[--i]!=r || (P[i]= --r)>0 ) {}
    sigma[j]=i+1; /* permutation values start at~|1| */
  }
}

void Schensted_Robinson (entry* sigma, index n, entry* P, entry* Q)
{ index j;
  for(j=0;j<n;++j) P[j]=0;
  for(j=0; j<n; ++j)
  { index i=sigma[j]-1,r=1;
    if (P[i]==0) P[i]=r;  else error("Not a permutation.\n");
    while (++i<n) if(P[i]==r) P[i]= ++r;
    Q[j]=r;
  }
}

#if 0
static entry Young_char_val(entry* lambda, entry* mu, index l, index m)
{ index i=0; entry sum=0,li, M=m==0 ? 0 : mu[0]; /* largest part of |mu| */
  if (M==0) return 1; /* stop when |mu| is depleted */
  while (i<l && (li=lambda[i])>=M) /* try parts $\geq M$ of |lambda| */
  { index j; entry c=1; /* number of equal parts */
    while (++i<l && lambda[i]==li) ++c; /* move |i| beyond largest part |li| */
    
    { li-=M;
      for(j=i; j<l && lambda[j]>li; ++j) lambda[j-1]=lambda[j];
      lambda[j-1]=li;
    }
    sum+=c*Young_char_val(lambda,mu+1,l,m-1);
    
    { while (--j>=i) lambda[j]=lambda[j-1];
      lambda[j]=li+M;
    }
  }
  return sum;
}
#endif

#if 0
entry Schur_char_val(entry* lambda, entry* mu, index l, index m)
{ index i; entry sum=0;
  while (l>0 && lambda[l-1]==0) --l; /* get reduced form of~|lambda| */
  
  if (l<=1) return 1; /* trivial character */
  if (l>lambda[0]) /* then better work with the transpose partition */
  { vector* tr=Trans_part(lambda,l);
    entry ch=Schur_char_val(tr->compon,mu,lambda[0],m); freemem(tr);
    return Sign_part(mu,m)*ch;
  }
  
  { entry* lambda_prime=mkintarray(4*l)
       ,* sigma=lambda_prime+l,* pos=sigma+l,* nu=pos+l;
      /* 4 length-|l| arrays */
    boolean sg=true; /* positive sign */
    copyrow(lambda,lambda_prime,l);
      /* |lambda| might be alias of |mu|, but |lambda_prime| is not */
    for (i=0; i<l; ++i) pos[i]=sigma[i]=i;
      /* |sigma| is the permutation; |pos| records its swap sequence */
    do
    { copyrow(lambda_prime,nu,l); 
                                  { index i; for (i=1; i<l; ++i) if (nu[i]>nu[i-1]) /* skip most cases */
                                    { entry nui=nu[i]; index j=i;
                                      do nu[j]=nu[j-1]; while (--j>0 && nui>nu[j-1]); nu[j]=nui;
                                    }
                                  }
      sum+= sg ? Young_char_val(nu,mu,l,m) : -Young_char_val(nu,mu,l,m);
      
      { index i=0,j;
        
        do
        { 
          { lambda_prime[i]-=sigma[i];
            if ((j=pos[i])<i) { swap(&sigma[i],&sigma[j]); sg=!sg;}
          }
          do
            if(--j<0) break; /* tried all positions for this |i| */
          while (lambda_prime[i]+sigma[j]<0);
        } while (j<0 && ++i<l);
        if (i==l) break;
        
        do /* now |j>=0| and |sigma[j]| can move validly to |sigma[i]| */
        { 
          { if ((pos[i]=j)<i) { swap(&sigma[i],&sigma[j]); sg=!sg;}
            lambda_prime[i]+=sigma[i]; /* this becomes non-negative */
          }
          if (--i<0) break;
          for (j=i; lambda_prime[i]+sigma[j]<0; --j) {} /* this leaves |j>=0| */
        } while (true);
      }
    } while (true);
    freearr(lambda_prime);
  }
  return sum;
}
#endif

#if 0
matrix* Schur_char(entry* lambda, index l)
{ index i,n=check_part(lambda,l); entry np=n_parts(n);
  matrix* result=mkmatrix(np,n+1); entry** res=result->elm;
  res[0][0]=n; for (i=1; i<n; ++i) res[0][i]=0; i=0;
  while (res[i][n]=Schur_char_val(lambda,res[i],l,n),++i<np)
  { copyrow(res[i-1],res[i],n); Nextpart(res[i],n); }
  return result;
}
#endif

poly* MN_char(entry* lambda, index l)
{ index n=check_part(lambda,l);
  if (n==0) return poly_one(0); /* the character of $\Sym0$ */
  while (lambda[l-1]==0) --l; /* minimise |l| */
  wt_init(n); /* get ready for accumulating contributions to the character */
  { 
    entry* mu=mkintarray(3*n),* save=mu+n,* lambda_prime=save+n;
    int i, j, r, d=lambda[0]+l, k=0; /* sum of leg lengths */
    boolean* edge=alloc_array(boolean,2*d-2),* candidate=edge+d-2;
      /* index |2<=r<d| */
    enum {hor, vert}; /* values used for |edge| */
    
    for (i=0; i<n; ++i) mu[i]=0;
    
    { int r=l-1,c=0; /* current column number */
      for (j=0; r>=0; --r)
      { while (c<lambda[r]) { edge[j++]=hor; ++c; } /* columns of length |r| */
        edge[j++]=vert; /* row |r|, of length |c==lambda[r]| */
      }
    }
    
    for (r=2; r<d; ++r)
    { for (j=0; j+r<d; ++j)
        if (edge[j]==hor && edge[j+r]==vert) break;
      candidate[r]= j+r<d;
    }
    

    
    { i=0; /* index of last entry that was set in~|mu| */
      for (r=d-1; r>1; --r) /* try hooks of size |r| */
        if (candidate[r])
        { recurse: /* recursive starting point */
          
          { for (j=1; j<r; ++j) k+=edge[j]; /* leg length of hook first tried */
            for (j=0; j<d-r; ++j)
            { if (edge[j]==hor && edge[j+r]==vert)
              { edge[j]=vert; edge[j+r]=hor; mu[i]=r; save[i++]=j; goto recurse;
              resume: j=save[--i]; r=mu[i]; mu[i]=0; edge[j]=hor; edge[j+r]=vert;
              }
              k+= edge[j+r]-edge[j+1]; /* adjust |k| for hook tried next */
            }
            while (++j<d) k-= edge[j]; /* restore |k| */
          }
        }
    }
    
    { int r=l,c=0,s=0; /* size of |lambda_prime| */
      for (j=0; r>0; )
        if (edge[j++]==vert) s+=lambda_prime[--r]=c;  else ++c;
        /* build |lambda_prime| from edges */
      for (j=0; j<s; ++j) mu[i++]=1; /* extend |mu| with |s| ones */
      wt_ins(mu,n_tableaux(lambda_prime,l),k%2);
      for (j=0; j<s; ++j) mu[--i]=0; /* remove the ones again */
    }
    if (i>0) goto resume;
    
     
    { freearr(edge); freearr(mu); }
  }
  return wt_collect();
}

bigint* MN_char_val(entry* lambda, entry* mu, index l, index m)
{ bigint* value=null; index n=check_part(lambda,l),m2;
  if (n==0) return one;
  while (lambda[l-1]==0) --l;  while (mu[m-1]==0) --m;
  for (m2=m; m2>0 && mu[m2-1]==1; --m2) {} /* number of parts $\mu_i\geq2$ */
  { entry* save=mkintarray(2*n),* lambda_prime=save+n;
    int i, j, d=lambda[0]+l, k=0; /* sum of leg lengths */
    boolean* edge=alloc_array(boolean,2*d);
    enum {hor, vert};

    
    { int r=l-1,c=0; /* current column number */
      for (j=0; r>=0; --r)
      { while (c<lambda[r]) { edge[j++]=hor; ++c; } /* columns of length |r| */
        edge[j++]=vert; /* row |r|, of length |c==lambda[r]| */
      }
    }
    
    for (i=0; i<m2; ++i)
    { for (j=0; j+mu[i]<d; ++j)
        if (edge[j]==hor && edge[j+mu[i]]==vert) break;
      if (j+mu[i]==d) return null; /* no hook of size |mu[i]| was found */
    }
    
    { i=0; /* index into |mu| */
    recurse:
      if (i<m2)
        
        { int r=mu[i];
          for (j=1; j<r; ++j) k+=edge[j]; /* leg length of hook first tried */
          for (j=0; j+r<d; ++j)
          { if (edge[j]==hor && edge[j+r]==vert)
            { edge[j]=vert; edge[j+r]=hor; save[i++]=j; goto recurse;
            resume: j=save[--i]; r=mu[i]; edge[j]=hor; edge[j+r]=vert;
            }
            k+= edge[j+r]-edge[j+1]; /* adjust |k| for hook tried next */
          }
          while (++j<d) k-= edge[j]; /* restore |k| */
        }
      else
        
        { int r=l,c=0,s=0; /* size of |lambda_prime| */
          for (j=0; r>0; )
            if (edge[j++]==vert) s+=lambda_prime[--r]=c;  else ++c;
            /* build |lambda_prime| from edges */
          value= k%2==0 ? add(value,n_tableaux(lambda_prime,l))
                        : sub(value,n_tableaux(lambda_prime,l)) ;
        }
      if (i>0) goto resume;
    }
    freearr(edge); freearr(save);
  }
  return value;
}

