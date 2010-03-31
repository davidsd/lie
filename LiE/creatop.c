#include "lie.h"


static symblst creatop(strtype a,fobject f,objtype restype)
{
    symblst	    s;
    s = creatsym(a);
    s->class = OPERATOR;
    s->type = restype;
    s->data.f = f;
    return (s);
}

static symblst mkarg(int n) /* arguments allocated outside reach of GC */
{ if (n == 0) return NULL;
  return alloc_array(symbrec,n);
}

symblst creatopsym
(int n, strtype a, fobject f, int restype, ...)
{ va_list  ap; int i, argtype; symblst t, s;
  va_start(ap, restype);
  s = creatop(a, f, restype);
  s->arglst = t = mkarg(n);
  for (i = 0; i < n; ++i)
  { t->class = DUMMY; t->a.name =  0;
    argtype = va_arg(ap, int);
    t->type = (objtype) argtype;
    t= t->next = (i==n-1 ? NULL : t + 1);
  }
  va_end(ap);
  return s;
}

