#include  "lie.h"


static boolean	ignore=true;

FILE* cur_in,* cur_out;
FILE		*monfile = NULL;
boolean		stop=false;
char		buffer[LINELENGTH];
char		label2[LABELLENGTH];
char		monfil[LABELLENGTH];
char		infofil[LABELLENGTH];
char		infoind[LABELLENGTH];
char		initfil[LABELLENGTH];
char		learnind[LABELLENGTH];
char		learnfil[LABELLENGTH];
char		titlefil[LABELLENGTH];
char		dirpath[LABELLENGTH];
char		promptlabel[LABELLENGTH];
char		pager[LABELLENGTH],editor[LABELLENGTH];
int		lmargin = 0;

labeltp label;
strtype fun_name;

char* input_fname;

int		block_depth = 0;
object		repair_obj = (object) NULL ;
symblst		topsym, top_definitions;
jmp_buf		envbuf;

boolean
  parsing=true, isargument=false, alloc_gc=true,
  redirected_input=false;
boolean
  am_monitor=false, prompt=true, runtime=false, verbose=false,
  lprint=true, gc_set=true, bigint_set=true;

object stop_loop, stop_fun;

strtype seq_name, if_name, dollar_name, break_name, return_name,
assign_name, assign_loc_name, block_name,setdefault_name;


void ignore_intr(void) { ignore=true; }
void allow_intr(void) { ignore=false; }

static void handle(int sig)
{
    if (ignore)
	return;
    while (cur_in != stdin) exit_input_file(false); /* pop input files */
    parsing=false;
    error("\nCalculation aborted\n");
}

static void init_loop(void)
{
    topsym ->next=(symblst) NULL;
    stop_loop=NULL;
    stop_fun=NULL;
    parsing=true;

    tree_pt=0;
    label_pt=0;
    block_depth=0;
    fun_name=0;
    label=label_null;
    if (gc_set)	gc();
    initpar();
    if (cur_out != stdout) cur_out=stdout;
    strcpy(promptlabel,PROMPT);
    fflush(cur_out);
    init_command(); /* this also prompts and calls |inputline| */
    registrate_cpu();
}

int main(int argc, char** argv)
{ if (no_terminal(stdin)) redirected_input=true;
  cur_in=stdin; cur_out=stdout; /* these cannot be initialised statically */

  
  { int nr=1;  argc--; infofil[0]='\0';
    if (argc > 0)
    {
        strcpy(initfil,argv[nr++]);
        argc--;
    }
    if (argc > 0)
    {
        strcpy(infofil,argv[nr++]);
        strcat(infofil,"/");
        argc--;
    }
    if (argc!=0)
    {
        printf("Illegal number of arguments.\n");
        exit(1);
    }
  }
  init();

    ignore_intr();
    (void) signal(SIGINT, handle);
    (void) setjmp(envbuf);

 /* Finished  signal handling and environment set  */

    while (init_loop(),init_loop_gettype(),yyparse(),!stop)
    {	if (feof(cur_in))
	  stop=exit_input_file(false); /* close file when fully read */
	if (runtime) print_runtime();
    }
    if (!redirected_input) Printf("end program\n");
    return 0;
}

