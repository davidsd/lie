#include  "lie.h"
#define full_error  error("Reduce space full.\n" \
                    "Try increasing maxnodes (currently %ld).\n" \
		   ,(long)maxnodes)
#define relocate(p,src_base,dst_base) (p=&(dst_base)[(p)-(src_base)])


static symblst tree_ar;
static labeltp label_ar;
static labelrec labelrec_null= {0,0,NULL}; 

static struct { objclass num; char* str;} class_name [] =
{ { VALUE, "value" }
, { OPERATOR, "operator" }
, { FUNCTION, "function"  }
, { FUNCTION_COPIED, "function_copied" }
, { MAP, "map" }
, { DUMMY, "dum" }
, { ARG, "arg" }
, { BLOCK, "block" }
};

int tree_pt=0, label_pt=0;
unsigned long maxnodes=0, maxlabels=0;
labeltp label_null=&labelrec_null;

symbrec blank_symbol={UNKNOWN,DUMMY,{empty_str},empty_str,{NULL},NULL,NULL};

void inittree(void)
{ if (maxnodes<2) maxnodes=MAXNODES_DFLT;
  maxlabels=maxnodes/2;
tree_ar=alloc_array(symbrec,maxnodes);
  label_ar =alloc_array(labelrec,maxlabels);
}

void newtree(void)
{ if (tree_pt!=0) fatal("System error. newtree.\n");
  freearr(tree_ar); freearr(label_ar); inittree();
  Printf("New tree space with maximum number of nodes: %ld.\n",
  (long) maxnodes);
}

static void creatnode(symbrec* s,strtype name)
{ *s=blank_symbol;
  if (name==empty_str) s->a.label=label_null;
  else
  { if (label_pt >= maxlabels)
      error("Too big program (maxnodes %ld). Try increasing maxnodes.\n"
	   , (long)maxnodes);
    label_ar[label_pt].name=name; /* name of node */
    label_ar[label_pt].line=lineno; label_ar[label_pt].fname=input_fname;
      /* \LiE. source file and line number */
    s->a.label=&label_ar[label_pt++];
  }
}

symblst	newnode(labeltp a)
{ if (tree_pt>=maxnodes) full_error;
  creatnode(&tree_ar[tree_pt],0);
  tree_ar[tree_pt].a.label =a; /* use label |a| instead of |label_null| */
  return &tree_ar[tree_pt++];
}

symblst addnode(int n, strtype name, ...)
  /* extra arguments have type |(symbrec*)| */
{ if (tree_pt >= maxnodes) full_error;
  creatnode(&tree_ar[tree_pt], name);
  if (n>0) /* then attach the arguments */
  { va_list ap; int i; symblst* t=&tree_ar[tree_pt].arglst;
    va_start(ap, name);
    for (i=0; i<n; ++i)
    { *t=va_arg(ap,symblst); t=&(*t)->next; }
    va_end(ap);
  }
  return &tree_ar[tree_pt++];
}

symblst addintnode(entry n)
{ symblst s=addnode(0, empty_str);
  s->class=VALUE;
  s->type=INTEGER;
  s->data.val=(object)mkintcel(n);
  setshared(s->data.val);
  return s;
}

symblst addvecnode(int n)
{ symblst s=addnode(0, empty_str);
  s->class=VALUE;
  s->type=VECTOR;
  s->data.val=(object)mkvector(n);
  setshared(s->data.val);
  return s;
}

symblst addbigintnode(bigint* b)
{ symblst s=addnode(0, empty_str);
  s->class=VALUE;
  s->type=BIGINT;
  s->data.val=(object)b;
  setshared(s->data.val);
  return s;
}

symblst addgroupnode(char lietype,index rank)
{ symblst s=addnode(0,empty_str); s->type=GROUP; s->class=VALUE;
  if (lietype=='T')
  { s->data.val=(object) mkgroup((index) 0); s->data.val->g.toraldim=rank; }
  else
  { s->data.val=(object) mkgroup((index) 1);
    Liecomp(s->data.val,0)=mksimpgrp(lietype, rank);
  }
  setshared(s->data.val); return s;
}

symblst addtekstnode(char* t)
{ symblst s=addnode(0, empty_str);
  s->class=VALUE;
  s->type=TEKST;
  s->data.val=(object)mktekst(strlen(t));
  setshared(s->data.val);
  strcpy(s->data.val->t.string, t);
  return s;
}

symblst addnamenode(strtype name)
{ symblst s=addnode(0,name);
  s->class=VALUE;
  s->type=VOID;
  return s;
}

symblst addlabelnode(labeltp l)
{ symblst s=newnode(l);
  s->class=VALUE;
  s->type=VOID;
  return s;
}

symblst addtypnode(objtype typno)
{ symblst s=addnode(0, empty_str);
  s->class=DUMMY;
  s->type=typno;
  return s;
}

symblst addtypnamenode(objtype typno, strtype name)
{ symblst s=addnode(0, name);
  s->class=DUMMY;
  s->type=typno;
  return s;
}

symblst addprogramnode(symblst prog)
{ symblst s=addnode(1, match(".sequence",false),prog); return s; }

void addstaynode(object o)
{ symblst s=newnode(NULL);
  s->class=VALUE;
  s->type=type_of(o);
  s->data.val=o;
}


char* code_type(objtype typeno)
{ static char buff[2][8]; static int alt=0; /* alternates between calls */

  char* p=buff[alt=1-alt], *s;
  if ((typeno&FUN)!=0) { strcpy(p,"fun "); typeno&=~FUN; p+=4; }
  if ((s=code_obj_type(typeno&TYPEMASK))!=NULL) strcpy(p,s);
  else if ((s=code_obj_type(typeno&(TYPEMASK&~ARR)))!=NULL)
    { strcpy(p,s); p[2]='l'; }
  else strcpy(p,"???");
  return buff[alt];
}

char* code_class(int classno)
{ static char buff[sizeof("function_copied")+1];
  int i;
  for (i=0; i<array_size(class_name); ++i)
    if (classno==class_name[i].num)
    return sprintf(buff,"%s:",class_name[i].str),buff;
  return "sys error:";
}

symblst copynode(symblst t)
{
  symblst s;
  if (tree_pt >= maxnodes) full_error;
  s=tree_ar+tree_pt++;
  assignsym(s, t);
  if (s->class==VALUE)
  { if (!isshared(t->data.val)) t->data.val=NULL;
    setshared(s->data.val);
  }
  s->a.label=t->a.label;
  s->formal=t->formal;
  return s;
}

symblst copytree(symblst s)
{
  symblst r;
  if (s==NULL) return NULL;
  r=copynode(s);
  r->arglst=copytree(s->arglst);
  r->next=copytree(s->next);
  if (s->class==BLOCK) r->data.expr=copytree(s->data.expr);
  return r;
}

symblst load_function_body(funclst func)
{ int i,n_nodes=func->n_nodes;
  symbrec* result=func->top_expr;
  symblst src=func->start_nodes, dst=&tree_ar[tree_pt];
  if ((tree_pt+=n_nodes)>maxnodes) full_error;
  for (i=0; i<n_nodes; ++i)
  { symblst node=&dst[i];  *node=src[i]; /* copy entire node */
    if (node->arglst!=NULL) relocate(node->arglst,src,dst);
    if (node->next!=NULL) relocate(node->next,src,dst);
    if (node->class==BLOCK) relocate(node->data.expr,src,dst);
    else if (node->class==VALUE) setshared(node->data.val);
  }
  relocate(result,src,dst);
  return result;
}

funclst store_function_body(symblst arg,symblst expr)
{ funclst result; int i;
  
  result=(funclst)
  allocmem(sizeof(funcrec)+tree_pt * sizeof(symbrec)+label_pt*sizeof(labelrec));
  
  { result->start_args=(symbrec*) &result[1];
      /* arguments and nodes follow |funcrec| */
    result->start_labels=(labeltp) &result->start_args[tree_pt];
      /* labels follow nodes */
    result->n_labels=label_pt; /* all current labels are in this function */
    result->n_args= arg==NULL ? 0 : (int)(arg-tree_ar)+1;
      /* |*arg| is last argument, unless |arg==NULL| */
    result->start_nodes= &result->start_args[result->n_args];
      /* nodes follow the arguments */
    result->top_arg=
      result->n_args==0 ? NULL : &result->start_args[result->n_args-1];
    result->n_nodes=tree_pt-result->n_args;
    result->top_expr=expr;
    relocate(result->top_expr,tree_ar,result->start_args);
  }
  
  for (i=0; i<label_pt; ++i) result->start_labels[i]= label_ar[i];
  
  { symblst src=tree_ar, dst=result->start_args;
    for (i=0; i<tree_pt; ++i)
    { symbrec* node=&dst[i];  *node=src[i];
      if (node->next!=NULL) relocate(node->next,src,dst);
      if (node->arglst!=NULL) relocate(node->arglst,src,dst);
      if (node->class==BLOCK) relocate(node->data.expr,src,dst);
      if (node->a.label!=label_null)
        relocate(node->a.label,label_ar,result->start_labels);
    }
  }
  return result;
}

void mark_tree(void)
{ mark_expression(tree_ar,tree_pt); }

void assign_type_to_node(symblst s)
{
  object t=s->data.val;
  if (t==NULL && s->type== VOID) return;
  if (s->type==UNKNOWN || s->type==ARGTYPE
       || stop_loop!=NULL || stop_fun!=NULL)
  { s->type= t==NULL ? VOID : type_of(t); return; }
  if (t!=NULL) {
      if (s->type==type_of(t)) return;
      if (coerc_tab(type_of(t),s->type))
	{ s->type=type_of(t); return; }
  }
  error("Type %s expected - type %s computed.\n",
  code_type(s->type), t?code_type(type_of(t)):"vid");
}

void assignobject(symblst s,object t)
{
    s->data.val=t;
    s->class=VALUE;
    assign_type_to_node(s);
    s->a.label=label_null;
}

object force_integer(symblst topstack)
{
    object i_dex;
    object top_obj=topstack->data.val;
    if (!is_int(type_of(top_obj)))
	error("Integer expected - %s  encountered.\n",code_type(type_of(
		top_obj)));
    if (!isshared(top_obj) && type_of(top_obj)==INTEGER)
    {
	setshared(top_obj);
	return i_dex=top_obj;
    }
    if (type_of(top_obj)==BIGINT) i_dex=(object) (*bin2int)(&top_obj->b);
	else i_dex=cpobject(top_obj);
    setshared(i_dex);
    topstack->data.val=i_dex;
    assign_type_to_node(topstack);
    return i_dex;
}

