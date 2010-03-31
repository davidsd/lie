#include "lie.h"

void error_unequal(entry a,entry b)
{ error("Number of components unequal (%ld <-> %ld).\n",(long)a,(long)b);
}

vector* vec_min_vec(vector* a) {
  index i; vector* result= mkvector(a->ncomp);
  for (i = 0; i<a->ncomp; ++i) result->compon[i] = -a->compon[i];
  return result;
}
/*
object vec_not_vec(vector* a)
{ index i, n=a->ncomp; vector* result= mkvector(n);
  for (i = 0; i<n; ++i) result->compon[i] = a->compon[n-1-i];
  return (object) result;
}
*/
matrix* mat_min_mat(matrix* a) {
    matrix* result;
    index	      i, j;
    result = mkmatrix(a->nrows, a->ncols);
    for (i = 0; i<a->nrows; i++) {
	for (j = 0; j<a->ncols; j++)
	    *(*(result->elm + i) + j) = -*(*(a->elm + i) + j);
    }
    return result;
}

entry imod(entry x,entry n)
{
    return x>=0 ? x%n : -x%n==0 ? 0 : n-(-x%n) ;
}

/*
object
vec_mod_vec_int(a, b)
    object	    a, b;
{
    object	    result;
    index i;
    entry g = b->i.intval;
    if (g<0)
	error("LiE can only take the modulus by a positive number.\n");
    result = (object) mkvector(a->v.ncomp);
    for (i = 0; i<a->v.ncomp; i++)
	result->v.compon[i] = imod(a->v.compon[i],g);
    return result;
}*/

vector* vec_mod_vec_int(vector *a, entry b) {
    vector* result;
    index i;
    if (b<0)
	error("LiE can only take the modulus by a positive number.\n");
    result = mkvector(a->ncomp);
    for (i = 0; i<a->ncomp; i++)
	result->compon[i] = imod(a->compon[i],b);
    return result;
}

matrix *mat_div_mat_int(matrix *a, entry b) {
    index  i,j;
    matrix* result;
    index n = a->ncols, m =a->nrows;
    if (!b)
	error("Division by zero\n");
    result = mkmatrix(a->nrows, a->ncols);
    for (i = 0; i<m; i++)
    for (j = 0; j<n; j++)
	*(*(result->elm + i) + j) = *(*(a->elm + i) + j)/b;
    return result;
}

matrix *mat_mod_mat_int(matrix *a, entry b) {
    index  i, j;
    index m = a->nrows, n = a->ncols;
    matrix* result;
    if (b<0)
	error("LiE can only take the modulus by a positive number.\n");
    result = mkmatrix(a->nrows, a->ncols);
    for (i = 0; i<m; i++)
    for (j = 0; j<n; j++)
	*(*(result->elm + i) + j) =
	imod(*(*(a->elm + i)+j), b);
    return result;
}
/*
object bin_pow_bin_bin(object a, object b)
{
    if (b->b.size<0)
      error("Negative exponent; I cannot compute that power.\n");
    return power(a,(bigint*)b,(object)one,(f2object)mult);
}

object pol_pow_pol_bin(object a, object b)
{
    if (b->b.size<0)
      error("Negative exponent; I cannot compute that power.\n");
    return power(a,(bigint*)b,(object)poly_one(a->pl.ncols)
		,(f2object)Mul_pol_pol);
}
*/
vector* vec_mul_int_vec(long a, vector* b) {
    vector *result;
    index i;
    result = mkvector(b->ncomp);
    for (i = 0; i<b->ncomp; i++)
	result->compon[i] = a * b->compon[i];
    return (result);
}

vector* vec_div_vec_int(vector *a, long b) {
    vector*   result;
    index	      i;
    result = mkvector(a->ncomp);
    if (!b) error("Division by 0\n");
    for (i = 0; i<a->ncomp; i++) 
	result->compon[i] = a->compon[i]/b;
    return (result);
}

vector* vec_mul_mat_vec(matrix *a, vector *b) {
    index    i, k, n, m;
    vector *result;
    if (a->ncols != b->ncomp)
	error("Number columns arg1 unequal number of components arg2 .\n");
    n = a->nrows;
    m = a->ncols;
    result = mkvector(n);
    for (i = 0; i<n; i++) {
	result->compon[i] = 0;
	for (k = 0; k<m; k++)
	    result->compon[i] += b->compon[k] * *(*(a->elm + i) + k);
    }
    return result;
}

vector *vec_mul_vec_mat(vector *v, matrix *m) {
    index    i, k, nrows=m->nrows, ncols=m->ncols;
    vector *result;
    if (v->ncomp != nrows)
	error("Number rows arg2 unequal number of components arg1 .\n");
    result = mkvector(ncols);
    for (i = 0; i<ncols; i++) {
	result->compon[i] = 0;
	for (k = 0; k<nrows; k++)
	    result->compon[i] += v->compon[k] * *(*(m->elm + k) + i);
    }
    return result;
}

vector *vec_append_vec_vec(vector *v, vector *w)  {
    vector *result;
    index i,nv=v->ncomp, nw=w->ncomp;
    result = mkvector(nv+nw);
    for (i=0;i<nv;i++)
	result->compon[i]=v->compon[i];
    for (i=0;i<nw;i++)
	result->compon[nv+i]=w->compon[i];
    return result;
}

matrix *mat_mul_mat_mat(matrix *a, matrix *b) {
    if (a->ncols != b->nrows)
	error("Number columns arg1 unequal number of rows arg2 .\n");
    return NULL; /*Matmult(a, b);*/
}

poly *pol_mul_pol_mat(poly *a, matrix *b) {
    poly *m;
    int i;
    if (a->ncols != b->nrows)
	error("Number variables arg1 unequal number of rows arg2 .\n");
    m = mkpoly(a->nrows,b->ncols);
    mulmatmatelm(a->elm,b->elm,m->elm,a->nrows,a->ncols,b->ncols);
    for (i=0;i<m->nrows;i++) { 
	m->coef[i] = a->coef[i];
	setshared(m->coef[i]);
    }
    return m;
}

/*
object mat_pow_mat_bin(object a, object b)
{
    if (a->m.ncols != a->m.nrows)
	error("Number of columns unequal number of rows.\n");
    if (b->b.size<0)
	error("Matrix raised to negative power.\n");
    return power(a,(bigint*)b,(object)mat_id(a->m.nrows),(fobject)Matmult);
}
*/

matrix *mat_append_mat_mat(matrix *a, matrix *b) {
    matrix *result;
    index i,n1=a->nrows,n2=b->nrows,m;
    if (a->ncols != b->ncols) 
    error("Unequal number of columns. (%ld <-> %ld) \n",
	(long)a->ncols, (long)b->ncols);
    m=a->ncols;
    result = mkmatrix(n1+n2,m);
    for (i=0;i<n1;++i) copyrow(a->elm[i],result->elm[i],m);
    for (i=0;i<n2;++i) copyrow(b->elm[i],result->elm[n1+i],m);
    return result;
}

matrix* mat_transpose_mat(matrix *a) {
    return Transpose((matrix *) a);
}


index int_mul_vec_vec(vector *a, vector *b) {
    index result;
    index sum, i;
    if (a->ncomp != b->ncomp)
	error_unequal(a->ncomp, b->ncomp);
    sum = 0;
    for (i = 0; i<a->ncomp; i++)
	sum += a->compon[i] * b->compon[i];
    result = sum;
    return result;
}

object
mat_sub_mat_int(a, obj_k)
     object	    a; entry obj_k;
{
    object	    result;
    index  m = a->m.nrows, n = a->m.ncols;
    index  k = obj_k-1;
    index	      i,j; 
    if (k >= m || k<0)
	error("Index %ld out of range.\n",(long)(k+1));
    if (isshared(a) || a->m.rowsize == m) {
	result = (object) mkmatrix(m - 1, n);
	for (i = 0; i<k; i++)
	for (j = 0; j<n; j++)
	    *(*(result->m.elm + i)+j) = *(*(a->m.elm + i)+j);
	for (i = k+1; i< m;i++)
	for (j = 0; j<n; j++)
	    *(*(result->m.elm + i-1)+j) = *(*(a->m.elm + i)+j);
    } else {
	result = a;
	for (i = k+1; i< m;i++)
	    *(result->m.elm+i-1) = *(result->m.elm+i);
	result->m.nrows = m - 1;
    }
    return  result;
}

vector *vec_dif_vec_vec(vector *a, vector *b) {
    vector *result;
    index	      i;
    if (a->ncomp != b->ncomp)
	error_unequal(a->ncomp, b->ncomp);
    result = mkvector(a->ncomp);
    for (i = 0; i<a->ncomp; i++)
	result->compon[i] = a->compon[i] - b->compon[i];
    return result;
}
/*
object
mat_add_mat_mat(a, b)
    object	    a, b;
{
    object	    result;
    index    i,j;
    index	      n = a->m.ncols,m = a->m.nrows;
    if (a->m.nrows != b->m.nrows)
	error("Number of rows of matrix arguments unequal.\n");
    if (a->m.ncols != b->m.ncols)
	error("Number of columns of matrix arguments unequal.\n");
    if (isshared(a)) 
	result = (object) copymatrix((matrix*) a);
    else
	result = a;
    for (i = 0; i<m; i++)
    for (j = 0; j<n; j++)
	*(*(result->m.elm + i) + j) += *(*(b->m.elm + i) + j);
    return result;
}
*/

matrix* mat_sub_mat_mat(matrix* a, matrix* b) {
    matrix* result;
    index    i,j;
    index	      n=a->ncols, m=a->nrows;
    if (a->nrows != b->nrows)
	error("Number of rows of matrix arguments unequal.\n");
    if (a->ncols != b->ncols)
	error("Number of columns of matrix arguments unequal.\n");
    result = mkmatrix(a->nrows, a->ncols);
    for (i = 0; i<m; i++)
    for (j = 0; j<n; j++)
	*(*(result->elm + i) + j) 
	= *(*(a->elm + i) + j) - *(*(b->elm + i) + j);
    return (result);
}

matrix *mat_mul_int_mat(entry a, matrix *b) {
    matrix* result;
    index    i,j;
    index n = b->ncols, m = b->nrows;
    result = mkmatrix(b->nrows, b->ncols);
    for (i = 0; i<m; i++)
    for (j = 0; j<n; j++)
	*(*(result->elm + i) + j) = *(*(b->elm + i) + j) * a;
    return result;
}

object
int_siz_vec(a)
    object	    a;
{
    object	    result;
    result = (object) mkintcel(a->v.ncomp);
    return (result);
}

object
int_rowsiz_mat(a)
    object	    a;
{
    object	    result;
    result = (object) mkintcel(a->m.nrows);
    return (result);
}

object
int_colsiz_mat(a)
    object	    a;
{
    object	    result;
    result = (object) mkintcel(a->m.ncols);
    return (result);
}

object
int_n_vars_pol(p)
    object	    p;
{
    return (object) mkintcel(p->pl.ncols);
}

/*
object mat_null_int_int(object m_obj,object n_obj)
{ index r = Integer(m_obj), c = Integer(n_obj);
  if (r<0) error("row size<0\n");
  if (c<0) error("column size<0\n");
  return (object) mat_null(r,c);
}

object mat_one_int_int(m_obj,n_obj)
    object m_obj,n_obj;
{ index i,j,r=Integer(m_obj), c=Integer(n_obj); matrix* m;
  if (r<0) error("row size<0\n");
  if (c<0) error("column size<0\n");
  m=mkmatrix(r,c);
  for (i=0;i<r;++i) for (j=0;j<c;++j) m->elm[i][j]=1;
  return (object) m;
}

object pol_null_int(r)
intcel *r;
{
    index d = r->intval;
    if (d<0) error("n_vars of polynomial negative (=%ld)",(long)d);
    return (object) poly_null(d);
}

object pol_one_int(r)
intcel *r;
{
    index d = r->intval;
    if (d<0) error("n_vars of polynomial negative (=%ld)",(long)d);
    return (object) poly_one(d);
}

object mat_unit_int(intcel* nn)
{ index n=nn->intval;
  if (n<0) 
    error("Cannot create identity matrix of negative size %ld.\n",(long)n);
  return (object) mat_id(n);
}

object vec_unit_int_int(intcel* nn, intcel* ii)
{ index n=nn->intval, i=ii->intval,j; vector* result; entry* v;
  if (i<0 || i>n)
    error("Cannot create unit vector %ld of size %ld.\n",(long)i,(long)n);
  result=mkvector(n); v=result->compon;
  for (j=0; j<n; ++j) v[j]= 0;
  v[i-1]=1;
  return (object) result;
}

object vec_null_int(intcel* nn)
{ index n=nn->intval,j; vector* result; entry* v;
  if (n<0)
    error("Cannot create vector of negative size %ld.\n",(long)n);
  result=mkvector(n); v=result->compon;
  for (j=0; j<n; ++j) v[j]= 0;
  return (object) result;
}


object vec_one_int(intcel* nn)
{ index n=nn->intval,j; vector* result; entry* v;
  if (n<0)
    error("Cannot create vector of negative size %ld.\n",(long)n);
  result=mkvector(n); v=result->compon;
  for (j=0; j<n; ++j) v[j]= 1;
  return (object) result;
}
*/
