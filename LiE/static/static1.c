#include "lie.h"

static void error_unequal(entry a,entry b)
{ error("Number of components unequal (%ld <-> %ld).\n",(long)a,(long)b);
}

static object vecmake(symblst arglst)
{
    index	      i, n;
    symblst	    list;
    object	    result;
    list = arglst;
    n = 0;
    while (list) {
	eval_value( list);
	list = list->next;
	n++;
    }
    list = arglst;
    result = (object) mkvector(n);
    for (i = 0; i < n; i++) {
	result->v.compon[i] = Integer(list->data.val);
	list = list->next;
    }
    return (result);
}


static object matmake(symblst arglst)
{
  object result;
  symblst list;
  index i, j, nr=0, nc=0, nc_last=0;
  for (list=arglst; list!=NULL; list=list->next, ++nr)
    {
      eval_value(list);
      nc = list->data.val->v.ncomp;
      if (nr>0 && nc!=nc_last)
	error("Matrix: Rows of unequal length.\n");
      nc_last = nc;
    }
  list=arglst;
  result = (object) mkmatrix(nr, nc);
  for (i = 0; i<nr; ++i, list=list->next)
    for (j = 0; j<nc; ++j)
      result->m.elm[i][j] = list->data.val->v.compon[j];
  return (result);
}

static object
vec_make(funsym) symblst
funsym;
{
    index	      i, size; 
    object	    result;
    symblst	    f, sizesym;
    object	    funargobj;
    strtype  fun_name_old = fun_name;
    sizesym = funsym->next;
    eval_value(sizesym);
    size = Integer(sizesym->data.val);
    if (size<0)
	error("MAKE cannot make a vector of negative size \n");
    result = (object) mkvector(size);
    addstaynode(result);
	f= newnode(funsym->a.label);
    fun_name = funsym->a.label->name;
    for (i = 0; i<size; i++) {
	funargobj = force_integer(funsym->arglst);
	funargobj->i.intval = i + 1;
	evalbl_value_dup(f,funsym);
	result->v.compon[i] = Integer(f->data.val);
    }
    fun_name = fun_name_old;
    return (result);
}


static object
mat_make(funsym) symblst
funsym;
{
    index	      i, j, rowsize, colsize;
    object	    result;
    symblst	    f, rowsizesym, colsizesym;
    object	    funarg1obj, funarg2obj;
    strtype  fun_name_old = fun_name;
    rowsizesym = funsym->next;
    colsizesym = rowsizesym->next;
    funsym->next = NULL;    /* Disconnect funsym from sizesym */
    eval_value(rowsizesym);
    rowsize = Integer(rowsizesym->data.val);
    eval_value(colsizesym);
    colsize = Integer(colsizesym->data.val);
    if (rowsize<0)
	error("Negative rowsize.\n");
    if (colsize<0)
	error("Negative colsize\n");
    result = (object) mkmatrix(rowsize, colsize);
    f= newnode(funsym->a.label);
    addstaynode(result);
    fun_name = funsym->a.label->name;
    for (i = 0; i<rowsize; i++) {
	for (j = 0; j<colsize; j++) {
	    funarg1obj = force_integer(funsym->arglst);
	    funarg2obj = force_integer(funsym->arglst->next);
	    funarg1obj->i.intval = i + 1;
	    funarg2obj->i.intval = j + 1;
	    evalbl_value_dup(f,funsym);
	    result->m.elm[i][j] = Integer(f->data.val);
	}
    }
    fun_name = fun_name_old;
    return (result);
}

static object mat_apply(symblst funsym)
{
  index	rowsize, colsize, i,j;
  object	    result;
  object	    funarg1obj, funarg2obj;
  symblst	    f, mat1sym, mat2sym;
  strtype  fun_name_old = fun_name;
  mat1sym = funsym->next;
  mat2sym = mat1sym->next;
  eval_value(mat1sym);
  if (mat1sym->type!=MATRIX) error("System error mat_apply.\n");
  rowsize = mat1sym->data.val->m.nrows;
  colsize = mat1sym->data.val->m.ncols;
  if (mat2sym) {
    eval_value(mat2sym);
    if (mat2sym->type!=MATRIX) error("System error mat_apply.\n");
    if (mat2sym->data.val->m.nrows != rowsize ||
	mat1sym->data.val->m.ncols != colsize)
      error("Dimensions left and right operands unequal.\n");
  }
  result = (object) mkmatrix(rowsize, colsize);
  addstaynode(result);
  f= newnode(funsym->a.label);
  fun_name = funsym->a.label->name;
  for (i = 0; i<rowsize; i++)
    for (j = 0; j<colsize; j++) {
      funarg1obj = force_integer(funsym->arglst);
      funarg1obj->i.intval=mat1sym->data.val->m.elm[i][j];
      if (mat2sym!=NULL)
	{
	  funarg2obj = force_integer(funsym->arglst->next);
	  funarg2obj->i.intval=mat2sym->data.val->m.elm[i][j];
	}
      evalbl_value_dup(f,funsym);
      result->m.elm[i][j]= Integer(f->data.val);
    }
  fun_name = fun_name_old;
  return (result);
}

static object vec_apply(symblst funsym)
{
  index	      i, n, size;
  object	    result, funarg1obj, funarg2obj;
  symblst	    f, vec1sym, vec2sym;
  strtype  fun_name_old = fun_name;
  vec1sym = funsym->next;
  vec2sym = vec1sym->next;
  eval_value(vec1sym);
  if (vec1sym->type!=VECTOR)
    return (NULL);
  size = vec1sym->data.val->v.ncomp;
  if (vec2sym) {
    eval_value(vec2sym);
    if (vec2sym->type!=VECTOR)
      error("System error vec_apply.\n");
    if (vec2sym->data.val->v.ncomp != size)
      error("index vector arguments unequal.\n");
  }
  result = (object) mkvector(size);
  addstaynode(result);
  f= newnode(funsym->a.label);
  n = size;
  fun_name = funsym->a.label->name;
  for (i = 0; i<n; i++) {
    funarg1obj = force_integer(funsym->arglst);
    funarg1obj->i.intval = vec1sym->data.val->v.compon[i];
    if (vec2sym!=NULL)
      {
	funarg2obj = force_integer(funsym->arglst->next);
	funarg2obj->i.intval = vec2sym->data.val->v.compon[i];
      }
    evalbl_value_dup(f,funsym);
    *(result->v.compon + i) = Integer(f->data.val);
  }
  fun_name = fun_name_old;
  return (result);
}

static object
ifmap_int(arglst) symblst
arglst;
{
    symblst left = arglst->next; 
    symblst right = left->next;
    boolean crit;
    eval_value(arglst);
    crit = Integer(arglst->data.val)!=0;
    clrshared(arglst->data.val);
    freemem(arglst->data.val);
    if (crit) {
	eval_value(left);
	return (left->data.val);
    } else {
	if (right) {
	    eval_value(right);
	    return (right->data.val);
	}
	return (object) NULL;
    }
}

static object
ifmap_pol(arglst) symblst
arglst;
{
    symblst left = arglst->next; 
    symblst right = left->next;
    poly *crit;
    eval_value(arglst);
    crit = (poly*) arglst->data.val; 
    if (!issorted(crit)) crit = Reduce_pol(crit);
    if (crit->nrows > 1 || crit->coef[0]->size) {
	eval_value(left);
	return (left->data.val);
    } else {
	if (right) {
	    eval_value(right);
	    return (right->data.val);
	}
	return (object) NULL;
    }
}

static object
ifmap_vec(arglst) symblst
arglst;
{
    symblst left = arglst->next, right = left->next;
    index i=0;
    vector *selector;
    index ncomp;
    eval_value(arglst);
    selector = (vector*) arglst->data.val;
    ncomp = selector->ncomp;
    while (i<ncomp && !selector->compon[i]) i++;
    if (i != ncomp) {
    /* vector unequal to zero vector */
	eval_value(left);
	return (left->data.val);
    } else {
	if (right) {
	    eval_value(right);
	    return (right->data.val);
	}
	return (object) NULL;
    }
}

static object
ifmap_mat(arglst) symblst
arglst;
{
    symblst left = arglst->next, right = left->next;
    index i=0,j=0;
    matrix *selector;
    entry nrows, ncols;
    eval_value(arglst);
    selector = (matrix*) arglst->data.val;
    nrows = selector->nrows;
    ncols = selector->ncols;

    j = ncols;
    while (i<nrows && j == ncols) {
	j = 0;
	while (j<ncols && !selector->elm[i][j]) j++;
	i++;
    }
    if (i != nrows || j != ncols) {
    /* matrix unequal to zero matrix */
	eval_value(left);
	return (left->data.val);
    } else {
	if (right) {
	    eval_value(right);
	    return (right->data.val);
	}
	return (object) NULL;
    }
}

static object vec_min_vec(vector* a)
{ index i; vector* result= mkvector(a->ncomp);
  for (i = 0; i<a->ncomp; ++i) result->compon[i] = -a->compon[i];
  return (object) result;
}

static object vec_not_vec(vector* a)
{ index i, n=a->ncomp; vector* result= mkvector(n);
  for (i = 0; i<n; ++i) result->compon[i] = a->compon[n-1-i];
  return (object) result;
}

static object
mat_min_mat(a)
    object	    a;
{
    object	    result;
    index	      i, j;
    result = (object) mkmatrix(a->m.nrows, a->m.ncols);
    for (i = 0; i<a->m.nrows; i++) {
	for (j = 0; j<a->m.ncols; j++)
	    *(*(result->m.elm + i) + j) = -*(*(a->m.elm + i) + j);
    }
    return (result);
}

static entry imod(entry x,entry n)
{
    return x>=0 ? x%n : -x%n==0 ? 0 : n-(-x%n) ;
}

static object
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
}

static object
mat_div_mat_int(a, b)
    object	    a, b;
{
    index  i,j;
    entry g = b->i.intval;
    object	    result;
    index n = a->m.ncols, m =a->m.nrows;
    if (!g)
	error("Division by zero\n");
    result = (object) mkmatrix(a->m.nrows, a->m.ncols);
    for (i = 0; i<m; i++)
    for (j = 0; j<n; j++)
	*(*(result->m.elm + i) + j) = *(*(a->m.elm + i) + j)/g;
    return (result);
}

static object
mat_mod_mat_int(a, b)
    object	    a, b;
{
    index  i, j;
    entry g = b->i.intval;
    index m = a->m.nrows, n = a->m.ncols;
    object	    result;
    if (g<0)
	error("LiE can only take the modulus by a positive number.\n");
    result = (object) mkmatrix(a->m.nrows, a->m.ncols);
    for (i = 0; i<m; i++)
    for (j = 0; j<n; j++)
	*(*(result->m.elm + i) + j) =
	imod(*(*(a->m.elm + i)+j), g);
    return (result);
}

static object bin_pow_bin_bin(object a, object b)
{
    if (b->b.size<0)
      error("Negative exponent; I cannot compute that power.\n");
    return power(a,(bigint*)b,(object)one,(f2object)mult);
}

static object pol_pow_pol_bin(object a, object b)
{
    if (b->b.size<0)
      error("Negative exponent; I cannot compute that power.\n");
    return power(a,(bigint*)b,(object)poly_one(a->pl.ncols)
		,(f2object)Mul_pol_pol);
}

static object
vec_mul_int_vec(a, b)
    object	    a, b;
{
    object	    result;
    index	      i;
    result = (object) mkvector(b->v.ncomp);
    for (i = 0; i<b->v.ncomp; i++)
	result->v.compon[i] = a->i.intval * b->v.compon[i];
    return (result);
}

static object
vec_div_vec_int(a, b)
    object	    a, b;
{
    object	    result;
    index	      i;
    entry g = b->i.intval;
    result = (object) mkvector(a->v.ncomp);
    if (!g) error("Division by 0\n");
    for (i = 0; i<a->v.ncomp; i++) 
	result->v.compon[i] = a->v.compon[i]/g;
    return (result);
}

static object
vec_mul_mat_vec(a, b)
    object	    a, b;
{
    index    i, k, n, m;
    object	    result;
    if (a->m.ncols != b->v.ncomp)
	error("Number columns arg1 unequal number of components arg2 .\n");
    n = a->m.nrows;
    m = a->m.ncols;
    result = (object) mkvector(n);
    for (i = 0; i<n; i++) {
	result->v.compon[i] = 0;
	for (k = 0; k<m; k++)
	    result->v.compon[i] += b->v.compon[k] * *(*(a->m.elm + i) + k);
    }
    return result;
}

static object
vec_mul_vec_mat(v, m)
    object	    v, m;
{
    index    i, k, nrows=m->m.nrows, ncols=m->m.ncols;
    object	    result;
    if (v->v.ncomp != nrows)
	error("Number rows arg2 unequal number of components arg1 .\n");
    result = (object) mkvector(ncols);
    for (i = 0; i<ncols; i++) {
	result->v.compon[i] = 0;
	for (k = 0; k<nrows; k++)
	    result->v.compon[i] += v->v.compon[k] * *(*(m->m.elm + k) + i);
    }
    return result;
}

static object
vec_append_vec_vec(v,w) 
    object v,w;
{
    object result;
    index i,nv=v->v.ncomp, nw=w->v.ncomp;
    result = (object) mkvector(nv+nw);
    for (i=0;i<nv;i++)
	result->v.compon[i]=v->v.compon[i];
    for (i=0;i<nw;i++)
	result->v.compon[nv+i]=w->v.compon[i];
    return result;
}

static object
mat_mul_mat_mat(a, b)
    object	    a, b;
{
    if (a->m.ncols != b->m.nrows)
	error("Number columns arg1 unequal number of rows arg2 .\n");
    return (object) Matmult((matrix *) a, (matrix *) b);
}

static object
pol_mul_pol_mat(a, b)
    poly	  *a; matrix *b;
{
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
    return (object) m;
}

matrix* mat_null (index r,index c)
{ index i,j; matrix* m=mkmatrix(r,c); entry** me=m->elm;
  for (i=0;i<r;++i) for (j=0;j<c;++j) me[i][j]=0;
  return m;
}

matrix* mat_id(index size)
{ index i,j; matrix* m=mkmatrix(size,size); entry** me=m->elm;
  for (i=0;i<size;++i) for (j=0;j<size;++j) me[i][j] = i==j;
  return m;
}

static object mat_pow_mat_bin(object a, object b)
{
    if (a->m.ncols != a->m.nrows)
	error("Number of columns unequal number of rows.\n");
    if (b->b.size<0)
	error("Matrix raised to negative power.\n");
    return power(a,(bigint*)b,(object)mat_id(a->m.nrows),(fobject)Matmult);
}

static object 
mat_append_mat_mat(a,b)
    object a,b;
{
    object result;
    index i,n1=a->m.nrows,n2=b->m.nrows,m;
    if (a->m.ncols != b->m.ncols) 
    error("Unequal number of columns. (%ld <-> %ld) \n",
	(long)a->m.ncols, (long)b->m.ncols);
    m=a->m.ncols;
    result = (object) mkmatrix(n1+n2,m);
    for (i=0;i<n1;++i) copyrow(a->m.elm[i],result->m.elm[i],m);
    for (i=0;i<n2;++i) copyrow(b->m.elm[i],result->m.elm[n1+i],m);
    return result;
}

static object
mat_transpose_mat(a)
    object	    a;
{
    return (object) Transpose((matrix *) a);
}

static object
int_mul_vec_vec(a, b)
    object	    a, b;
{
    object	    result;
    index	      sum, i;
    if (a->v.ncomp != b->v.ncomp)
	error_unequal(a->v.ncomp, b->v.ncomp);
    sum = 0;
    for (i = 0; i<a->v.ncomp; i++)
	sum += a->v.compon[i] * b->v.compon[i];
    result = (object) mkintcel(sum);
    return (result);
}

object vec_add_vec_vec(object v, object w)
{
  index i; vector* a=&v->v,* b=&w->v; vector* result;
  if (a->ncomp != b->ncomp) error_unequal(a->ncomp,b->ncomp);
  result = isshared(a) ? copyvector(a) : a;
  for (i = 0; i<a->ncomp; ++i) result->compon[i] += b->compon[i];
  return (object) result;
}


object
mat_add_mat_vec(a, v)
    object	    a, v;
{
    object	    result;
    index  m = a->m.nrows, n = a->m.ncols;
    index	      i,j; 
    if (a->m.ncols != v->v.ncomp)
	error("Number of vector components unequal number of columns .\n");
    if (isshared(a) || a->m.rowsize == m) {
	result = (object) mkmatrix(2 * m + 1, n);
	for (i = 0; i<m; i++)
	for (j = 0; j<n; j++)
	    *(*(result->m.elm + i)+j) = *(*(a->m.elm + i)+j);
	} 
    else result = a;
    for (j = 0; j<n; j++)
	*(*(result->m.elm + m)+j) = v->v.compon[j];
    result->m.nrows = m + 1;
    return (result);
}

static object
mat_sub_mat_int(a, obj_k)
    object	    a, obj_k;
{
    object	    result;
    index  m = a->m.nrows, n = a->m.ncols;
    index  k = obj_k->i.intval-1;
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

static object
vec_dif_vec_vec(a, b)
    object	    a, b;
{
    object	    result;
    index	      i;
    if (a->v.ncomp != b->v.ncomp)
	error_unequal(a->v.ncomp, b->v.ncomp);
    result = (object) mkvector(a->v.ncomp);
    for (i = 0; i<a->v.ncomp; i++)
	result->v.compon[i] = a->v.compon[i] - b->v.compon[i];
    return (result);
}

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

static object
mat_sub_mat_mat(a, b)
    object	    a, b;
{
    object	    result;
    index    i,j;
    index	      n=a->m.ncols, m=a->m.nrows;
    if (a->m.nrows != b->m.nrows)
	error("Number of rows of matrix arguments unequal.\n");
    if (a->m.ncols != b->m.ncols)
	error("Number of columns of matrix arguments unequal.\n");
    result = (object) mkmatrix(a->m.nrows, a->m.ncols);
    for (i = 0; i<m; i++)
    for (j = 0; j<n; j++)
	*(*(result->m.elm + i) + j) 
	= *(*(a->m.elm + i) + j) - *(*(b->m.elm + i) + j);
    return (result);
}

static object
mat_mul_int_mat(a, b)
    object	    a, b;
{
    object	    result;
    index    i,j;
    entry g = a->i.intval;
    index n = b->m.ncols, m = b->m.nrows;
    result = (object) mkmatrix(b->m.nrows, b->m.ncols);
    for (i = 0; i<m; i++)
    for (j = 0; j<n; j++)
	*(*(result->m.elm + i) + j) = *(*(b->m.elm + i) + j) * g;
    return (result);
}

static object
int_siz_vec(a)
    object	    a;
{
    object	    result;
    result = (object) mkintcel(a->v.ncomp);
    return (result);
}

static object
int_rowsiz_mat(a)
    object	    a;
{
    object	    result;
    result = (object) mkintcel(a->m.nrows);
    return (result);
}

static object
int_colsiz_mat(a)
    object	    a;
{
    object	    result;
    result = (object) mkintcel(a->m.ncols);
    return (result);
}

static object
int_n_vars_pol(p)
    object	    p;
{
    return (object) mkintcel(p->pl.ncols);
}

static object mat_null_int_int(object m_obj,object n_obj)
{ index r = Integer(m_obj), c = Integer(n_obj);
  if (r<0) error("row size<0\n");
  if (c<0) error("column size<0\n");
  return (object) mat_null(r,c);
}

static object mat_one_int_int(m_obj,n_obj)
    object m_obj,n_obj;
{ index i,j,r=Integer(m_obj), c=Integer(n_obj); matrix* m;
  if (r<0) error("row size<0\n");
  if (c<0) error("column size<0\n");
  m=mkmatrix(r,c);
  for (i=0;i<r;++i) for (j=0;j<c;++j) m->elm[i][j]=1;
  return (object) m;
}

static object pol_null_int(r)
intcel *r;
{
    index d = r->intval;
    if (d<0) error("n_vars of polynomial negative (=%ld)",(long)d);
    return (object) poly_null(d);
}

static object pol_one_int(r)
intcel *r;
{
    index d = r->intval;
    if (d<0) error("n_vars of polynomial negative (=%ld)",(long)d);
    return (object) poly_one(d);
}

static object mat_unit_int(intcel* nn)
{ index n=nn->intval;
  if (n<0) 
    error("Cannot create identity matrix of negative size %ld.\n",(long)n);
  return (object) mat_id(n);
}

static object vec_unit_int_int(intcel* nn, intcel* ii)
{ index n=nn->intval, i=ii->intval,j; vector* result; entry* v;
  if (i<0 || i>n)
    error("Cannot create unit vector %ld of size %ld.\n",(long)i,(long)n);
  result=mkvector(n); v=result->compon;
  for (j=0; j<n; ++j) v[j]= 0;
  v[i-1]=1;
  return (object) result;
}

static object vec_null_int(intcel* nn)
{ index n=nn->intval,j; vector* result; entry* v;
  if (n<0)
    error("Cannot create vector of negative size %ld.\n",(long)n);
  result=mkvector(n); v=result->compon;
  for (j=0; j<n; ++j) v[j]= 0;
  return (object) result;
}


static object vec_one_int(intcel* nn)
{ index n=nn->intval,j; vector* result; entry* v;
  if (n<0)
    error("Cannot create vector of negative size %ld.\n",(long)n);
  result=mkvector(n); v=result->compon;
  for (j=0; j<n; ++j) v[j]= 1;
  return (object) result;
}


	/*
	 * Write here the interfaces above defined operations. 
	 */

    Symbrec static1[] = {
    C1("-", vec_min_vec, VECTOR, VECTOR)
    C1("!", vec_not_vec, VECTOR, VECTOR)
    C1("-", mat_min_mat, MATRIX, MATRIX)
    C2("^", bin_pow_bin_bin, BIGINT, BIGINT , BIGINT)
    C2("^", pol_pow_pol_bin, POLY, POLY , BIGINT)
    C2("*", vec_mul_int_vec, VECTOR, INTEGER, VECTOR)
    C2("*", int_mul_vec_vec, INTEGER, VECTOR, VECTOR)
    C2("*", vec_mul_mat_vec, VECTOR, MATRIX, VECTOR)
    C2("*", vec_mul_vec_mat, VECTOR, VECTOR, MATRIX)
    C1("size", int_siz_vec, INTEGER, VECTOR)
    C2("+", vec_add_vec_vec, VECTOR, VECTOR, VECTOR)
    C2("-", vec_dif_vec_vec, VECTOR, VECTOR, VECTOR)
    C2("+", mat_add_mat_vec, MATRIX, MATRIX, VECTOR)
    C2("-", mat_sub_mat_int, MATRIX, MATRIX, INTEGER)
    C2("+", mat_add_mat_mat, MATRIX, MATRIX, MATRIX)
    C2("-", mat_sub_mat_mat, MATRIX, MATRIX, MATRIX)
    C2("*", mat_mul_int_mat, MATRIX, INTEGER, MATRIX)
    C2("^", mat_pow_mat_bin, MATRIX, MATRIX, BIGINT)
    C2("^", mat_append_mat_mat, MATRIX, MATRIX, MATRIX)
    C2("^", vec_append_vec_vec, VECTOR, VECTOR, VECTOR)
    C2("/", mat_div_mat_int, MATRIX, MATRIX, INTEGER)
    C2("/", vec_div_vec_int, VECTOR, VECTOR, INTEGER)
    C2("%", mat_mod_mat_int, MATRIX, MATRIX, INTEGER)
    C2("%", vec_mod_vec_int, VECTOR, VECTOR, INTEGER)
    C1("n_rows", int_rowsiz_mat, INTEGER, MATRIX)
    C1("n_cols", int_colsiz_mat, INTEGER, MATRIX)
    C1("n_vars", int_n_vars_pol, INTEGER, POLY)
    C2("*", mat_mul_mat_mat, MATRIX, MATRIX, MATRIX)
    C2("*", pol_mul_pol_mat, POLY, POLY, MATRIX)
    C1("*", mat_transpose_mat, MATRIX, MATRIX)
    C2("null", mat_null_int_int, MATRIX, INTEGER, INTEGER)
    C2("all_one", mat_one_int_int, MATRIX, INTEGER, INTEGER)
    C1("null", vec_null_int, VECTOR, INTEGER)
    C1("all_one", vec_one_int, VECTOR, INTEGER)
    C1("poly_null", pol_null_int, POLY, INTEGER)
    C1("poly_one", pol_one_int, POLY, INTEGER)
    C1("id", mat_unit_int, MATRIX, INTEGER)
    C2("id", vec_unit_int_int, VECTOR, INTEGER, INTEGER)
    M2(".make", vec_make, VECTOR, INTFUN, INTEGER)
    M3(".make", mat_make, MATRIX, INTFUN, INTEGER, INTEGER)
    M1(".strucval", vecmake, VECTOR, INTLST)
    M1(".strucval", matmake, MATRIX, VECLST)
    M2(".make", vec_apply, VECTOR, INTFUN, VECTOR)
    M3(".make", vec_apply, VECTOR, INTFUN, VECTOR, VECTOR)
    M2(".make", mat_apply, MATRIX, INTFUN, MATRIX)
    M3(".make", mat_apply, MATRIX, INTFUN, MATRIX, MATRIX)
    M3(".if", ifmap_int, ARGTYPE, INTEGER, ARGTYPE, ARGTYPE)
    M2(".if", ifmap_int, ARGTYPE, INTEGER, ARGTYPE) 
    M3(".if", ifmap_vec, ARGTYPE, VECTOR, ARGTYPE, ARGTYPE)
    M2(".if", ifmap_vec, ARGTYPE, VECTOR, ARGTYPE) 
    M3(".if", ifmap_mat, ARGTYPE, MATRIX, ARGTYPE, ARGTYPE)
    M2(".if", ifmap_mat, ARGTYPE, MATRIX, ARGTYPE) 
    M3(".if", ifmap_pol, ARGTYPE, POLY, ARGTYPE, ARGTYPE)
    M2(".if", ifmap_pol, ARGTYPE, POLY, ARGTYPE) 
};

int nstatic1 = array_size(static1);
