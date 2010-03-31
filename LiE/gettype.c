#include  "lie.h"
#define RETURNSTACK_LEN  200
#define VAR_LEN  1000


static struct { strtype name; objtype type; }	type_stack[VAR_LEN];
static objtype return_type[RETURNSTACK_LEN];
static int return_pt, var_pt;


static objtype last_type(symblst s)
{ if (s==NULL) return VOID;
  while (s->next!=NULL) s=s->next;
  return s->type;
}

static void add_var(strtype name,objtype type)
{
  if (var_pt>=VAR_LEN)
    error("Number of variables (%ld) exceeded.\n",(long)VAR_LEN);
  type_stack[var_pt].name=name;
  type_stack[var_pt++].type=type;
}

static void push_type(symblst list)
{
  for ( ; list!=NULL; list=list->next) add_var(list->formal,list->type);
}

static objtype type_var(strtype name)
{ int i=var_pt-1;
  for (i=var_pt-1; i>=0; --i)
    if (type_stack[i].name==name) return type_stack[i].type;
  { symblst foundsym=srchsym(topsym,name,NULL);
    if (foundsym==NULL || foundsym->class!=VALUE) return ERROR;
    return foundsym->type;
  }
}

static void check_function_args(symblst sym,symblst foundsym)
{ symblst f,s;
  for (f=foundsym->arglst, s=sym->arglst; f!=NULL && s!=NULL;
       f=f->next,s=s->next)
    if (isfuntype(f) && s->class != BLOCK)
      error("%s is not a user defined function. \n"
	   ,name_tab[s->a.label->name]);
}

static void check_var_type(strtype name,objtype type)
{
  int i; objtype old_type;
  if (type==UNKNOWN) return; /* assume all is well in this case */
  for (i=var_pt-1; i>=0; --i)
    if (type_stack[i].name==name)
      { old_type=type_stack[i].type; goto found; }
  { symblst foundsym=srchsym(topsym,name,NULL);
    add_var(name,type); /* add local variable, whether or not found */
    if (foundsym==NULL) return;
    old_type=foundsym->type;
  }
found:
  if (block_depth==0)
    { if (i>=0) type_stack[i].type=type; } /* change type */
  else
    if (!eq_types(type,old_type))
      error("Illegal type change for %s: %s -> %s,\n"
	   ,name_tab[name],code_obj_type(old_type),code_obj_type(type));
}


extern fobject seqfun;

void init_loop_gettype(void) { var_pt=return_pt=0; }

objtype eval_type(symblst s)
{
  symblst t=s->arglst; strtype name=s->a.label->name;
  labeltp label0=label;
  label=s->a.label;

for (; t!=NULL; t=t->next)
{ if (t->type==UNKNOWN)
  { if (eval_type(t)==UNKNOWN && name_tab[name][0]!='.') return UNKNOWN;
    if (t->a.label!=NULL && t->a.label!=label_null
	&& t->a.label->name==return_name)
      { if (return_pt==RETURNSTACK_LEN) error("Too many happy returns:\n");
	return_type[return_pt++]=t->type;
      }
  }
}
  label=label0; /* restore |label| after handling arguments */
  
  {
    symblst foundsym; strtype name=s->a.label->name;
  
    if (s->type!=UNKNOWN) return s->type;
    if (name==0) error("System error: eval_type0: no_name.\n");
    if (name==seq_name)
    { s->class=MAP; s->data.f=seqfun; return s->type=last_type(s->arglst); }
    if (s->class==BLOCK) 
                     { int var_pt0=var_pt; ++block_depth; push_type(s->arglst);
                     s->type=eval_type(s->data.expr);
                     var_pt=var_pt0; --block_depth;
                       return s->type;
                     }
    
    { objtype type;
      if (s->arglst==NULL && (type=type_var(name))!=ERROR)
      { s->class=DUMMY; return s->type=type; }
    }
    
    { if ((foundsym=srchsym(symbol_tab[name],name,s->arglst))==NULL)
        foundsym=srchsym(topsym, name, s->arglst);
      if (foundsym==NULL) error_not_foundsym(s);
      check_function_args(s,foundsym);
      assignsym(s,foundsym); /* set |s->class| and |s->type| */
    
      if (s->class==OPERATOR) s->data.expr=foundsym; /* save binding found */
    }
    if (name==assign_name || name==assign_loc_name)
      
      { symblst lhs=s->arglst, rhs=lhs->next;
        strtype varname=lhs->a.label->name; /* name of left hand side */
        if (rhs->type==VOID)
        error("You cannot assign a %s value.\n", code_obj_type(VOID));
        if (name==assign_loc_name)
        { if (block_depth==0) error
      	("'loc %s= .. ' can only be used inside functions/blocks.\n"
      	,name_tab[varname]);
          add_var(varname, rhs->type);
        }
        else check_var_type(varname,rhs->type);
        return s->type=VOID;
      }
    if (name==if_name) 
                     { symblst then_part=s->arglst->next, else_part=then_part->next;
                       if (else_part==NULL) return s->type=VOID;
                       if (!eq_types(then_part->type,else_part->type)
                        && then_part->type!=UNKNOWN && else_part->type!=UNKNOWN)
                         error("Type mismatch between branches of if-clause: %s and %s.\n"
                     	 ,code_obj_type(then_part->type), code_obj_type(else_part->type));
                       return s->type=
                         then_part->type!=UNKNOWN ? then_part->type : else_part->type;
                     }
    if (s->type==ARGTYPE)
      
      { if ((s->type=last_type(s->arglst))!=UNKNOWN) return s->type; }
    if (s->class==FUNCTION) 
                        { int var_pt0=var_pt, return_pt0=return_pt;
                          strtype fun_name_old=fun_name;
                          
                          { fun_name=foundsym->a.name;
                            ++block_depth;
                            assignnames(s, foundsym->arglst);
                            push_type(s->arglst);
                          }
                          
                          { s->data.expr=load_function_body(foundsym->data.func);
                            foundsym->class=FUNCTION_COPIED;
                            s->type= eval_type(s->data.expr);
                            foundsym->class=FUNCTION;
                            s->class=BLOCK;
                          }
                          
                          { int i;
                            for (i=return_pt0; i<return_pt; ++i)
                              if (!eq_types(return_type[i],s->type)
                               && return_type[i]!=UNKNOWN && s->type!=UNKNOWN) break;
                              else if (s->type==UNKNOWN) s->type=return_type[i];
                            if (i<return_pt)
                            { Printf("Inconsistent return types for %s:\n",name_tab[name]);
                              for (i=return_pt0; i<return_pt; ++i)
                                Printf("return nr.%d has type: %s,\n"
                          	    , i-return_pt+1, code_obj_type(return_type[i]));
                              error("function body has type: %s\n",code_obj_type(s->type));
                            }
                            if (s->type==UNKNOWN)
                              error("The result of function %s has indeterminate type.\n"
                          	 ,name_tab[name]);
                          }
                          
                          { var_pt=var_pt0;
                            return_pt=return_pt0;
                            fun_name=fun_name_old;
                            --block_depth;
                          }
                          return s->type;
                        }
    return s->type; /* if |s->class==FUNCTION_COPIED|, this is |UNKNOWN| */
  }
}

