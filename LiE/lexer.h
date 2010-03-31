

 extern int lineno;

 extern char* name_tab[];

 extern symblst symbol_tab[];

enum{ empty_str=0 };

extern char* keyword[];

 strtype match(char* str, boolean copy);

short creatmapname(char *a);
void clear_symbol_tab(void);

void init_scanner(void); void init_command(void); void clear_input(void);

int yylex(void);


