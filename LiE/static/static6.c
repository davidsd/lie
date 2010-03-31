#include "lie.h"

static object
int_abs_int(a)
    object	    a;
{
    object	    result;
    result = (object) mkintcel(labs(a->i.intval));
    return (result);
}

static object
int_min_int(a)
    object	    a;
{
    if (isshared(a))
	return (object) mkintcel(-a->i.intval);
    else {
	a->i.intval = -a->i.intval;
	return a;
    }
}

static object
int_lt_int_int(a, b)
    object	    a, b;
{
    boolean crit = (a->i.intval < b->i.intval); 
    freemem(a);freemem(b);
    if (crit)
	return (object) bool_true;
    return (object) bool_false;
}

static object
int_gt_int_int(a, b)
    object	    a, b;
{
    boolean crit = (a->i.intval > b->i.intval); 
    freemem(a);freemem(b);
    if (crit) 
	return (object) bool_true;
    return (object) bool_false;
}

static object
int_le_int_int(a, b)
    object	    a, b;
{
    boolean crit = (a->i.intval <= b->i.intval); 
    freemem(a);freemem(b);
    if (crit) 
	return (object) bool_true;
    return (object) bool_false;
}

static object
int_ge_int_int(a, b)
    object	    a, b;
{
    boolean crit = (a->i.intval >= b->i.intval); 
    freemem(a);freemem(b);
    if (crit) 
	return (object) bool_true;
    return (object) bool_false;
}

static object
int_eq_int_int(a, b)
    object	    a, b;
{
    boolean crit = (a->i.intval == b->i.intval);
    freemem(a);freemem(b);
    if (crit) return (object) bool_true;
    return (object) bool_false;
}
static object
int_ne_int_int(a, b)
    object	    a, b;
{
    boolean crit = (a->i.intval != b->i.intval);
    freemem(a);freemem(b);
    if (crit) return (object) bool_true;
    return (object) bool_false;
}

static object
int_add_int_int(a, b)
    object	    a, b;
{
    object	    result;
    result = (object) mkintcel(a->i.intval + b->i.intval);
    return (result);
}

static object
int_dif_int_int(a, b)
    object	    a, b;
{
    object	    result;
    result = (object) mkintcel(a->i.intval - b->i.intval);
    return (result);
}

static object
int_mul_int_int(a, b)
    object	    a, b;
{
    object	    result;
    result = (object) mkintcel(a->i.intval * b->i.intval);
    return (result);
}

static object int_div_int_int(object a, object b)
{ if (b->i.intval==0) error("Division by zero.\n");
  return (object) mkintcel(a->i.intval / b->i.intval);
}

static object int_mod_int_int(object a, object b)
{ entry x = a->i.intval,n=b->i.intval;
  if (n <= 0) error("Modulus must be positive number.\n");
  return (object) mkintcel(x>=0?x%n:((-x)%n==0?0:n-((-x)%n)));
}

 static object addupdate_int_int(a,b) 
 object a,b ;
 {
     object result;
     if (type_of(a) == BIGINT || isshared(a)) 
	 result = (object) mkintcel(Integer(a)); 
     else result = a;
     result -> i.intval += Integer(b);
     return result;
 }

static object
vid_addupdate_int_int(list)
    symblst	    list;
{return inside_vid_assign(list,true,addupdate_int_int);} 

Symbrec static6[] = {
    C1("abs", int_abs_int, INTEGER, INTEGER)
    C2("==", int_eq_int_int, INTEGER, INTEGER, INTEGER)
    C2("!=", int_ne_int_int, INTEGER, INTEGER, INTEGER)
    C2("<=", int_le_int_int, INTEGER, INTEGER, INTEGER)
    C2(">=", int_ge_int_int, INTEGER, INTEGER, INTEGER)
    C2("<", int_lt_int_int, INTEGER, INTEGER, INTEGER)
    C2(">", int_gt_int_int, INTEGER, INTEGER, INTEGER)
    C1("-", int_min_int, INTEGER, INTEGER)
    C2("+", int_add_int_int, INTEGER, INTEGER, INTEGER)
    C2("-", int_dif_int_int, INTEGER, INTEGER, INTEGER)
    C2("*", int_mul_int_int, INTEGER, INTEGER, INTEGER)
    C2("/", int_div_int_int, INTEGER, INTEGER, INTEGER)
    C2("%", int_mod_int_int, INTEGER, INTEGER, INTEGER)
    M2(".+=",vid_addupdate_int_int, VOID,INTEGER,INTEGER)
};
int nstatic6 = array_size(static6);
