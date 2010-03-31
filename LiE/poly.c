#include "lie.h"

#ifdef __STDC__
static void error_nvars(index n1,index n2);
#endif

static void error_nvars(index n1,index n2)
{
    Printf("Number of variables in polynomials unequal\n");  
    error("( %ld <-> %ld variables).\n",(long)n1,(long)n2);
}

poly* Pol_from_vec(v)
vector *v;
{
    poly *result =  mkpoly(1,v->ncomp);
    copyrow(v->compon, result->elm[0], v->ncomp); 
    result->coef[0] = one;
    freemem(v);
    return result;
}

entry Degree_pol(poly* p)
{   entry ncols = p->ncols;
    entry nrows = p->nrows;
    entry max=MinEntry; /* will be overwritten for i==0 */
    entry i,j;
    if (p->coef[0]->size==0) return 0; /* degree(0X[0,..,0]) set to 0 */
    for (i = 0 ; i < nrows; ++i)
    { entry sum=0; entry* row = p->elm[i];
      for (j=0 ; j<ncols; ++j) sum += row[j];
      if (sum>max) max = sum;
    }
    return max;
}

/*************************************************************
*  Check that a polynomial has row length r, and normalise   *
*************************************************************/

poly* check_pol(p,r) poly* p; entry r;
{ entry d = p->ncols;
  if (d != r) 
      error("Number of variables in polynomial unequal Lie rank.\n"); 
  if (!issorted(p)) return Reduce_pol(p);
  return p;
}

/* The polynomial arithmetic routines will guarantee that no improper
   0 coefficients will remain, even if they were present in the arguments.
*/

poly* Add_pol_pol(a,b,neg_b) poly* a,* b; boolean neg_b;
{ return Addmul_pol_pol_bin(a,b, neg_b ? minus_one : one); }

poly* Mul_bin_pol(a,b) bigint* a; poly* b; /* modifies b unless shared */
{ entry nrows = b->nrows; entry i; poly* result = private_pol(b);
  setshared(a);
  for (i=0; i<nrows; i++)
  { result->coef[i]=mult(a,b->coef[i]); setshared(result->coef[i]); }
  clrshared(a);
#ifndef argumentsave
  freemem(a); /* don't freepol(b) since either isshared(b) or b==result */
#endif
  return result;
}

poly* Addmul_pol_pol_bin(a,b,c) poly* a,* b; bigint* c; /* a+c*b */
{ index i,j,k; entry len=a->ncols; cmp_tp cmp; poly* result;
  if (len != b->ncols) error_nvars(len,b->ncols);
  if (!c->size) return a;
  if (issorted(a) || issorted(b)) /* then make use of this sorting: */
  { cmpfn_tp compare=set_ordering(cmpfn,len,defaultgrp);
    if (!issorted(a)) a=Reduce_pol(a); else
    if (!issorted(b)) b=Reduce_pol(b); /* now both are sorted */
    if (a->nrows==1 && !a->coef[0]->size) return Mul_bin_pol(c,b);
    if (b->nrows==1 && !b->coef[0]->size) return a;
    result=mkpoly(a->nrows+b->nrows,len); i=j=k=0; setshared(c);
    while(j<b->nrows)
    { while (i<a->nrows && (cmp=compare(a->elm[i],b->elm[j],len))>0)
	if (!a->coef[i]->size) i++; /* skip term with 0 coefficient */ else
	{ result->coef[k]=a->coef[i]; setshared(result->coef[k]);
	  copyrow(a->elm[i++],result->elm[k++],len);
	}
      if (i<a->nrows && cmp==0) /* add compatible terms */
      { result->coef[k]= c==one	      ? add(a->coef[i],b->coef[j])
		       : c==minus_one ? sub(a->coef[i],b->coef[j])
		       : add(a->coef[i],mult(c,b->coef[j]));
	if (!result->coef[k]->size) /* if terms cancel */
	{ freemem(result->coef[k]); i++; j++; }
	else
	{ setshared(result->coef[k]);
	  copyrow(a->elm[i++],result->elm[k++],len); j++;
	}
      }
      else /* i==a->nrows || compare(a->elm[i],b->elm[j],len)<0; */
      if (!b->coef[j]->size) j++; else
      { result->coef[k]= c==one	      ? b->coef[j]
		       : c==minus_one ? sub(null,b->coef[j])
		       : mult(c,b->coef[j]);
	setshared(result->coef[k]);
	copyrow(b->elm[j++],result->elm[k++],len);
      }
    } /* Now all terms of b have been included, but a may have some left */
    while (i<a->nrows)
      if (!a->coef[i]->size) i++; else
      { result->coef[k]=a->coef[i]; setshared(result->coef[k]);
	copyrow(a->elm[i++],result->elm[k++],len);
      }
    clrshared(c);
#ifndef argumentsave
    freepol(a); freepol(b); freemem(c);
#endif
    if (k) { result->nrows=k; setsorted(result); return result; }
    freemem(result); return poly_null(len);
  } /* end of sorted case; if unsorted simply append polynomials and reduce */
  setshared(c); result=mkpoly(a->nrows+b->nrows,len);
  for (i=0; i<a->nrows; i++)
  { result->coef[i]=a->coef[i]; setshared(result->coef[i]);
    copyrow(a->elm[i],result->elm[i],len);
  }
  for (j=0; j<b->nrows; j++,i++)
  { result->coef[i]= c==one ? b->coef[j] : mult(b->coef[j],c);
    setshared(result->coef[i]);
    copyrow(b->elm[j],result->elm[i],len);
  }
  clrshared(c);
#ifndef argumentsave
  freepol(a); freepol(b); freemem(c);
#endif
  return Reduce_pol(result);
}

poly* Div_pol_bin(a,b) poly* a; bigint* b;
{
    entry nrows = a->nrows;
    entry i;
    poly *result =  isshared(a)?
	copypoly(a) : (setshared(a),a);
    setshared(b);
    for (i = 0; i < nrows; i++) {
	result->coef[i] = quotient(a->coef[i],b);
	setshared(result->coef[i]);
    }
    clrshared(b);
#ifndef argumentsave
    freepol(a);freemem(b);
#endif
    return result;
}

poly* Mod_pol_bin(a,b) poly* a; bigint* b;
{
    entry nrows = a->nrows;
    entry i;
    poly *result =  isshared(a)?
	copypoly(a) : (setshared(a),a);
    setshared(b);
    for (i = 0; i < nrows; i++) {
	result->coef[i] = mod(a->coef[i],b);
	setshared(result->coef[i]);
    }
    clrshared(b);
#ifndef argumentsave
    freepol(a);freemem(b);
#endif
    return result;
}

poly
*Mul_pol_int(b,a)
   intcel *a;
   poly *b;
{
    entry nrows = b->nrows, ncols = b->ncols;
    entry d = a->intval;
    entry i,j;
    poly *result =  isshared(b)?
	copypoly(b) : (setshared(b),b);
    for (i = 0; i < nrows; i++) 
    for (j = 0; j < ncols; j++) {
	result->elm[i][j] =  b->elm[i][j] * d;
    }
#ifndef argumentsave
    freepol(b);freemem(a);
#endif
    return result;
}

poly* Div_pol_vec(b,a) poly* b; vector* a;
{ index nrows = b->nrows, ncols = b->ncols;
  entry i,j;
  poly* result = private_pol(b);
  if (ncols != a->ncomp)
    error("Size of vector should equal number of indeterminates.\n");
  for (j=0; j<ncols; j++)
  { entry d=a->compon[j]; if (d==0) error("Division by zero.\n");
    for (i=0; i<nrows; i++) result->elm[i][j]/=d;
  }
  return Reduce_pol(result);
}

poly* Mod_pol_vec(b,a) poly *b; vector *a;
{ index nrows = b->nrows, ncols = b->ncols;
  entry i,j;
  poly* result = private_pol(b);
  if (ncols != a->ncomp)
    error("Size of vector should equal number of indeterminates.\n");
  for (j=0; j<ncols; j++)
  { entry d=labs(a->compon[j]); if (d==0) continue; /* mod 0 is noop */
    for (i=0; i<nrows; i++)
    { if ((result->elm[i][j]%=d)<0) result->elm[i][j]+=d; }
  }
  return Reduce_pol(result);
}

poly *Disjunct_mul_pol_pol(p1, p2)		
poly *p1, *p2;			

/***************************************************************
* Product of polynomials.De sets of free variables of p1 and p2*
* are disjunct.						       *
***************************************************************/

{ index r1= p1->ncols, r2=p2->ncols, n1=p1->nrows,
      n2=p2->nrows;
  entry **e1=p1->elm, **e2=p2->elm, **a;
  index i, j, s=0;
  poly *ans;
  a=(ans=mkpoly(n1*n2,r1+r2))->elm;
  for(i=0;i<n1;i++)
    for(j=0; j<n2; j++)
    { copyrow(e1[i],a[s],r1); copyrow(e2[j],a[s]+r1,r2);
      ans->coef[s]= mult(p1->coef[i],p2->coef[j]);
      setshared(ans->coef[s]);
      s++;
    }
#ifndef argumentsave
    freepol(p1);
    freepol(p2);
#endif
    return(ans);
}

poly *Mul_pol_pol(p1,p2)
    poly *p1, *p2;
{
    index ncols1 = p1->ncols, ncols2 = p2->ncols,
    nrows1 = p1->nrows, nrows2 = p2->nrows;
    index nrows = nrows1 * nrows2;
    poly *result, *garbage;
    index i,j,k = 0,l;

/***************************************************************
*  The wide polynomial is chosen p1			       *
***************************************************************/

    if (ncols1 != ncols2) error_nvars(ncols1,ncols2);

    garbage = result = mkpoly(nrows, ncols1);


/***************************************************************
*  Expand loop						       *
***************************************************************/


    for (i=0; i < nrows1; i++) {
	bigint *c = p1->coef[i];
	for (j=0; j < nrows2; j++) {
	    entry *monom = result->elm[k], *monom2 = p2->elm[j];
	    copyrow(p1->elm[i],monom,ncols1);
	    for (l=0; l<ncols2;l++) monom[l] += monom2[l];
	    result->coef[k] = mult(c,p2->coef[j]);
	    setshared(result->coef[k]);
	    k++;
	}
	/*
	result = Reduce_pol(result);
	k = result->nrows;
	if (result != garbage) error("System warning.\n");
	*/
    }


/***************************************************************
*  Sort and reduce polynomial				       *
***************************************************************/

    result = copypoly(Reduce_pol(result));
    freemem(garbage);

/***************************************************************
*  Freemem arguments					       *
***************************************************************/

#ifndef argumentsave
    freepol(p1);
    freepol(p2);
#endif

    return result;
}
