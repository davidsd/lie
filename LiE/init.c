#include  "lie.h"
#define INITFIL  "initfile"


object defaultgrp = NULL, grp;
bigint* one,* null,* minus_one;
intcel* bool_false,* bool_true;
objcel the_nothing,* nothing=&the_nothing;



static poly* create_basic_poly(poly **nulls, index r)
{
    int i;
    static boolean first = true;
    poly *result = (poly*) NULL;
    if (first)
      { for (i=0; i<NPOLY; i++) nulls[i]= (poly*) NULL; first = false; }
    if (r<NPOLY && nulls[r]!=NULL) return nulls[r];
    result = mkpoly(1,r); setsorted(result);
    for (i=0; i<r; i++) result->elm[0][i] = 0;
    if (r < NPOLY) { nulls[r]=result; setlonglife(result); }
    return result;
}

poly *poly_one(index r)
{
    static poly *ones[NPOLY];
    poly* result=create_basic_poly(ones, r);
    result->coef[0] = one;
    return result;
}

poly *poly_null(index r)
{
    static poly *zeros[NPOLY];
    poly* result= create_basic_poly(zeros, r);
    result->coef[0] = null;
    return result;
}

static void makelink(Symblst a, int n)
{ int i; symblst new_a=(symblst)a; /* overlapping in memory */
  for (i=0; i<n; ++i)
  { if (a[i].name!=NULL) new_a[i].a.name=match(a[i].name,false);
    new_a[i].data.f=a[i].f;
    if (a[i].arglst!=0) new_a[i].arglst=&new_a[i+a[i].arglst];
    else new_a[i].arglst=NULL;
    if (a[i].next!=0) new_a[i].next=&new_a[i+a[i].next];
    else new_a[i].next=NULL;
    }
}

static boolean eqrow_null(entry *a,index n)
{
    index i=0;
    while (i<n && *a++ == 0) i++;
    return i==n;
}

boolean Mat_null(object a)
{
    index    i=0,
    m=a->m.nrows,
    n=a->m.ncols;
    while (i < m && eqrow_null(*(a->m.elm+i),n))
	i++;
    if (i == m) return true;
    return false;
}

boolean Vec_null(object a)
{
    index	      i, n;
    n=a->v.ncomp;
    i=0;
    while (i < n && *(a->v.compon + i) == 0L)
	i++;
    if (i == n) return true;
    return false;
}

boolean Pol_null(object a)
{
    index nrows=a->pl.nrows;
    index i=0;
    while (i <nrows && !a->pl.coef[i]->size) i++;
    if (i == nrows) return true;
    return false;
}

void init(void)
{ int ch;
  info_depth.n = 0;
  
  { if (initfil[0]=='\0') strcpy(initfil, INITFIL);
    strcpy(dirpath, infofil);
    strcpy(learnfil, infofil);
    strcpy(titlefil, infofil);
    strcat(infofil,"INFO");
    strcat(learnfil,"LEARN");
    strcat(titlefil,"title");
  strcpy(infoind, infofil);
    strcpy(learnind, learnfil);
    strcat(infoind,".ind");
    strcat(learnind,".ind");
    strcpy(monfil, "monfile");
  }
  
  { init_scanner(); /* must store keywords first */
    seq_name=match(".sequence",false);
    if_name=match(".if",false);
    assign_name=match(".assign",false);
    assign_loc_name=match(".assign_loc",false);
    dollar_name=match("$",false);
    break_name=match("_break",false);
    block_name=match("$block",false);
    return_name=match("_return",false);
    setdefault_name=match("_setdefault",false);
    input_fname=name_tab[match("stdin",false)];
  }
  
  { makelink(static1, nstatic1);
    makelink(static2, nstatic2);
    makelink(static3, nstatic3);
    makelink(static4, nstatic4);
    makelink(static5, nstatic5);
    makelink(static6, nstatic6);
    makelink(static7, nstatic7);
  }
  
  { maxptrs = MAXPTRS_DFLT; initmem();
    maxnodes = MAXNODES_DFLT; inittree();
  }
  
  { one=entry2bigint(1);	      setlonglife(one);
    null=entry2bigint(0);	      setlonglife(null);
    minus_one=entry2bigint(-1); setlonglife(minus_one);
  bool_false=mkintcel(0);     setlonglife(bool_false);
    bool_true=mkintcel(1);      setlonglife(bool_true);
    nothing->any.type=VOID;     setlonglife(nothing);
  }
  
  { top_definitions=creatsym(match(".top_definitions",false));
    add_user_defined();
    topsym=creatsym(match(".topsym",false));
    topsym->formal=match(".topsym_formal",false);
  }
  init_state();
  initialize_readline();
  
  { if (!redirected_input)
    { printf("\nLiE version 2.2.2 created on %s\n" 
        "Authors: Arjeh M. Cohen, Marc van Leeuwen, Bert Lisser.\n", date);
      { FILE* f= fopen(titlefil,"r");
        if (f==NULL) printf("Purpose: development CWI\n\n");
        else { while ((ch=getc(f)) != EOF) putchar(ch); fclose(f); }
      }
      printf("\ntype '?help' for help information\n" 
  	   "type '?' for a list of help entries.\n");
    }
  }
  if (enter_input_file(initfil)) ++lineno;
  { char* p=getenv("EDITOR"); 
      strcpy(editor, p==NULL ? DEFAULT_EDITOR : p);
    p=getenv("PAGER");
      strcpy(pager, p==NULL ? DEFAULT_PAGER : p);
   }
   strcpy(promptlabel,PROMPT);
}


