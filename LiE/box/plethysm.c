#include  "lie.h"

poly* Adams(index n,poly* p)
{ if (n==1) return p; /* avoid work in this trivial case */
  { index i,j, r=Lierank(grp); poly* dom_ch=Domchar_p(p);
    for (i=0; i<dom_ch->nrows; ++i)
      for (j=0; j<r; j++) dom_ch->elm[i][j] *= n;
    { poly* result=Vdecomp(dom_ch); freepol(dom_ch); return result; }
  }
}

poly* SAtensor(boolean alt,index m,poly* p)
{ index n,r=Lierank(grp); poly** adams,** q,* result;
  if (m==0) return poly_one(r);  else if (m==1) return p;

  adams=alloc_array(poly*,m+1); 
  for (n=1; n<=m; ++n) adams[n]=Adams(n,p);
  q=alloc_array(poly*,m+1);
  q[0]=poly_one(r);
  for (n=1; n<=m; ++n)
  { 
    { index i; q[n]=Tensor(p,q[n-1]); /* the initial term of the summation */
      for (i=2; i<=n; ++i) q[n] =
        Add_pol_pol(q[n],Tensor(adams[i],q[n-i]),alt&&i%2==0);
    }
    
    { index i; bigint* big_n=entry2bigint(n);  setshared(big_n);
      for (i=0; i<q[n]->nrows; ++i)
      { bigint** cc= &q[n]->coef[i]
             ,* c= (clrshared(*cc),isshared(*cc)) ? copybigint(*cc,NULL) : *cc;
        *cc=divq(c,big_n); setshared(*cc);
        
        { if (c->size != 0)
            error("Internal error (SAtensor): remainder from %ld.\n" ,(long)n);
          freemem(c);
        }
      }
      clrshared(big_n); freemem(big_n);
    }
  }
  result=q[m];
{ for (n=1; n<=m; ++n) freepol(adams[n]); } freearr(adams);
{ for (n=0; n<m; ++n)  freepol(q[n]); } freearr(q);
 return result;
}

poly* Plethysm(entry* lambda,index l,index n,poly* p)
{ if (n==0) return poly_one(Lierank(grp));  else if (n==1) return p;

  { index i,j;
    poly* sum= poly_null(Lierank(grp)),**adams=alloc_array(poly*,n+1);
    poly* chi_lambda=MN_char(lambda,l);
    for (i=1; i<=n; ++i) { adams[i]=Adams(i,p); setshared(adams[i]); }
    
    for (i=0;i<chi_lambda->nrows;i++)
    { entry* mu=chi_lambda->elm[i]; poly* prod=adams[mu[0]],*t;
      for (j=1; j<n && mu[j]>0; ++j)
        { t=prod; prod=Tensor(t,adams[mu[j]]); freepol(t); }
      sum= Addmul_pol_pol_bin(sum,prod,mult(chi_lambda->coef[i],Classord(mu,n)));
    }
    freemem(chi_lambda);
    setshared(p); /* protect |p|; it coincides with |adams[1]| */
    for (i=1; i<=n; ++i)
      { clrshared(adams[i]); freepol(adams[i]); }  freearr(adams);
  clrshared(p);

    
    { bigint* fac_n=fac(n);  setshared(fac_n); /* used repeatedly */
      for (i=0; i<sum->nrows; ++i)
      { bigint** cc= &sum->coef[i]
             ,* c= (clrshared(*cc),isshared(*cc)) ? copybigint(*cc,NULL) : *cc;
        *cc=divq(c,fac_n); setshared(*cc);
        if (c->size!=0) error("Internal error (plethysm).\n");  else freemem(c);
      }
      clrshared(fac_n); freemem(fac_n);
    }
    return sum;
  }
}

