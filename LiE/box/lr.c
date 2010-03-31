#include  "lie.h"

poly* LR_tensor_irr(entry* lambda,entry * mu, index n)
{ index i,j;
	entry* nu; entry** T;
  if (n==0) return poly_one(0);
  
  { nu=&mkintarray(n+1)[1]; copyrow(lambda,nu,n); nu[-1]=lambda[0]+mu[0];
  T=alloc_array(entry*,n+1);
    for (i=0;i<=n;++i) /* allocate row |T[i]| and place sentinel before it */
    	{ T[i]= &mkintarray(mu[i==0?0:i-1]+1)[1]; T[i][-1]=n-1-i; }
    for (i=0,j=mu[0]-1; j>=0; --j)
    { while (i<n && mu[i]>j) ++i; /* find first |i| with |mu[i]<=j| */
      T[i][j]=-1; /* place sentinel at bottom of column |j| */
    } 
  }
  wt_init(n); /* prepare to collect terms with exponents of size~|n| */

  
  { j=-1;  for (i=n-1; i>0 && mu[i]==0; --i) {} /* move to initial position */
  recurse: /* recursive starting point; */
    if (++j>=mu[i] &&(j=0,--i<0)) /* move to next empty position, if any */
      wt_ins(nu,one,false); /* if not, |T| is full; contribute |nu| once */
  else
    
    { index k= T[i+1][j]; entry prev= nu[k];
      do
      { while (nu[++k]==prev) {} /* find next |k| with |nu[k]<nu[@t$k'$@>]| */
        ++nu[T[i][j]=k]; goto recurse;
          /* insert |k| into |T| and extend partition |nu|; recurse */
        resume: prev= --nu[k=T[i][j]];
          /* restore |k| and |nu|; set |prev=nu[k]| */
      } while (prev>nu[T[i][j-1]]);
          /* if so, there are still corners of |nu| to try */
    }
    if (j==0) j= ++i<n?mu[i]:0; /* return to end of row below if necessary */
    if (--j>=0) goto resume; /* do return jump unless empty row is reached */
  }
  
  { --nu; freearr(nu);
    for (i=0;i<=n;i++) { entry* t=&T[i][-1]; freearr(t); }
    freearr(T);
  }
  return wt_collect(); /* return sum of all contributed terms */
}

poly* LR_tensor(poly* p,poly* q)
{ index i,j,n=p->ncols; poly* res=poly_null(n);
  for (i=0; i<p->nrows; ++i)
    for (j=0; j<q->nrows; ++j)
      res=Addmul_pol_pol_bin(res,LR_tensor_irr(p->elm[i],q->elm[j],n)
			    ,mult(p->coef[i],q->coef[j]));
  return res;
}

vector* From_Part_v (entry* lambda, index n)
{ index i; vector* result=mkvector(n-1); entry* res=result->compon;
  for (i=0; i<n-1; ++i) res[i]=lambda[i]-lambda[i+1];
  return result;
}

matrix* From_Part_m (entry** lambda, index n_rows, index n)
{ index i,j; matrix* result=mkmatrix(n_rows,n-1); entry** res=result->elm;
  for (i=0; i<n_rows; ++i)
    for (j=0; j<n-1; ++j) res[i][j]=lambda[i][j]-lambda[i][j+1];
  return result;
}

poly* From_Part_p (poly* p)
{ index i,j,n_rows=p->nrows,n=p->ncols; poly* result=mkpoly(n_rows,n-1);
  entry** lambda=p->elm; entry** res=result->elm;
  for (i=0; i<n_rows; ++i)
  { result->coef[i]=p->coef[i]; setshared(p->coef[i]); /* copy coefficient */
    for (j=0; j<n-1; ++j) res[i][j]=lambda[i][j]-lambda[i][j+1];
  }
  return Reduce_pol(result);
}

vector* To_Part_v (entry* wt,index n)
{ index i=n; vector* result=mkvector(n+1); entry* lambda=result->compon;
  entry sum=0;
  while (lambda[i]=sum, --i>=0) sum+=wt[i];
  return result;
}

matrix* To_Part_m (entry** wt, index n_rows, index n)
{ index i; matrix* result=mkmatrix(n_rows,n+1); entry** lambda=result->elm;
  for (i=0; i<n_rows; ++i)
  { index j=n; entry sum=0;
    while (lambda[i][j]=sum, --j>=0) sum+=wt[i][j];
  }
  return result;
}

poly* To_Part_p (poly* p)
{ index i,n_rows=p->nrows,n=p->ncols; entry** wt=p->elm;
  poly* result=mkpoly(n_rows,n+1); entry** lambda=result->elm;
  for (i=0; i<n_rows; ++i)
  { index j=n; entry sum=0; 
    result->coef[i]=p->coef[i]; setshared(p->coef[i]);
    while (lambda[i][j]=sum, --j>=0) sum+=wt[i][j];
  }
  return Reduce_pol(result);
}

