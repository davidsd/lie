#include "lie.h"
#include <time.h>

int Printf(char *format, ...)
{ va_list ap; int nc;
  va_start(ap, format); nc=vfprintf(cur_out, format, ap);
  if (am_monitor) vfprintf(monfile, format, ap);
  va_end(ap); return nc;
}

void err_Printf(char *format, ...)
{ va_list ap;
  va_start(ap,format); vfprintf(stderr, format, ap);
  if (am_monitor) vfprintf(monfile, format, ap);
  va_end(ap);
}
/*
void error(char *format, ...)
{
    symblst	    list;
    extern jmp_buf  envbuf;
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    if (am_monitor) vfprintf(monfile, format, ap);
    if (label->name)
    {
	 boolean printin = (label->name != seq_name);
	 if (printin)
	    Printf("(in %s",name_tab[label->name]);
	 if (no_terminal(cur_in) || strcmp(label->fname, "stdin")!=0) {
	    if (!printin) Printf("(");
	    Printf(" at line %d of file %s)\n",label->line, label->fname);
	 }
	 else
	    if (printin) Printf(")\n");
    }
    if (fun_name)
	Printf("[in function %s ]\n", name_tab[fun_name]);
    for (list = top_definitions; list!=NULL; list = list->next)
	/* Recover symbol table * / 
    {	if (list->class == FUNCTION_COPIED) list->class = FUNCTION;
	if (list->next && list->next->class == DUMMY)
          list->next = list->next->next;	/* Remove sym * /
    }
    if (repair_obj) {
	setshared(repair_obj); 
	repair_obj = (object) NULL;
    }
    if (cur_in==stdin)
      clear_input();
    else
      do exit_input_file(parsing); while (cur_in!=stdin); /* pop input files * /
    longjmp(envbuf, -1);
}
*/
void fatal(char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    (void) fprintf(stderr, "\n*** FATAL ERROR ***\n");
    vfprintf(stderr, format, ap);
    abort();
}

void monprintf(char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    if (am_monitor)
    vfprintf(monfile, format, ap);
}

void monitor_cmd(char* file_arg)
{
    if (am_monitor)
	error("monitoring is already being done\n");
    monfile = fopen(file_arg, "w");
    if (monfile == NULL)
	error("can't open %s\n", file_arg);
}

void endmon(void)
{
    if (!am_monitor)
	error("not monitoring at this moment\n");
    monprintf("\n");
    if (fclose(monfile))
	error("error closing monitor file\n");
}

void invoke_prog(char* prog, char* args)
{ size_t l=strlen(prog); char* buf=allocmem(l+strlen(args)+2);
  strcpy(buf,prog); buf[l]=' '; strcpy(&buf[l+1],args);
  system(buf); freem(buf);
}


typedef struct {
  char *name;
  FILE *fp;
  int line, tree_pt;
} read_stack_tp;

#define READ_STACK_LEN 10 /* Number of open read files */
static read_stack_tp read_stack[READ_STACK_LEN];
static int read_stack_pt=0;

/* Calling |enter_input_file| will attempt to open the given file for reading
   commands, returning |true| on success, and returning |false| after undoing
   all preparations otherwise.
*/
   
boolean enter_input_file(char* name)
{ if (read_stack_pt >= READ_STACK_LEN) return false;
  read_stack[read_stack_pt].fp=cur_in;
  read_stack[read_stack_pt].line=lineno;
  read_stack[read_stack_pt].name=input_fname;
  read_stack[read_stack_pt].tree_pt=tree_pt;
  if ((cur_in=fopen(name,"r"))!=NULL)
  { ++read_stack_pt; lineno=1;
    input_fname=name_tab[match(name,true)]; /* make a secure copy */
    return true;
  }
  cur_in=read_stack[read_stack_pt].fp; /* restore */
  return false;
}

boolean exit_input_file(parse_error)
{ fclose(cur_in);
  if (read_stack_pt < 0) fatal("Exit_input_file.\n");
  if (read_stack_pt == 0) {
      if (parse_error) fatal("No recovery possible.\n");
      return true;
  }
  --read_stack_pt;
  cur_in=read_stack[read_stack_pt].fp;
  lineno=read_stack[read_stack_pt].line;
  input_fname=read_stack[read_stack_pt].name;
  if (parse_error) {
      tree_pt=read_stack[tree_pt].tree_pt;
  }
  return false;
}

/* Timing of computations */

static clock_t last_time;

void registrate_cpu(void) { last_time=clock(); }

void print_runtime(void)
{ long diff=clock()-last_time;
  Printf("\nComputation time %ld.%02ld sec.\n"
	, diff/CLOCKS_PER_SEC, (100*diff/CLOCKS_PER_SEC)%100);
}

