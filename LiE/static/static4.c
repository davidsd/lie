/* static4 is engaged with type VOID */
#include "lie.h"

#ifdef development
extern int MEMCONTROL;
#endif
#ifdef __STDC__
typedef object symb_fun(symblst);
static symb_fun
  vid_for, vid_downfor, vid_forvec, vid_format
, arg_while_int_arg, vid_eval, arg_sequence, vid_assign, vid_assign_loc
, vid_update_vec_int_int, vid_addassign_vec_int_int
, vid_update_mat_int_int_int, vid_addassign_mat_int_int_int
, vid_update_mat_vec_int, vid_vecupdate_pol_bin_vec
, vid_update_pol_pol_int, vid_addupdate_vec_int
, vid_addupdate_mat_vec, vid_addupdate_mat_mat
, vid_addupdate_vec_vec, vid_addupdate_pol_pol
, vid_addupdate_tex_tex, vid_addupdate_tex_bin
;
static object
  vid_on_int_tex(intcel* n, tekst* t)
, vid_off_int_tex(intcel* n, tekst* t)
, vid_save(void), vid_restore(void), vid_on(void)
, vid_for_a(symblst endsym,boolean increment)
, vid_void_arg(object x), vid_void(void)
, arg_nref_arg(object s)
, mat_copy_mat_int_int_int_a (matrix*,object,object,object,boolean)
, mat_copy_mat_int_int_int (matrix*,object,object,object)
, mat_addassign_mat_int_int_int (matrix*,object,object,object)
, vec_copy_vec_int_int_a (vector*,object,object,boolean)
, vec_copy_vec_int_int(vector*,object,object)
, vec_addassign_vec_int_int(vector*,object,object)
, pol_copy_pol_bin_vec (poly*,object,vector*)
, mat_copy_mat_vec_int(matrix*,vector*,object)
, pol_copy_pol_pol_int(poly*,object,object)
, pol_addc_pol_pol(object,object)
, vid_error_tex(object)
, int_eq_tex_tex(tekst*,tekst*)
, tex_append_tex_tex(object,object)
, tex_add_tex_bin(tekst*,bigint*)
, tex_fmt_bin_int(bigint*,intcel*)
, tex_add_tex_grp(tekst*,group*)
, tex_add_grp_tex(group*,tekst*)
, tex_add_bin_tex(bigint*,tekst*)
, tex_mul_tex_int(tekst*,intcel*)
, tex_mul_int_tex(intcel*,tekst*)
, vid_addr(object)
, mat_getmat_tex(tekst*)
, tex_getstring_tex(tekst*)
, vid_savemat_mat_tex(object,tekst*)
, vid_savestring_tex_tex(object,tekst*);
#endif

boolean Boolean(object t)
{
return (is_int(type_of(t))?Integer(t)!=0:
  (eq_types(type_of(t),VECTOR)?!Vec_null(t):
      (eq_types(type_of(t),MATRIX)?!Mat_null(t):
	  (eq_types(type_of(t),POLY)?!Pol_null(t):false) 
      ) 
  ) 
); 
}

static object vid_on_int_tex(n,t) intcel *n; tekst *t;
{
    if (set_on(n->intval,match(t->string,false)))
	error("Option %s not found.\n",t->string);
    return (object) NULL;
}

static object vid_off_int_tex(n,t) intcel *n; tekst *t;
{
    if (set_off(n->intval,match(t->string,false)))
	error("Option %s not found.\n",t->string);
    return (object) NULL;
}

static object vid_save() { save_state(); return (object) NULL; }

static object vid_restore() { restore_state(); return (object) NULL; }

static object vid_on() { print_state(); return (object) NULL; }

static object vid_for_a(endsym,increment) symblst endsym; boolean increment;
{
    object	    i_dex;
    symblst	    result;
    symblst	    blocksym = endsym->next;
    symblst	    topstack = topsym->next;
    entry	    endloop; 
    object	    stop_later=stop_loop;
    stop_loop=NULL;
    eval_value(endsym);
    endloop = Integer(endsym->data.val);
    result = newnode(blocksym->a.label);
    i_dex = force_integer(topstack);
    /* Copy arexpr to array store */
    for (; increment?i_dex->i.intval <= endloop:
    i_dex->i.intval >= endloop; 
	increment?i_dex->i.intval++:i_dex->i.intval--) {
	evalbl_value_dup(result, blocksym);

/***************************************************************
*  Side effect on index					       *
***************************************************************/


	{
	    boolean result_shared = (result->data.val == i_dex);
	    if (result_shared) setshared(i_dex);
	    i_dex = force_integer(topstack);
	    if (result_shared) clrshared(result->data.val);
	}


/***************************************************************
*  break						       *
***************************************************************/


	if (stop_loop!=NULL || stop_fun!=NULL)
	{ result->data.val= stop_loop!=NULL ? stop_loop : nothing ; break; }
    }
    stop_loop=stop_later; /* restore |break| switch */
    return (object) result->data.val;
}

static object vid_for(endsym) symblst endsym;
{ return vid_for_a(endsym,true); }

static object vid_downfor(endsym) symblst endsym;
{ return vid_for_a(endsym,false); }


static object vid_forvec(vecsym) symblst vecsym;
{
    index	      i, vecsize;
    object	    i_dex, vecobj;
    symblst	    result;
    symblst  topstack = topsym->next;
    object        stop_later=stop_loop;
    stop_loop = NULL;
    eval_value( vecsym);
    vecobj = vecsym->data.val;
    result = newnode(vecsym->next->a.label);
    i_dex = force_integer(topstack);
    vecsize = vecobj->v.ncomp;
    for (i = 0; i < vecsize; i++) {
	i_dex->i.intval = vecobj->v.compon[i];
	evalbl_value_dup( result, vecsym->next);
	i_dex = force_integer(topstack);
	if (stop_loop!=NULL || stop_fun!=NULL)
	{ result->data.val= stop_loop!=NULL ? stop_loop : nothing ; break; }
    }
    stop_loop=stop_later;
    return (object) result->data.val;
}


static object vid_format(matsym) symblst matsym;
{
    index	      i, j, rowsize, colsize;
    object	    i_dex, matobj;
    symblst	    result;
    symblst topstack = topsym->next;
    object stop_later=stop_loop;
    eval_value( matsym);
    matobj = matsym->data.val;
    result = newnode(matsym->next->a.label);
    i_dex = topstack->data.val=(object) mkvector(matobj->m.ncols);
    /* i_dex has to be copied after assignment */
    setshared(i_dex);
    stop_loop=NULL;
    rowsize = matobj->m.nrows;
    colsize = matobj->m.ncols;
    for (i = 0; i < rowsize; i++) {
	for (j = 0; j< colsize; j++)
	    i_dex->v.compon[j] = matobj->m.elm[i][j];
	evalbl_value_dup(result, matsym->next);
	if (type_of(topstack) != VECTOR)
	    error("Vector expected - %s encountered\n",
	    code_type(type_of(topstack)));
	if (stop_loop!=NULL || stop_fun!=NULL)
	{ result->data.val= stop_loop!=NULL ? stop_loop : nothing ; break; }
    }
    stop_loop=stop_later;
    return (object) result->data.val;
}

static object arg_while_int_arg(condition) symblst condition;
{
    symblst blocksym = condition->next;
    object stop_later=stop_loop;
    symblst cresult, bresult;
    stop_loop=NULL;
    cresult = newnode(condition->a.label);
    bresult = newnode(blocksym->a.label);
    while (stop_loop==NULL && stop_fun==NULL &&
	(evalbl_value_dup(cresult, condition), Boolean(cresult->data.val)))
	evalbl_value_dup(bresult, blocksym);
    if (stop_loop!=NULL || stop_fun!=NULL)
      bresult->data.val= stop_loop!=NULL ? stop_loop : nothing;
    stop_loop=stop_later;
    return (object) bresult->data.val;
}

static object vid_eval(s) symblst s;
{
    eval_value( s);
    return (object) NULL;
}

static object vid_void_arg(object x) { return (object) NULL; }

static object vid_void(void) { return (object) NULL; }

static object arg_nref_arg(s) object s;
{
    Printf("nref: %d\n",s->i.nref);
    return s;
}

#ifdef development

static object bin_addc_bin_int(a,b)
object a,b;
{
    bigint *result = (bigint*) a;
    addc(&result,Integer(b));
    return (object) result;
}

static object bin_divc_bin_int(a,b)
object a,b;
{
    bigint *result = (bigint*) a;
    divc(&result,Integer(b));
    return (object) result;
}

static object bin_mulc_bin_int(a,b)
object a,b;
{
    bigint *result = (bigint*) a;
    mulc(&result,Integer(b));
    return (object) result;
}
#endif

#ifdef where_object
static object vid_showptr_tex(t)
tekst *t;
{	
    int result =  sscanf(t->string,"%x",&showptr);
    if (result != 1) 
	error("%d pointers read instead of the expected one.\n",result);
    Printf("Message - if freeing pointer %lx\n",showptr);
    return (object) NULL;
}

static object vid_memcontrol_int(k)
object k;
{	
    MEMCONTROL = Integer(k);
    return (object) NULL;
}
#endif

object vid_gc(VOID) { gc(); return (object) NULL; }

object vid_print(object x)
{ if (x!=NULL)
    switch (type_of(x))
    {      case INTEGER: printint(x);
    break; case VECTOR:  if (lprint) print_vector(&x->v); else printvec(x);
    break; case MATRIX:
	   if (lprint) print_matrix(&x->m); else print_mat_bars(&x->m);
	   Printf("\n");
    break; case GROUP: printgroup(x);
    break; case BIGINT:
	Printf("%*s",lmargin,""); printbigint(&x->b,0);	Printf("\n");
    break; case TEKST: printtekst(x);
    break; case POLY:
          { poly* p= &x->pl; if (!issorted(x)) p=Reduce_pol(p);
	    if (lprint) print_poly(p); else print_poly_vertical(p);
	  }
    }
  fflush(cur_out);
  return (object) NULL;
}

static object arg_sequence(symblst list)
{
  symblst result;
  if (list==NULL) return (object) NULL;
  do
    {
      eval_value(list);
      result = list;
      if (list->class != VALUE) error("Sequence: system error.\n");
      list = list->next;
    }
  while (list!=NULL && stop_loop==NULL && stop_fun==NULL);
  if (stop_loop!=NULL || stop_fun!=NULL)
    result->data.val=nothing; /* jumped out, no relevant result */
  return result->data.val;
}

object inside_vid_assign(list,glob,update)
    symblst list; /* e.g., for 'm[i]=v': [m,v,i], for 'p|v=i': [p,i,v] */
    boolean glob;
    fobject update;
{
/* 1 Calculate expression.
   2 Clrshared `var`
   3 Compute update(var, expression),
*/
    symblst	    expr, foundsym;
    strtype	    name;
/* save old value of |repair_obj|,  which is defined in main.w */
    object repair = repair_obj;
    object expr_obj;
    expr = list->next; /* the RHS */
    name = list->a.label->name; /* name of LHS variable (without selectors) */

/* First evaluate the RHS */

    eval_value( expr); expr_obj = expr->data.val;

/* Then look up variable to be modified (which may be changed by the RHS) */

    foundsym = srchsym(topsym, name, (symblst) NULL);
    if (foundsym && foundsym->class==OPERATOR)
    error("%s is a standard function."
	  "Redefinition for same types is not allowed.\n"
	 ,name_tab[foundsym->a.name]);
    repair_obj = foundsym?foundsym->data.val:(object) NULL;

    if (update!=NULL) {

/***************************************************************
*  Compute possible selectors of the LHS		       *
***************************************************************/

	int count = 0;
	object obj[2]; 
	symblst objlst;
	for (objlst = expr->next; objlst!=NULL; ++count,objlst=objlst->next)
	{ if (count>= 2) error("System count. %d\n",count);
	  eval_value(objlst);

/***************************************************************
*  Attention: no coercion of selectors			       *
***************************************************************/

	  obj[count] = objlst->data.val;
	}
	clrshared(repair_obj); /* to allow in situ update of variable */
	clrshared(expr_obj);   /* RHS will disappear in assignment */

/* This is where we really do the update: */
	if (count == 1)
	    expr_obj = (*(f3object)update)(repair_obj, expr_obj,obj[0]);
	else 
	if (count == 2)
	    expr_obj = (*(f4object)update)(repair_obj, expr_obj,obj[0],obj[1]);
	else
	if (count == 0)
	    expr_obj = (*(f2object)update)(repair_obj, expr_obj);
	else
	    error("System count. %d\n",count);
	expr->data.val = expr_obj;
	expr->type = type_of(expr_obj);
	setshared(expr_obj);
    }

/* The following will be skipped if |update!=NULL|, because updating
   assignments may not have an undefined LHS (|error_not_foundsym| called) */

/* Now record newly defined variable, if applicable */
    if (block_depth>0) {
      if ( !(glob && foundsym)) /* new local variable */
        {
	    symblst next = topsym->next;
	    topsym->next = foundsym = newnode((labeltp) NULL);
	    foundsym->formal = name;
	    foundsym->next = next;
	}
    }
    else /* outermost evaluation level */
    if (glob) {
      if (!foundsym) /* new global variable */
        {
	    symblst next = top_definitions->next;
	    top_definitions->next = foundsym = creatsym(name);
	    foundsym->next = next; 
	}
    }
    else error("System: assignment.\n"); /* outermost cannot be local */



/***************************************************************
*  ASIGNSYM						       *
***************************************************************/

    repair_obj = foundsym?foundsym->data.val:(object) NULL;
    if (!update) clrshared(repair_obj); 
    /* Bert Lisser 11-9-90 */
    /* Lower refcount of value to be discarded in simple assignment */
    if (repair_obj && (repair_obj != expr_obj)) {
	if (type_of(repair_obj)==POLY) {
	    poly *hulp = (poly*) repair_obj;
	    freepol(hulp);
	}
	else 
	    freemem(repair_obj);
    }
    if (type_of(expr_obj) == POLY && !issorted(expr_obj)) {
	expr->data.val = (object) Reduce_pol((poly*) expr_obj);
	if (expr->data.val != expr_obj) {
	    clrshared(expr_obj);
	    setshared(expr->data.val);
	}
    }
    assignsym(foundsym,expr);

    /* 21 -11 -90 */
    expr->data.val = (object) NULL;

/***************************************************************
*  END ASSIGNSYM					       *
***************************************************************/

    repair_obj = repair;
    return (object) NULL;
}

static object vid_assign(list) symblst list;
{ return inside_vid_assign(list,true,(fobject)NULL); }

static object vid_assign_loc(list) symblst list;
{ return inside_vid_assign(list,false,(fobject)NULL); }

static object mat_copy_mat_int_int_int_a
 (matrix* a,object e, object n1, object n2, boolean addassign) 
{ index nrows, ncols;
  entry kval, lval, eval;
  matrix *result;
  nrows = a->nrows;
  ncols = a->ncols;
  kval = Integer(n1) - 1;
  lval = Integer(n2) - 1;
  if (kval < 0 || kval >= nrows)
    error("Row index (%ld) into matrix out of range. \n",(long)(kval+1));
  if (lval < 0 || lval >= ncols)
    error("Column index (%ld) into matrix out of range. \n",(long)(lval+1));
  result = isshared(a) ? copymatrix(a) :a;
  eval = Integer(e);
  if (addassign)
      *(*(result->elm + kval) + lval) += eval;
  else
      *(*(result->elm + kval) + lval) = eval;
  return (object) result;
}

static object mat_copy_mat_int_int_int (a,e,n1,n2)
    matrix *a; object n1, n2, e;
{
    return mat_copy_mat_int_int_int_a(a, e, n1, n2, false);
}

static object mat_addassign_mat_int_int_int (a,e,n1,n2) 
    matrix *a; object n1, n2, e;
{
    return mat_copy_mat_int_int_int_a(a, e, n1, n2, true);
}

static object vec_copy_vec_int_int_a(a, e, n ,plusassign) 
 vector* a; object n; object e; boolean plusassign;
{
    index  ncomp = a->ncomp;
    index	      kval;
    entry	eval;
    vector	    *result;
    kval = Integer(n) - 1;
    if (kval < 0 || kval >= ncomp)
	error("Index (%ld) into vector out of range. \n", (long)(kval+1));
    result = isshared(a) ? copyvector(a) : a ;
    eval = Integer(e);
    if (plusassign)
	result->compon[kval] += eval;
    else
	result->compon[kval] = eval;
    return (object) result;
}

static object vec_copy_vec_int_int(a, e, n) vector* a; object n, e;
{
    return vec_copy_vec_int_int_a(a, e, n ,false); 
}

static object vec_addassign_vec_int_int(a, e, n) vector* a; object n, e;
{
    return vec_copy_vec_int_int_a(a, e, n ,true); 
}

static object pol_copy_pol_bin_vec (a, e, v) poly* a; vector* v; object e;
{
    index nrows = a->nrows;
    index nvars = a->ncols;
    index index1;
    poly	  *result;
    if (nvars != v->ncomp) 
    error("%ld entries in update vector were required  - %ld present.\n",
	(long)nvars,(long)v->ncomp);
    index1=searchterm(a,v->compon);
    if (index1>=0) { /* Update */
	result = isshared(a) ? copypoly(a) : a ;
	clrshared(result->coef[index1]);
    }
    else {
    /* Add */
	int i;
	entry *elm, *compon = v->compon; 
	index1 = nrows;
	if (a->rowsize == a->nrows || isshared(a)) {
	    setshared(a);
	    result = extendpoly(a);
	    clrshared(a);
	}
	else 
	    result = a;
	elm = result->elm[nrows];
	for (i = 0;i < nvars; i++)
	     elm[i] = compon[i];
	result->nrows = nrows + 1;
    }
    if (type_of(e) == INTEGER) e = (object) entry2bigint(e->i.intval);
    result->coef[index1] = (bigint*) e;
    setshared(e);
    clrsorted(result);
    return (object) result;
}

static object mat_copy_mat_vec_int(matrix* a, vector * e, object n)
{ index nrows=a->nrows, ncols=a->ncols, ncomp=e->ncomp,kval=Integer(n)-1;
  matrix* result;
  if (kval < 0 || kval >= nrows)
      error("Row index (%ld) into matrix out of range. \n",(long)(kval+1));
  if (ncomp != ncols)
  error("Size of vector is unequal to length of rows in matrix.\n");
  result = isshared(a) ? copymatrix(a) : a ;
  copyrow(e->compon,result->elm[kval],ncomp);
  return (object) result;
}

static object pol_copy_pol_pol_int(poly* a, object e, object n)
{ index nrows=a->nrows, ncols=a->ncols;
  poly* ep=type_of(e)==INTEGER ? int2pol(&e->i)
	  :type_of(e)==BIGINT ? bin2pol(&e->b) : &e->pl, * result;
  index ncomp = ep->ncols, kval=Integer(n)-1;
  if (ep->nrows!=1)
    error("Value to assign has %ld terms instead of one term)\n"
	 ,(long)ep->nrows);
  if (kval<0 || kval >= nrows)
    error("Index (%ld) into polynomial out of range. \n",(long)(kval+1));
  if (ncomp != ncols)
    error("Number variables in term and polynomial differ (%ld <-> %ld).\n",
	  (long)ncomp,(long)ncols);
  result = isshared(a) ? copypoly(a) : a ;
  copyrow((ep->elm)[0],result->elm[kval],ncomp);
  clrshared(result->coef[kval]);
  result->coef[kval] = ep->coef[0];
  setshared(result->coef[kval]);
  clrsorted(result);
  return (object) result;
}

static object vid_update_vec_int_int(list) symblst list;
{ return inside_vid_assign(list,true,vec_copy_vec_int_int); } 

static object vid_addassign_vec_int_int(list) symblst list;
{ return inside_vid_assign(list,true,vec_addassign_vec_int_int); } 

static object vid_update_mat_int_int_int(list) symblst list;
{ return inside_vid_assign(list,true,mat_copy_mat_int_int_int); }

static object vid_addassign_mat_int_int_int(list) symblst list;
{ return inside_vid_assign(list,true,mat_addassign_mat_int_int_int); }

static object vid_update_mat_vec_int(list) symblst list;
{ return inside_vid_assign(list,true,mat_copy_mat_vec_int); } 

static object vid_vecupdate_pol_bin_vec(list) symblst list;
{ return inside_vid_assign(list,true,pol_copy_pol_bin_vec); } 

static object vid_update_pol_pol_int(list) symblst list;
{ return inside_vid_assign(list,true,pol_copy_pol_pol_int); }

static object vid_error_tex(s) object s;
{
    error("%s\n",s->t.string);
    return (object) NULL;
}

static object int_eq_tex_tex(s,t) tekst*s,* t;
{
    if (strcmp(s->string,t->string)) return (object) bool_false;
    return (object) bool_true;
}

static object tex_append_tex_tex(s,t) object s,t;
{
    object result;
    short n= s->t.len,m= t->t.len;
    result=(object) mktekst(n+m);
    strcpy(result->t.string,s->t.string);
    strcat(result->t.string,t->t.string);
    return result;
 }

static object tex_add_tex_bin(tekst* t, bigint* b)
{ string s = bigint2str(b); tekst *result = mktekst(t->len + strlen(s));
  memcpy(result->string,t->string,t->len);
  strcpy(&result->string[t->len],s);
  freem(s);
  if (!isshared(t)) freemem(t);
  return (object) result;
}

static object tex_add_bin_tex(bigint* b,tekst* t)
{ string s = bigint2str(b); size_t l= (int) strlen(s);
  tekst *result = mktekst(t->len+l);
  strcpy(result->string,s);
  strcpy(&result->string[l],t->string);
  freem(s);
  if (!isshared(t)) freemem(t);
  return (object) result;
}

static object tex_add_tex_vec(tekst* t, vector* v)
{ size_t l=1+v->ncomp; index i; char buf[20];
  tekst *result; char* p;
  if (v->ncomp==0) l=2;
  else
    for (i=0; i<v->ncomp; ++i) l+=sprintf(buf,"%ld",(long)v->compon[i]);
  result=mktekst(t->len+l); memcpy(result->string,t->string,t->len);
  p=&result->string[t->len];
  *p++='[';
  if (v->ncomp>0) p+=sprintf(p,"%ld",(long)v->compon[0]);
  for (i=1; i<v->ncomp; ++i) p+=sprintf(p,",%ld",(long)v->compon[i]);
  strcpy(p,"]");
  if (!isshared(t)) freemem(t);
  return (object) result;
}

static object tex_add_vec_tex(vector* v, tekst* t)
{ size_t l=1+v->ncomp; index i; char buf[20];
  tekst *result; char* p;
  if (v->ncomp==0) l=2;
  else
    for (i=0; i<v->ncomp; ++i) l+=sprintf(buf,"%ld",(long)v->compon[i]);
  result=mktekst(t->len+l);
  p=result->string;
  *p++='[';
  if (v->ncomp>0) p+=sprintf(p,"%ld",(long)v->compon[0]);
  for (i=1; i<v->ncomp; ++i) p+=sprintf(p,",%ld",(long)v->compon[i]);
  *p++=']';
  strcpy(p,t->string);
  if (!isshared(t)) freemem(t);
  return (object) result;
}

static object tex_add_tex_grp(tekst* t, group* g)
{ string grp_txt = grp2str(g); int n = (int)strlen(grp_txt); 
  tekst *result = mktekst(t->len + n);
  memcpy(result->string,t->string,t->len);
  strcpy(&result->string[t->len],grp_txt);
  if (!isshared(t)) freemem(t);
  freem(grp_txt);
  return (object) result;
}

static object tex_add_grp_tex( group* g, tekst* t)
{ string grp_txt = grp2str(g); size_t l = (int)strlen(grp_txt); 
  tekst *result = mktekst(t->len + l);
  strcpy(result->string,grp_txt);
  strcpy(&result->string[l],t->string);
  if (!isshared(t)) freemem(t);
  freem(grp_txt);
  return (object) result;
}

static object tex_mul_tex_int(t,d) tekst* t; intcel* d;
{
    entry n = d->intval;
    index i;
    tekst *result = mktekst(n * t->len);
    if (n <= 0) 
	error ("Negative value of multiplier for text is not allowed.\n");
    result->string[0] = '\0';
    for (i=0;i<n;i++) 
	strcat(result->string,t->string);
    if (!isshared(t)) freemem(t);
    return (object) result;
}

static object tex_mul_int_tex(intcel* d,tekst* t)
{ return tex_mul_tex_int(t,d); }

static object tex_fmt_bin_int(b,d) bigint* b; intcel* d;
{
    int w = (int)d->intval;	 
    string s = bigint2str(b);
    int n = (int)strlen(s),i;
    boolean left = (w < 0);
    int width = abs(w);
    tekst *result;
    char *str;
    if (n > width) width = n;
    result = mktekst(width);
    str = result->string;
    for (i=0;i < width; i++)  str[i] = ' ';
    if (left) strcpy(str,s);
    else
    strcpy(str+(width-n),s);
    freem(s);
    return (object) result;
}

static object vid_addr(o) object o;
 {
     Printf("%s: address = %p\n",code_type(type_of(o)),o);
     return (object) NULL;
 }

static object pol_addc_pol_pol(a,p) object	a,p;
{
    object	    result;
    index  m = a->pl.nrows, n = a->pl.ncols, mp = p->pl.nrows;
    index	      i,j,k; 
    bigint *coef;
    if (type_of(p) == INTEGER) p = (object) (*int2pol)(&p->i);
    else
    if (type_of(p) == BIGINT) p = (object) (*bin2pol)(&p->b);
    if (n != p->pl.ncols)
	error("Number of indeterminates unequal (%ld <-> %ld).\n",
	(long)n,(long)p->pl.ncols);
    if (isshared(a) || (m + mp) > a->pl.rowsize) {
	result = (object) mkpoly(2 * m + mp, n);
	result->pl.nrows = m;
	for (i = 0; i < m; i++) {
	    for (j = 0; j < n; j++)
		*(*(result->pl.elm + i)+j) = *(*(a->pl.elm + i)+j);
	    coef = a->pl.coef[i];
	    result->pl.coef[i] = coef;
	    setshared(coef);
	} 
    }
    else result = a;

/***************************************************************
*  Add new polynomial					       *
***************************************************************/

    for (i = 0, k = m; i < mp; i++,k++) {
	for (j = 0; j < n; j++)
	    *(*(result->pl.elm + k)+j) = *(*(p->pl.elm + i) + j);
	coef = p->pl.coef[i];
	result->pl.coef[k] = coef;
	setshared(coef);
    }
    result->pl.nrows = m + mp;
    clrsorted(result);
    return result;
}

static object vid_addupdate_vec_int(list) symblst list;
{ return inside_vid_assign(list,true,vec_addelm_vec_int); } 

static object vid_addupdate_mat_vec(list) symblst list;
{ return inside_vid_assign(list,true,mat_add_mat_vec); }

static object vid_addupdate_mat_mat(list) symblst list;
{ return inside_vid_assign(list,true,mat_add_mat_mat); }

static object vid_addupdate_vec_vec(list) symblst list;
{ return inside_vid_assign(list,true,vec_add_vec_vec); }

static object vid_addupdate_pol_pol(list) symblst list;
{ return inside_vid_assign(list,true,pol_addc_pol_pol); } 

static object vid_addupdate_tex_tex(list) symblst list;
{ return inside_vid_assign(list,true,tex_append_tex_tex); }

static object vid_addupdate_tex_bin(list) symblst list;
{ return inside_vid_assign(list,true,tex_add_tex_bin); } 

#ifndef  NOSAVEMAT
    static object vid_writemat_mat_grp_tex(m,g,t)
    object m; group *g; tekst *t;
    {
	Objectwrite(m,g,t->string);
	return (object) NULL;
    }

    static object vid_writestring_tex_grp_tex(s,g,t)
    object s; group *g; tekst *t;
    {
	Objectwrite(s,g,t->string);
	return (object) NULL;
    }

    static object mat_readmat_grp_tex(g,t)
    group *g; tekst *t;
    {
	return  Objectread(g,t->string);
    }

    static object tex_readstring_grp_tex(g,t)
    group *g; tekst *t;
    {
	return  Objectread(g,t->string);
    }

#endif

static object mat_getmat_tex(t) tekst* t;
{
    return  Objectget(t->string);
}

static object tex_getstring_tex(t) tekst* t;
{
    return  Objectget(t->string);
}

static object vid_savemat_mat_tex(m,t) object m; tekst* t;
{
    Objectsave(m,t->string);
    return (object) NULL;
}

static object vid_savestring_tex_tex(s,t) object s; tekst* t;
{
    Objectsave(s,t->string);
    return (object) NULL;
}
/*
 * Write here the interfaces above defined operations. 
 */

Symbrec		static4[] =
{
 M2(".forint", vid_for, ARGTYPE, INTEGER, ARGTYPE)
 M2(".downforint", vid_downfor, ARGTYPE, INTEGER, ARGTYPE)
 M2(".forvec", vid_forvec, ARGTYPE, VECTOR, ARGTYPE)
 M2(".format", vid_format, ARGTYPE, MATRIX, ARGTYPE)
 M2(".whilefun", arg_while_int_arg, ARGTYPE, INTEGER, ARGTYPE)
 M2(".whilefun", arg_while_int_arg, ARGTYPE, VECTOR, ARGTYPE)
 M2(".whilefun", arg_while_int_arg, ARGTYPE, MATRIX, ARGTYPE)
 M2(".whilefun", arg_while_int_arg, ARGTYPE, POLY, ARGTYPE)
 M1(".sequence", arg_sequence, ARGTYPE, ARGLST)
 M1("eval", vid_eval, VOID, VOID)
 C0("gcol", vid_gc, VOID)
 C1("print", vid_print, VOID, ARGTYPE)
 C1("addr", vid_addr, VOID, ARGTYPE)
 C1("void", vid_void_arg, VOID, ARGTYPE)
 C0("_vid", vid_void, VOID)
 C1("nref", arg_nref_arg, ARGTYPE, ARGTYPE)
 C2("_on",vid_on_int_tex,VOID,INTEGER, TEKST)
 C2("_off",vid_off_int_tex,VOID,INTEGER, TEKST)
 C0("_on",vid_on, VOID)
 C0("_off",vid_on, VOID)
 C0("save_options",vid_save, VOID)
 C0("restore_options",vid_restore, VOID)
#ifdef where_object
 C1("showptr", vid_showptr_tex, VOID, TEKST)
 C1("memcontrol", vid_memcontrol_int, VOID, INTEGER)
 C2("addc",bin_addc_bin_int,BIGINT,BIGINT,INTEGER)
 C2("divc",bin_divc_bin_int,BIGINT,BIGINT,INTEGER)
 C2("mulc",bin_mulc_bin_int,BIGINT,BIGINT,INTEGER)
#endif
 C1("error", vid_error_tex, VOID, TEKST)
 M2(".assign", vid_assign, VOID, VOID, ARGTYPE)
 M2(".assign_loc", vid_assign_loc, VOID, VOID, ARGTYPE)
 M3(".update", vid_update_vec_int_int, VOID, VECTOR, INTEGER, INTEGER)
 M3(".add_and_assign", vid_addassign_vec_int_int, VOID, VECTOR, INTEGER, INTEGER)
 M4(".update", vid_update_mat_int_int_int, VOID, MATRIX, INTEGER, INTEGER, INTEGER)
 M4(".add_and_assign", vid_addassign_mat_int_int_int, VOID, MATRIX, INTEGER, INTEGER, INTEGER)
 M3(".update", vid_update_mat_vec_int, VOID, MATRIX, VECTOR, INTEGER)
 M3(".vecupdate", vid_vecupdate_pol_bin_vec, VOID, POLY, BIGINT, VECTOR)
 M3(".update", vid_update_pol_pol_int, VOID, POLY, POLY, INTEGER)
 M2(".+=",vid_addupdate_vec_int, VOID,VECTOR,INTEGER)
 M2(".+=",vid_addupdate_mat_vec, VOID,MATRIX,VECTOR)
 M2(".+=",vid_addupdate_mat_mat, VOID,MATRIX,MATRIX)
 M2(".+=",vid_addupdate_vec_vec, VOID,VECTOR,VECTOR)
 M2(".+=",vid_addupdate_pol_pol, VOID,POLY,POLY)
 M2(".+=",vid_addupdate_tex_tex, VOID,TEKST,TEKST)
 M2(".+=",vid_addupdate_tex_bin, VOID,TEKST,BIGINT)
 C2("==",int_eq_tex_tex,INTEGER,TEKST,TEKST) 
 C2("^",tex_append_tex_tex,TEKST,TEKST,TEKST) 
 C2("+",tex_append_tex_tex,TEKST,TEKST,TEKST) 
 C2("+",tex_add_tex_bin,TEKST,TEKST,BIGINT) 
 C2("+",tex_add_bin_tex,TEKST,BIGINT,TEKST) 
 C2("+",tex_add_tex_grp,TEKST,TEKST,GROUP) 
 C2("+",tex_add_grp_tex,TEKST,GROUP,TEKST) 
 C2("+",tex_add_tex_vec,TEKST,TEKST,VECTOR) 
 C2("+",tex_add_vec_tex,TEKST,VECTOR,TEKST) 
 C2("*",tex_mul_int_tex,TEKST,INTEGER,TEKST) 
 C2("*",tex_mul_tex_int,TEKST,TEKST,INTEGER) 
 C2("fmt",tex_fmt_bin_int,TEKST,BIGINT,INTEGER) 

#ifndef  NOSAVEMAT
 C3("write_mat",vid_writemat_mat_grp_tex,VOID,MATRIX,GROUP,TEKST)
 C3("write_string",vid_writestring_tex_grp_tex,VOID,TEKST,GROUP,TEKST)
 C2("read_mat",mat_readmat_grp_tex,MATRIX,GROUP,TEKST)
 C2("read_string",tex_readstring_grp_tex,TEKST,GROUP,TEKST)
#endif
 C2("save_mat",vid_savemat_mat_tex,VOID,MATRIX,TEKST)
 C2("save_string",vid_savestring_tex_tex,VOID,TEKST,TEKST)
 C1("get_mat",mat_getmat_tex,MATRIX,TEKST)
 C1("get_string",tex_getstring_tex,TEKST,TEKST)

};
int nstatic4 = array_size(static4);
fobject seqfun = arg_sequence;
