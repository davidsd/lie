#include  "lie.h"

extern long	chunks;


    static void clear_arguments(symblst argsym);


static void evalbl_value(symblst topnode)
{
    symblst topoldsym=topsym->next;
    strtype name = topnode->a.label->name;
    strtype fun_name_old = fun_name;
    int tree_pt0 = tree_pt;
    object stop_later = stop_fun;
    if (name != block_name) {	stop_fun = NULL; fun_name = name; }
    
    {   block_depth++;
        push_value(topnode->arglst);
        eval_value(topnode->data.expr);
        topnode->a.label = topnode->data.expr->a.label;
        assignsym(topnode, topnode->data.expr);
        /* Recover symbol table */
        topsym->next = pop_value(topoldsym);
        tree_pt = tree_pt0;
        block_depth--;
    }
    if (name != block_name)
      { if (stop_fun!=NULL)
	{ topnode->class=VALUE; topnode->data.val=stop_fun;
	  topnode->type=type_of(stop_fun);
	}
	stop_fun=stop_later; fun_name = fun_name_old;
      }
}

void evalbl_value_dup(symblst result,symblst topnode)
{
    int		tree_pt0 = tree_pt;
    assignsym(result,topnode);
    result->a.label = topnode->a.label;
    result->arglst = topnode->arglst;
    result->data.expr = copytree(result->data.expr);
    evalbl_value(result);
    tree_pt = tree_pt0;
}


void eval_value(symblst topnode)
{
    symblst	    argsym = topnode->arglst, foundsym;
    strtype name = topnode->a.label->name;
    object result;

    
        if (topnode->class == DUMMY && type_of(topnode) != UNKNOWN)
    	/* Change variable value after assignment */
        {
    	foundsym = srchsym(topsym, name, (symblst) NULL);
    	if (!foundsym) {
    #if development
    	    Printf("System eval_value: symbol %s not found.\n"
    		  ,name_tab[name]);
    #endif
    	    error_not_foundsym(topnode);
    	}
    	assignsym(topnode, foundsym);
    	setshared(topnode->data.val);
    	if (isargument)
    	    topnode->a.label = foundsym->a.label;
        }
    switch (topnode->class) {
    case VALUE:
	assign_type_to_node(topnode);
	return;

    case MAP:
	
	{
	    labeltp label0 = label;
	    label = topnode->a.label;
	    result = (*(symbfobj) topnode->data.f) (argsym);
	    topnode->class = VALUE;
	    topnode->a.label = label_null;
	    topnode->data.val = result;
	    assign_type_to_node(topnode);
	    clear_arguments(topnode->arglst);
	    topnode->arglst = NULL;
	    label = label0;
	}
 break;

    case OPERATOR:
	
	{
	    int    i = 0;
	    symblst	    argsym = topnode->arglst;
	    symblst	    argsym_expected = topnode->data.expr->arglst;
	    object	    arg[6];
	    fobject	    fn = topnode->data.expr->data.f;
	    labeltp label0 = label;
	    label = topnode->a.label;
	    
	        while (argsym!=NULL) {
	    	f1object coerc;
	    
	    	if (argsym->class != VALUE)
	    	    eval_value(argsym);
	    	clrshared(argsym->data.val);
	    
	    	coerc = (f1object) coerc_tab(type_of(argsym),type_of(argsym_expected));
	    
	    	if (coerc) { argsym->data.val = coerc(argsym->data.val); }
	    	arg[i] = argsym->data.val;
	    
	    	argsym = argsym->next;
	    	argsym_expected = argsym_expected->next;
	    
	    	i++;
	        }
	    
	        if (argsym_expected && isdefault(argsym_expected)){
	           if (!defaultgrp) error("Defaultgroup expected.\n");
	           arg[i++] = defaultgrp;
	        }
	    
	    {   switch (i) {
	        case 0:
	    	result = (*(f0object)fn)();
	    	break;
	        case 1:
	    	result = (*(f1object)fn)(arg[0]);
	    	break;
	        case 2:
	    	result = (*(f2object)fn)(arg[0], arg[1]);
	    	break;
	        case 3:
	    	result = (*(f3object)fn)(arg[0], arg[1], arg[2]);
	    	break;
	        case 4:
	    	result = (*(f4object)fn)(arg[0], arg[1], arg[2], arg[3]);
	    	break;
	        case 5:
	    	result = (*(f5object)fn)(arg[0], arg[1], arg[2], arg[3], arg[4]);
	    	break;
	        case 6:
	    	result = (*(f6object)fn)(arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
	    	break;
	        default:
	    	error("To many arguments.\n");
	    	break;
	        }
	        clear_arguments(topnode->arglst);
	    }
	    topnode->class = VALUE;
	    topnode->a.label = label_null;
	    topnode->data.val = result;
	    topnode->arglst = NULL;
	    assign_type_to_node(topnode);/* 7 - 4 - 89 */
	    label = label0;
	}
 break;

    case FUNCTION_COPIED:
	
	{ int tree_pt0 = tree_pt;
	  topnode->class = DUMMY;
	  eval_type(topnode);
	  eval_value(topnode);
	  tree_pt = tree_pt0;
	}
 break;

    case BLOCK:
	
	
	    for ( ; argsym!=NULL; argsym = argsym->next)
	    {
		if (argsym->class != VALUE) eval_value(argsym);
		clrshared(argsym->data.val);
	    }
	evalbl_value(topnode);
 break;

    case DUMMY:
	
	{ if (name_tab[name][0]=='.') /* topnode has class |MAP| */
	      for ( ; argsym!=NULL; argsym = argsym->next)
	      { if (argsym->class != VALUE) eval_type(argsym); }
	  else /* topnode has class other than map */
	      for ( ; argsym!=NULL; argsym = argsym->next)
	      { if (argsym->class != VALUE) eval_value(argsym); }
	  eval_type(topnode);
	  eval_value(topnode);
	}
 break;

    default: error("Illegal class encountered:%s.\n",
	code_class(topnode->class));
    }
    setshared(topnode->data.val);

    
    if (chunks > gccrit)
    {   if (verbose)
    	Printf("Begin garbage collection: %ld (line=%d file=%s)\n",
    	    (long)chunks,label->line,label->fname);
        if (topnode->data.val) mark (topnode->data.val);
        gc();
        if (verbose)
    	Printf("End garbage collection: %ld\n", (long)chunks);
        if (chunks > gccrit)
    	fatal("Garbage collection doesn't help.\n");
    #ifdef dos
    if (verbose)
      (void) printf("%ld bytes are available. \n",(long)farcoreleft());
    #endif
    }
}

static void clear_arguments(symblst argsym)
{
for ( ; argsym!=NULL; argsym = argsym->next) argsym->data.val = NULL;
}

