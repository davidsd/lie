#include  "lie.h"


static boolean match_arg(symblst pattern, symblst actual, boolean deflt)
{ while (pattern!=NULL && actual!=NULL)
    if (   type_of(actual)!=type_of(pattern)
	&& type_of(pattern)!=ARGTYPE
	&& type_of(actual)!=UNKNOWN
	&& !coerc_tab(type_of(actual),type_of(pattern))
       ) return false;
    else
    { actual=actual->next; if (!isarrtype(pattern)) pattern=pattern->next; }
  return actual==NULL
      && (pattern==NULL || isarrtype(pattern) || deflt&&isdefault(pattern));
}

symblst srchsym(symblst table, strtype name, symblst arg)
{ symblst s;
  if (table==topsym)
    /* then search stack first, and user definitions afterwards */
  { for (s=table ; s!=NULL; s=s->next)
      if (s->formal==name) return isargument=true, s;
      table=top_definitions;
  }

  for (s=table ; s!=NULL; s=s->next)
    if (s->a.name==name && match_arg(s->arglst, arg, false))
      return isargument=false, s;
  for (s=table ; s!=NULL; s=s->next)
    if (s->a.name==name && match_arg(s->arglst, arg, true))
      return isargument=false, s;
  return isargument=false,s; /* |s==NULL|, not found anywhere */
}


void
addsym(strtype name, symblst arg, symblst expr, int class)
{ symblst s;
  
  { symblst ref=symbol_tab[name]; /* the permanant bindings */
    if (ref!=NULL && srchsym(ref,name,arg)!=NULL) /* one of them matches */
    { err_Printf("Sorry, there is a primitive definition for ");
      error_prototype(name_tab[name],arg);
      error("\nwhich you cannot redefine.\n");
    }
  }
  s=srchsym(top_definitions, name, arg);
    /* if an old binding matches, overwrite it */
  if (s==NULL)
  { s=creatsym(name);
    s->next=top_definitions->next;
    top_definitions->next=s;
  }
  s->class=class;
  if (class==FUNCTION)
  { s->data.func=store_function_body(arg, expr);
    s->arglst=s->data.func->top_arg;
    s->type=UNKNOWN;
  }
  else if (class==VALUE) { clrshared(s->data.val); assignsym(s,expr); }
}


symbrec* creatsym(strtype a)
{ symbrec* s = (symbrec*) allocmem(sizeof(symbrec));
  *s=blank_symbol; s->a.name = a;
  return s;
}

void error_prototype(string name, symblst arg)
{ err_Printf("%s(",name);
  if (arg!=NULL)
    while (err_Printf("%3s",code_obj_type(type_of(arg))),(arg=arg->next)!=NULL)
      err_Printf(", ");
  err_Printf(")");
}

void assignsym(symblst s, symblst t) /* |s=t| */
{ s->class=t->class; s->type=t->type; s->data=t->data; }

void push(symblst l, symblst sym)
{ sym->next = l->next;
   l->next=sym;
 }

symblst push_value(symblst list)
{ symblst* p=&list, q;
  for (q=list; q!=NULL; q=q->next) /* duplicate list */
  { *p=copynode(q); (*p)->arglst=q->arglst; p=&(*p)->next; }
  *p=topsym->next; return topsym->next=list; /* insert list after |topsym| */
}

symblst pop_value(symblst topoldsym)
  /* clear sharing in stack down to |topoldsym| */
{ symblst list;
  for (list=topsym->next; list != topoldsym; list=list->next)
    if (list->class==VALUE) clrshared(list->data.val);
  return topoldsym;
}

void assignnames(symblst actual, symblst formal)
{ symblst* p;
  for (p=&actual->arglst; *p!=NULL; p=&(*p)->next, formal=formal->next)
    (*p)->formal=formal->a.label->name;
  if (formal!=NULL)
    
    { strtype name=match("_gsetdefault",false);
      symblst foundsym=srchsym(symbol_tab[name],name,NULL);
      *p=addnode(0,name),
      assignsym(*p,foundsym); (*p)->formal=formal->a.label->name;
      (*p)->data.expr=foundsym;
    }
}

void assigntype(objtype t, symblst list)
{
  for ( ;list!=NULL; list=list->next)
  { list->type= list->next==NULL && t==GROUP ? GRPDFT : t;
    list->class=DUMMY;
  }
}

void add_dollar_names(symblst list)
{ static char *name[]=
    {"$1","$2","$3","$4","$5","$6","$7","$8","$9","$10"};
  int i;
  for (i=0; i<10 && list!=NULL; ++i,list=list->next)
    list->formal=match(name[i],false);
  if (list!=NULL) error("At most 10 arguments to a block allowed.\n");
}

void error_not_foundsym(symblst s)
{ strtype id=s->a.label->name; char* name = name_tab[id];
  label->line = s->a.label->line; /* adjust error indication */
  
  { if (strcmp(name, "_select")==0)
      error("Impossible to select component from %s.\n"
  	 , code_type(s->arglst->type));
    if (strcmp(name, ".strucval")==0) error("Array has inconsistent types.\n");
  }
  if (name[0] == '.') ++name; /* skip |'.'| */
  
  { symblst p;
    for (p=top_definitions; p!=NULL; p=p->next) 
      if (p->a.name==id) goto found;
    for (p=symbol_tab[id]; p!=NULL; p=p->next) 
      if (p->a.name==id) goto found;
    if (false) /* i.e., |if (p!=NULL)|; only do next after |goto found|: */
    { found:
      if (s->arglst==NULL) err_Printf("Arguments required for %s.\n",name);
      else
      { err_Printf("Argument types do not match in call. Types are: ");
        error_prototype(name,s->arglst); err_Printf(".\n");
      }
      err_Printf("Valid argument types are for instance: ");
      error_prototype(name,p->arglst); error(".\n");
    }
  }
  error("Identifier %s is not defined. \n",name);
}

