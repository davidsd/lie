/* Static5 is engaged with type BIGINT and ggd(vec) */
#include "lie.h"

#ifdef __STDC__
static entry gcd(entry x,entry y);
static entry abs_minimum(vector* v_vec);
static boolean equal_elements(vector* v_vec);
#endif

static bigint* bin_from_int(i) intcel* i;
{ entry k = i->intval; freemem(i); return entry2bigint(k); }

static intcel* int_from_bin(b) bigint* b;
{ entry k = bigint2entry(b); freemem(b); return mkintcel(k); }

static object
vid_factor_bin(b) object
b;
{
    return (object) Factor((bigint *) b);
}

/* Transform a vector into a matrix with the same components as the vector,
   when read by rows */

static object mat_matvec_vec_int(object v,object nrows_object)
{
  index size=v->v.ncomp, nrows, ncols=nrows_object->i.intval;
  if (ncols<=0) error("Number of columns should be positive.\n");
  if (size%ncols!=0) 
    error ("Number of columns should divide size of vector.\n");
  { matrix* m=mkmatrix(nrows=size/ncols,ncols); index i,j,k=0;
    for (i=0; i<nrows; ++i)
      for (j=0; j<ncols; ++j)
	m->elm[i][j]=v->v.compon[k++]; /* |k==ncols*i+j| before increment */
    return (object) m;
  }
}

static entry gcd(x,y) entry x,y;
{
    /* Requirement 0<x <= y */
    entry r = x;
    if (y==0) return 0;
    while (r) {
	x = y % r;
	y = r;
	r = x;
	/* x <= y */
    }
    return y;
}

static entry abs_minimum(vector* v_vec)
/* return minimal absoulte value of nonzero array elements,
   or 0 when all elements are 0. */
{
    index i; boolean is_zero=true;
    index n = v_vec->ncomp;
    entry* v = v_vec->compon;
    index minimum=0;
    for (i=0; i<n; ++i) if (v[i]!=0)
      if (is_zero) { is_zero=false; minimum=labs(v[i]); }
      else { entry c = labs(v[i]); if (c<minimum) minimum = c; }
    return minimum;
}

static boolean equal_elements(v_vec) vector* v_vec;
/* Do all nonzero elements have the same absolute value? */
{
    index i, first = 0;
    index n = v_vec->ncomp;
    entry* v = v_vec->compon;

/* Omit prefixed 0 */
    while (first < n && v[first]==0) first++;
    if (first == n) return true; /* All zero */
    i = first + 1;
    while (i < n && (v[i]== 0 || labs(v[first]) == labs(v[i]))) i++;
    if (i == n) return true;
    return false;
}

object int_gcd_vec(v_vec)
vector *v_vec;
{
    entry r = abs_minimum(v_vec);
    entry *v;
    index i;
    index n = v_vec->ncomp;
    if (isshared(v_vec)) v_vec = copyvector(v_vec);
    v = v_vec->compon;
    while (!equal_elements(v_vec)) {
	for (i=0;i<n;i++) 
	    v[i] = gcd(r, v[i]);
	r = abs_minimum(v_vec);
    }
    return (object) mkintcel(r);
}

Symbrec		static5[] =
{
    C1("$bigint", (fobject)bin_from_int, BIGINT, INTEGER)
    C1("$intbig", (fobject)int_from_bin, INTEGER, BIGINT)
    C1("factor", vid_factor_bin, VOID, BIGINT)
    C2("mat_vec", mat_matvec_vec_int,MATRIX,VECTOR,INTEGER)
    C1("gcd",int_gcd_vec, INTEGER, VECTOR)
};
int nstatic5 = array_size(static5);
#ifdef __STDC__
# define P(s) s
#else
# define P(s) ()
#endif

bigint*	(*int2bin) P((intcel*)) = bin_from_int;
intcel* (*bin2int) P((bigint*)) = int_from_bin;
