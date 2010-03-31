#include "lie.h"

static object
bin_abs_bin(a)
    object	    a;
{
    object	    result;
    result = (object) copybigint((bigint*) a, (bigint*) NULL);
    result->b.size = abs(result->b.size);
    return (result);
}

static object
bin_min_bin(a)
    object	    a;
{
    object	    result;
    if (isshared(a)) 
	result = (object) copybigint((bigint*) a, NULL);
    else
	result = a;
    result->b.size = -result->b.size;
    return (result);
}

static object
int_lt_bin_bin(a, b)
    object	    a, b;
{
    boolean crit = (cmp((bigint*) a,(bigint*) b) < 0); 
    freemem(a);freemem(b);
    if (crit) 
	return (object) bool_true;
    return (object) bool_false;
}

static object
int_gt_bin_bin(a, b)
    object	    a, b;
{
    boolean crit = (cmp((bigint*) a,(bigint*) b) > 0); 
    freemem(a);freemem(b);
    if (crit)
	return (object) bool_true;
    return (object) bool_false;
}

static object
int_le_bin_bin(a, b)
    object	    a, b;
{
    boolean crit = (cmp((bigint*) a,(bigint*) b) <= 0); 
    freemem(a);
    freemem(b);
    if (crit) 
	return (object) bool_true;
    return (object) bool_false;
}

static object
int_ge_bin_bin(a, b)
    object	    a, b;
{
    boolean crit = (cmp((bigint*) a,(bigint*) b) >= 0); 
    freemem(a);freemem(b);
    if (crit) 
	return (object) bool_true;
    return (object) bool_false;
}

static object
int_eq_bin_bin(a, b)
    object	    a, b;
{
    boolean crit = cmp((bigint*) a,(bigint*) b); 
    freemem(a); freemem(b);
    if (!crit)
	return (object) bool_true;
    return (object) bool_false;
}

static object
int_ne_bin_bin(a, b)
    object	    a, b;
{
    boolean crit = cmp((bigint*) a,(bigint*) b); 
    freemem(a); freemem(b);
    if (crit) 
	return (object) bool_true;
    return (object) bool_false;
}

static object bin_add_bin_bin(a,b)
       object a,b;
{
    return (object) add((bigint*) a , (bigint*) b);
}

static object bin_sub_bin_bin(a,b)
     object a,b;
{
    return (object) sub((bigint*) a , (bigint*) b);
}


static object 
bin_mul_bin_bin(a,b)
       object a,b;
{
    return (object) mult((bigint*) a,(bigint*) b);
}

static object 
bin_div_bin_bin(a,b)
       object a,b;
{
    return (object) quotient((bigint*) a,(bigint*) b);
}

static object 
bin_mod_bin_bin(a,b)
    object a,b;
{
    return (object) mod((bigint*) a,(bigint*) b);
}

 static object addupdate_bin_bin(a,b) 
 object a,b ;
 {
     bigint* c,* d,* result; 
     setshared(a);
     if (type_of(a) == INTEGER) c = (*int2bin)(&a->i);
     else c = (bigint*) a;
     if (type_of(b) == INTEGER) d = (*int2bin)(&b->i);
     else d = (bigint*) b;
     result =  add(c,d);
     clrshared(a);
     return (object) result;
 }

static object
vid_addupdate_bin_bin(list)
    symblst	    list;
{return inside_vid_assign(list,true,addupdate_bin_bin);} 


#ifdef development
static object bin_addc_bin_int(a,b)
bigint *a; intcel *b;
{
    addc(&a,b->intval);
    return (object) a;
}

static object bin_mulc_bin_int(a,b)
bigint *a; intcel *b;
{
    mulc(&a,b->intval);
    return (object) a;
}

static object bin_divc_bin_int(a,b)
bigint *a; intcel *b;
{
    divc(&a,b->intval);
    return (object) a;
}
#endif

Symbrec static7[] = {
    C1("abs", bin_abs_bin, BIGINT, BIGINT)
    C1("-", bin_min_bin, BIGINT, BIGINT)
    C2("==", int_eq_bin_bin, INTEGER, BIGINT, BIGINT)
    C2("!=", int_ne_bin_bin, INTEGER, BIGINT, BIGINT)
    C2("<=", int_le_bin_bin, INTEGER, BIGINT, BIGINT)
    C2(">=", int_ge_bin_bin, INTEGER, BIGINT, BIGINT)
    C2("<", int_lt_bin_bin, INTEGER, BIGINT, BIGINT)
    C2(">", int_gt_bin_bin, INTEGER, BIGINT, BIGINT)
    C2("+", bin_add_bin_bin, BIGINT, BIGINT, BIGINT)
    C2("-", bin_sub_bin_bin, BIGINT, BIGINT, BIGINT)
    C2("*",bin_mul_bin_bin,BIGINT,BIGINT,BIGINT)
    C2("/",bin_div_bin_bin,BIGINT,BIGINT,BIGINT)
    C2("%",bin_mod_bin_bin,BIGINT,BIGINT,BIGINT)
    M2(".+=",vid_addupdate_bin_bin, VOID,BIGINT,BIGINT)
#ifdef development
    C2("addc",bin_addc_bin_int, BIGINT, BIGINT, INTEGER)
    C2("mulc",bin_mulc_bin_int, BIGINT, BIGINT, INTEGER)
    C2("divc",bin_divc_bin_int, BIGINT, BIGINT, INTEGER)
#endif
};
int nstatic7 = array_size(static7);
