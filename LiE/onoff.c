#include  "lie.h"
#define local  static
#define STACK_LEN  100


typedef void (*switch_func_tp)(entry);
typedef struct
{ int runtime, verbose, bigints, gc, prompt, monitor, lex, degree, height,
   lprint, lmargin; } state_tp;
typedef struct
 { atp a; switch_func_tp fnc_on, fnc_off; int* state;} mode_tp;

state_tp state=
{OFFCODE, OFFCODE, ONCODE,ONCODE,ONCODE,OFFCODE,PLUS,OFFCODE,OFFCODE,ONCODE,OFFCODE};

cmpfn_tp cmpfn; 

local struct { int pt; state_tp s[STACK_LEN];} mstack; /* state stack */
local struct { int pt; object defaultgrp[STACK_LEN];} gstack;
 

local void prompt_on(entry unused) { prompt = true; }
local void prompt_off(entry unused) { prompt = false; }

local void lprint_on(entry unused) { lprint = true; }
local void lprint_off(entry unused) { lprint = false; }

local void gc_set_on(entry unused)  { gc_set = true; }
local void gc_set_off(entry unused) { gc_set = false; }

local void lmargin_on(entry argument)  { lmargin = argument; }
local void lmargin_off(entry unused) { lmargin = LMARGIN; }

local void monitor_on(entry unused)
{ if (!am_monitor)
   if ((monfile = fopen(monfil,"a"))==NULL)
     error("Cannot open %s.\n",monfil);
   else am_monitor=true;
}

local void monitor_off(entry unused)
{ if (am_monitor)
  { fprintf(monfile,"\n"); /* make sure last line has ended */
    fclose(monfile); monfile = NULL;
    am_monitor=false;
  }
}

local void runtime_on(entry unused) 
{ if (!runtime) registrate_cpu(), runtime=true; }

local void runtime_off(entry unused) 
{ if (runtime)  print_runtime(), runtime = false; }

local void verbose_on(entry unused)  { verbose=true; }

local void verbose_off(entry unused)  { verbose = false; }

local void unsort(object x) 
{ if (type_of(x)==MATRIX || type_of(x)==POLY) clrsorted(x); }
void unmark_sorted(void) 
{ for_all_objects(unsort); }

local void cmpfn_deg_on(entry argument)
{ cmpfn = argument==MINUS ?  deg_decr : (argument=PLUS,deg_incr) ;
state.degree=argument; state.lex=OFFCODE; state.height=OFFCODE;
  unmark_sorted();
}

local void cmpfn_lex_on(entry argument)
{ cmpfn = argument==MINUS ?  lex_decr : (argument=PLUS,lex_incr) ;
state.degree=OFFCODE; state.lex=argument; state.height=OFFCODE;
  unmark_sorted();
}

local void cmpfn_height_on(entry argument)
{ cmpfn = argument==MINUS ? height_decr : (argument=PLUS,height_incr) ;
state.degree=OFFCODE; state.lex=OFFCODE; state.height=argument;
  unmark_sorted();
}

local void rechain_symbols(symbrec s[],int n)
{ int i; 
  for (i=n-1;i>=0;i-=s[i].formal)
  { s[i].next = symbol_tab[s[i].a.name];
    symbol_tab[s[i].a.name]=&s[i];
  }
}

local void rebuild_symbol_tab(void)
{ static boolean first = true; 
    if (!first) clear_symbol_tab(); else first = false;
  rechain_symbols((symblst)static3,nstatic3);
  rechain_symbols((symblst)static4,nstatic4);
  rechain_symbols((symblst)static5,nstatic5);
  rechain_symbols((symblst)static2,nstatic2);
  rechain_symbols((symblst)static1,nstatic1);
  if (bigint_set) rechain_symbols((symblst)static7,nstatic7);
  else rechain_symbols((symblst)static6,nstatic6);
}

local void bigint_set_on(entry unused)
{ static boolean start = true;
  if (!bigint_set || start)
  { bigint_set = true;
    rebuild_symbol_tab();
    start = false;
  }
}

local void bigint_set_off(entry unused)
{ if (bigint_set)
  { bigint_set = false;
    rebuild_symbol_tab();
  }
}

local mode_tp mode[] =
{
 {{"runtime"},runtime_on, runtime_off, &state.runtime},
 {{"verbose"},verbose_on, verbose_off, &state.verbose},
 {{"bigints"},bigint_set_on, bigint_set_off, &state.bigints},
 {{"gc"}, gc_set_on, gc_set_off, &state.gc},
 {{"prompt"},prompt_on,prompt_off, &state.prompt},
 {{"monitor"},monitor_on,monitor_off, &state.monitor},
 {{"lex"},cmpfn_lex_on, (switch_func_tp) NULL, &state.lex},
 {{"degree"},cmpfn_deg_on, (switch_func_tp) NULL, &state.degree},
 {{"height"},cmpfn_height_on, (switch_func_tp) NULL, &state.height},
 {{"lprint"},lprint_on,lprint_off, &state.lprint},
 {{"lmargin"},lmargin_on,lmargin_off, &state.lmargin}
};

boolean set_on(entry val, strtype name)
{ int i = 0; size_t n = sizeof(mode)/sizeof(mode[0]);
  for (i=0; i<n; ++i) if (mode[i].a.name == name) break;
  if (i==n) return true; /* not found */
  *mode[i].state = val;
  if (mode[i].fnc_on!=NULL) mode[i].fnc_on(val);
  else error("Cannot switch %s on.\n",name_tab[name]);
  return false;
}

boolean set_off(entry val, strtype name)
{ int i = 0; size_t n = sizeof(mode)/sizeof(mode[0]);
  for (i=0; i<n; ++i) if (mode[i].a.name == name) break;
  if (i==n) return true; /* not found */
  *mode[i].state = val;
  if (mode[i].fnc_off!=NULL) mode[i].fnc_off(val);
  else error("Cannot switch %s off.\n",name_tab[name]);
  return false;
}

void init_state(void)
{ int i; size_t n = sizeof(mode)/sizeof(mode[0]);
  for (i=0; i<n; ++i)
  { mode[i].a.name = match(mode[i].a.init_string,false);
    if (*mode[i].state==OFFCODE)
    { if (mode[i].fnc_off!=NULL) mode[i].fnc_off(OFFCODE); }
    else { if (mode[i].fnc_on!=NULL) mode[i].fnc_on(*mode[i].state); }
  }
  mstack.pt = 0;
  gstack.pt = 0;
}

void print_state(void)
{ int i; size_t n = sizeof mode/sizeof *mode;
  for (i=0; i<n; ++i)
  { if (*mode[i].state == OFFCODE) Printf("off\t");
    else if (*mode[i].state == ONCODE) Printf("on\t");
    else if (*mode[i].state == PLUS) Printf("on\t+\t");
    else if (*mode[i].state == MINUS) Printf("on\t-\t");
    else Printf("on\t%d\t",*mode[i].state);
    Printf("%s\n",name_tab[mode[i].a.name]);
  }
}

void save_state(void)
{ if (mstack.pt >= STACK_LEN) error("Not possible to save more options.\n");
  mstack.s[mstack.pt++]=state;
  gstack.defaultgrp[gstack.pt++]=defaultgrp;
}

local void set_on_off(entry val,strtype name)
{ if (val == OFFCODE) set_off(val, name); else set_on(val,name); }

void restore_state(void)
{ int i; size_t n=sizeof mode/sizeof *mode;
  if (mstack.pt==0) error("There are no saved options.\n");
  state=mstack.s[--mstack.pt];
  for (i=0; i<n; ++i) set_on_off(*mode[i].state,mode[i].a.name);
  defaultgrp=gstack.defaultgrp[--gstack.pt];
}


void mark_defaultgrp_stack(void)
{ int i; for (i=0; i<gstack.pt; ++i) mark(gstack.defaultgrp[i]); }


