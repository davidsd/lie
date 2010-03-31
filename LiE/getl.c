/* this file defines the input interface to the lexical scanner */

#include "lie.h"
extern boolean stop;

 /* ignore following stuff if not using readline */

#ifdef preprocessor

/* Instead of #including files we simply state the few declarations needed.
   This was needed because on some systems the included files would contain
   stuff that conflicts with that in "lie.h" (notably the typedef of |index|).
*/

typedef int Function (void);
extern char* readline (char*);
extern void add_history (char*);
extern int rl_insert (void);
extern int rl_bind_key(int, Function*);

/* A static variable for holding the line. */
static char* line_read = NULL;

/* prompt, read a string, and set |line_read| to it, or to |NULL| on |EOF| */
void fetch_line (char *prompt_string)
{ if (redirected_input || cur_in!=stdin) return;
  if (line_read != NULL) { free(line_read); line_read = NULL; }
  if (!prompt) prompt_string="";
  else if (am_monitor) fprintf(monfile,"%s",prompt_string);
  line_read = readline(prompt_string); /* Get a line from the user. */
  if (line_read!=NULL && line_read[0]!='\0') /* if line is not empty */
    add_history(line_read);                  /* add it to the history */
}

/* now get a character from |stdin|, in fact from the |line_read| buffer */

int fetch_char(void)
{ static int i=0; int c;
  if (redirected_input || cur_in!=stdin) c=getc(cur_in);
  else if (line_read==NULL) c=EOF;
  else if ((c=line_read[i++])=='\0') { i=0; c='\n'; }
  if (am_monitor && c!=EOF) putc(c,monfile);
  return c;
}

void initialize_readline(void) { rl_bind_key('\t',rl_insert); }

#else

void fetch_line(char *prompt_string)
{ if(!redirected_input &&  cur_in==stdin && prompt)
  { Printf("%s",prompt_string);
    if (am_monitor) fprintf(monfile,"%s",prompt_string);
  }
}

int fetch_char(void)
{ int c=getc(cur_in); if (am_monitor && c!=EOF) putc(c,monfile); return c; }

void initialize_readline(void){}

#endif








