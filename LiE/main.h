

void allow_intr(void);
void ignore_intr(void);

extern FILE* cur_in,* cur_out;
extern FILE		*monfile;
extern char		buffer[];
extern char		label2[],monfil[],
infofil[], infoind[], initfil[], learnind[], learnfil[], titlefil[], dirpath[],
promptlabel[], pager[],editor[];
extern int	       lmargin;
extern boolean	       stop;

extern labeltp label;
extern strtype fun_name;

extern char* input_fname;

extern int		block_depth;
extern object		repair_obj;
extern symblst		topsym, top_definitions;
extern jmp_buf		envbuf;

extern boolean
  parsing, isargument, alloc_gc, redirected_input;
extern boolean
  am_monitor, prompt, runtime, verbose, lprint, gc_set, bigint_set;

extern object stop_loop, stop_fun;

extern strtype seq_name, if_name, dollar_name, break_name, return_name,
assign_name, assign_loc_name, block_name,setdefault_name;

extern int yyparse(void);
#ifdef dos
extern long   farcoreleft(void);
#endif
extern void	add_user_defined();


