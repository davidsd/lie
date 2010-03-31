%start command

%{
#include "lie.h"
%}

%union {
	  short sub;
	  long val;
	  bigint* num;
	  symblst sym;
	  char* text;
	}

%token	IF 1 THEN 2 ELSE 3 FI 4
	FOR 5 TO 6 DOWNTO 7 IN 8 ROW 9 WHILE 10 DO 11 OD 12
	LOC 13 RETURN 14 BREAK 15
	MAKE 16
%token <sub> APPLY 17
	ON 20 OFF 21 SETDEFAULT 22 SAVESTATE 23 RESTORESTATE 24
%token <sub> TYPEID 25 HELP 31
%token  WHATTYPE 32 EDIT 33  READ 34 WRITE 35 MONFIL 36
	LEARN 37 EXEC 38 QUIT 39
%token <sub> LISTCOMMAND 41
%token	MAXNODES 44 MAXOBJECTS 45

%token WRITEHELP 18 APPENDHELP 19

%token <val> INT 26
%token <num> NUMBER 27
%token <sub> GROUPID 28 IDENT 29 DOLLAR 30
%token <sym> TEXT 40
%token <text> ANY_STRING 42
%token ENTER 43

%token	SEMICOLON 46 COMMA 47 BAR 48 COLON 49
%token  LPAR 50 RPAR 51 LBRACE 52 RBRACE 53 LBRACK 54 RBRACK 55

%token OR 56 AND 57 NOT 58 TIMES 59 POWER 60 X 61 BECOMES 62 PLUSAB 63
%token <sub> RELOP 64 ADDOP 65 DIVOP 66

%type <sym>	block series statlist statement assignment expression
		logical_expr op1 op2 op3 arithmetic_expr op6 op7 op8
		monadic_expr secondary selection primary
		var simple_group group formals joined_formals varlist
		list_option list
%type <sub> on
%type <text> string_opt

%{
#include "parseaux.c"
%}

%% /*beginning of rules section */

command : ENTER
	| series ENTER
	  { parsing = false; eval_type($1); /* global_pt=0; */
	    $1->type=ARGTYPE; eval_value($1);
	    if ($1->data.val)
	    { addsym(match("$",false),(symblst) NULL, $1, VALUE);
	      vid_print($1->data.val);
	    }
	  }
	| function_definition ENTER
	| READ string_opt ENTER
	  { char *filename = $2==NULL ? initfil : $2;
	    boolean istty = !no_terminal(cur_in);
	      /* not already reading from a file */
	    parsing=false;
	    if (!enter_input_file(filename))
	    { Printf("Not possible to read file %s.\n",filename);
	      YYERROR;
	    }
	    if (istty) strcpy(initfil, filename);
	  } 
	| EDIT string_opt ENTER
	  { char *filename = $2==NULL ? initfil : $2;
	    parsing=false;
	    invoke_prog(editor,filename);
	    if (!enter_input_file(filename))
              Printf("File %s could not be read after editing.\n",filename);
            else strcpy(initfil,filename); /* set default for further reads */
	  }
	| WRITE ANY_STRING ENTER
	  { parsing=false;
	    listvars(top_definitions,$2); listfuns(top_definitions,$2);
	  }
	| MONFIL string_opt ENTER
	  { if ($2==NULL) Printf("%s\n",monfil);
	    else if (strlen($2)>=LABELLENGTH)
	      error("File name %s too long.\n",$2);
	    else strcpy(monfil,$2); 
	  }
	| MAXOBJECTS ENTER
	  { Printf("Maximum number of objects: %ld\n",(long)maxptrs); }
	| MAXOBJECTS INT ENTER
	  { parsing=false; newmem($2); }
	| MAXNODES ENTER
	  { Printf("Maximum number of nodes: %ld\n",(long)maxnodes); }
	| MAXNODES INT ENTER
	  { parsing=false; maxnodes = $2; newtree(); }
	| LISTCOMMAND ENTER
	  { if ($1==2) listops(top_definitions,NULL,true);
	    else ($1==0 ? listvars :listfuns)(top_definitions,NULL);
	  }
	| HELP string_opt WRITEHELP ANY_STRING ENTER
	  { char* s = $2!=NULL ? $2 : $1 ? dir : help;
	    parsing=false; write_key(s,$4,"w");
	  }
	| HELP string_opt APPENDHELP ANY_STRING ENTER
	  { char* s = $2!=NULL ? $2 : $1 ? dir : help;
	    parsing=false; write_key(s,$4,"a");
	  }
	| HELP string_opt ENTER
	  { char* s = $2!=NULL ? $2 : $1 ? dir : help;
	    parsing=false; printexpr(s,TO_LOOK,true);
	  }
	| LEARN string_opt ENTER
	  { parsing=false;
	    if (!Learn($2==NULL ? "index" : $2))
	      { Printf("Info about %s is not available.\n",$2); YYERROR; }
	  }
	| COLON ANY_STRING ENTER
	  { parsing=false; invoke_prog($2,""); }
	| QUIT ENTER
	  { if (cur_in != stdin) {fclose(cur_in);cur_in=stdin; } 
	    stop = true;
	  }
	| WHATTYPE arithmetic_expr ENTER
	  { parsing=false;
	    eval_type($2);
	    Printf("%10s ",code_class($2->class));
	    Printf("%10s \n",code_type($2->type));
	  }
	| EXEC ANY_STRING ENTER
	  { entry pos = exec_learn($2);
	    if (pos >= 0)
	    { cur_in=fopen(learnfil,"r");
	      if (!cur_in)
		{ Printf("%s not found.\n",learnfil); cur_in=stdin;YYERROR; }
	      if (fseek(cur_in,pos,0)<0)
	      { Printf("Unfortunate seek %s.\n",learnfil); cur_in=stdin;YYERROR;
	      }
	    }
	    else
	    { Printf("Info about %s is not available.\n",$2);
	      cur_in = stdin; YYERROR;
	    }
	    lineno = 0;
	  }
	;

block	: LBRACE series RBRACE { $$=B(block_name,$2); }
	| LBRACE series RBRACE LPAR list RPAR
	  { $$=B(block_name,$2); $$->arglst = $5; add_dollar_names($5);
	    --tail_pt;
	  }
	;

series	: statlist { $$= addnode(1,seq_name,$1); } /* wrap up a series */
	;

statlist: statement
	| statement SEMICOLON
	| statement SEMICOLON statlist	{ $$=$1; $$->next=$3; }
	| SEMICOLON statlist		{ $$ = $2; }
	;

statement
	: assignment	
	| expression
	| RETURN expression
	  { $$=addnode(1,match("_return",false),$2); }
	| BREAK expression
	  { $$=addnode(1,match("_break",false),$2); }
	| RETURN 
	  { $$=addnode(0,match("_return",false)); }
	| BREAK 
	  { $$=addnode(0,match("_break",false)); }
	| SETDEFAULT
	  { $$=addnode(0,match("_setdefault",false)); }
	| SETDEFAULT expression
	  { $$=addnode(1,match("_setdefault",false),$2); }
	| on IDENT
	  { $$=addnode(2,match("_on",false)
		        ,addintnode($1),addtekstnode(name_tab[$2])); }
	| OFF IDENT
	  { $$=addnode(2,match("_off",false)
		        ,addintnode(OFFCODE),addtekstnode(name_tab[$2])); }
	| on
	  { $$=addnode(0,match("_on",false)); }
	| OFF
	  { $$=addnode(0,match("_off",false)); }
	| SAVESTATE
	  { $$=addnode(0,match("save_options",false)); }
	| RESTORESTATE
	  { $$=addnode(0,match("restore_options",false)); }
	;

on	: ON		{ $$ = ONCODE; }
	| ON INT
	  { if ($2 < 0)
	      error("Argument %ld to 'on' is negative.\n",(long)($2));
	    $$ = $2;
	  }
	| ON ADDOP	{ $$ =  $2==0 ? PLUS : MINUS; }
	;

assignment
	: IDENT BECOMES expression 
	  { $$ = addnode(2,match(".assign",false),addnamenode($1),$3); }
	| LOC IDENT BECOMES expression 
	  { $$ = addnode(2,match(".assign_loc",false),addnamenode($2),$4); }
	| IDENT PLUSAB expression 
	  { $$ = addnode(2,match(".+=",false),addnamenode($1),$3);
	    $$->arglst->type = UNKNOWN;
	  }
	| selection BECOMES arithmetic_expr 
	  { /* a[i]=n	    = .update(a,n,i)) */
	    symblst lst = $1->arglst;
	    check_variable($1->arglst);
	    if ($1->a.label->name == match(Select,false))
		$1->a.label->name = match(update,false);
	    else 
		$1->a.label->name = match(vecupdate,false);
	    /* insert n */
	    $3->next = lst->next; /* Points to indexes */
	    lst->next = $3;
	    $$ = $1;
	   }
	| selection PLUSAB arithmetic_expr 
	  { symblst lst = $1->arglst;
	    check_variable($1->arglst);
	    if ($1->a.label->name == match(Select,false))
		$1->a.label->name = match(plus_and_becomes,false);
	    else error("error with operator `+='.\n");
	    $3->next = lst->next; /* points to indexes */
	    lst->next = $3;
	    $$ = $1;
	   }
	;

expression
	: logical_expr | arithmetic_expr 
	;

logical_expr
	: expression OR op1
	  { $$=addnode(2,match("||",false),$1,$3); }
	| expression OR arithmetic_expr
	  { $$=addnode(2,match("||",false),$1,$3); }
	| op1
	;

op1	: op1 AND op2
	  { $$=addnode(2,match("&&",false),$1,$3); }
	| op1 AND arithmetic_expr
	  { $$=addnode(2,match("&&",false),$1,$3); }
	| arithmetic_expr AND op2
	  { $$=addnode(2,match("&&",false),$1,$3); }
	| arithmetic_expr AND arithmetic_expr
	  { $$=addnode(2,match("&&",false),$1,$3); }
	| op2
	;

op2	: NOT op2
	  { $$=addnode(1,match("!",false),$2); }
	| NOT arithmetic_expr
	  { $$=addnode(1,match("!",false),$2); }
	| op3
	;

op3	: arithmetic_expr RELOP arithmetic_expr	
	  { static char s[][3]={ "==", "!=", "<", ">", "<=", ">=" };
	    $$=addnode(2,match(s[$2],false),$1,$3);
	  }
	| LPAR logical_expr RPAR
	  { $$=$2; }
	;

arithmetic_expr
	: arithmetic_expr ADDOP op6
	  { $$=addnode(2,match($2==0 ? "+" : "-",false),$1,$3); }
	| op6
	;

op6	: op6 TIMES op7
	  { $$=addnode(2,match("*",false),$1,$3); }
	| op6 DIVOP op7
	  { $$=addnode(2,match($2==0 ? "/" : "%",false),$1,$3); }
	| op7
	;

op7	: op7 POWER op8
	  { $$=addnode(2,match("^",false),$1,$3); }
	| op8
	;

op8	: monadic_expr X monadic_expr
	  { $$=addnode(2,match("X",false),$1,$3); }
	| monadic_expr
	;

monadic_expr
	: ADDOP monadic_expr
	  { if ($1==1) $$=addnode(1,match("-",false),$2); else $$=$2; }
	| TIMES monadic_expr
	  { $$=addnode(1,match("*",false),$2); }
	| X monadic_expr
	  { $$=addnode(1,match("X",false),$2); } 
	| secondary
	;

secondary
	: selection | primary ;

selection
	: secondary LBRACK list RBRACK
	  { symblst  lst = $3; int i = 0;
	    while (lst!=NULL) { lst=lst->next; ++i; }
	    if (i>2)
	    { parsing=false;
	      error("Indexing with %d subscripts; at most 2 are allowed.\n",i);
	    }
	    $$=addnode(2,match(Select,false),$1,$3);
	    --tail_pt;
	  }
	| secondary BAR primary
	  { $$=addnode(2,match("|",false),$1,$3); }
	;

primary	: var
	| NUMBER	{ $$=addbigintnode($1); }
	| TEXT
	| group 
	| IDENT LPAR RPAR
	  { $$=addnode(0,$1); }
	| IDENT LPAR list RPAR
	  { $$=addnode(1,$1,$3); --tail_pt; }
	| MAKE LPAR var COMMA arithmetic_expr  RPAR
	  { $$=addnode(2,match(".make",false),$3,$5);
	    $3->arglst=addintnode(0); 
	  }
	| MAKE LPAR var COMMA arithmetic_expr COMMA arithmetic_expr RPAR
	  { $$=addnode(3,match(".make",false),$3,$5,$7);
	    $3->arglst=addintnode(0); $3->arglst->next=addintnode(0);
	  }
	| APPLY LPAR var COMMA arithmetic_expr COMMA arithmetic_expr RPAR
	  { char* s=$1==0 ? ".iapply" : $1==1 ? ".vapply" : ".mapply";
	    $$=addnode(3,match(s,false),$3,$5,$7); $3->arglst=copytree($7);
	  }
	| LBRACK list_option RBRACK
	  { $$=addnode(1,match(".strucval",false),$2); }
	| LPAR arithmetic_expr RPAR
	  { $$=$2; }
	| block 
	| IF expression THEN series ELSE series FI
	  { $$=addnode(3,match(".if",false)
		        ,$2,B(block_name,$4),B(block_name,$6)); }
	| IF expression THEN series FI
	  { $$=addnode(2,match(".if",false),$2,B(block_name,$4)); }
	| WHILE expression DO series OD
	  { $$=addwhile(match(".whilefun",false),$2,$4); }
	| FOR IDENT BECOMES arithmetic_expr TO arithmetic_expr DO series OD
	  { $$=addfor($2, $4, $6, $8, match(".forint",false)); }
	| FOR IDENT BECOMES arithmetic_expr DOWNTO arithmetic_expr DO series OD
	  { $$=addfor($2, $4, $6, $8, match(".downforint",false)); }
	| FOR IDENT IN arithmetic_expr DO series OD
	  { $$=addfor($2, addintnode(0), $4, $6, match(".forvec",false)); }
	| FOR IDENT ROW arithmetic_expr DO series OD
	  { $$=addfor($2, addvecnode(0), $4, $6, match(".format",false)); }
	;

var	: IDENT		{ $$=addnode(0,$1); }
	| DOLLAR	{ $$=addnode(0,$1); }
	;

group	: simple_group
	| group simple_group
	  { $$=addnode(2,match("*",false),$1,$2); }
	;

simple_group
	: GROUPID INT
	  { if (wronggroup($1,$2))
	      { Printf("Non-existent Lie group requested.\n"); YYERROR; }
	    $$=addgroupnode($1,(index)$2);
	  }
	;

function_definition
	: IDENT LPAR formals RPAR BECOMES series 
	   { addsym($1,$3, $6,FUNCTION); }
	| IDENT LPAR RPAR BECOMES series 
	  { addsym($1,NULL, $5,FUNCTION ); }
	| IDENT LPAR formals RPAR LBRACE series RBRACE
	   { addsym($1,$3, $6,FUNCTION); }
	| IDENT LPAR RPAR LBRACE series RBRACE 
	   { addsym($1,NULL,$5,FUNCTION ); }
	;

formals : joined_formals
	| joined_formals SEMICOLON formals
	  { symblst* p=&$1; while (*p!=NULL) p=&(*p)->next; *p=$3; $$=$1; }
	;

joined_formals
	: TYPEID varlist
	  { assigntype(type_code(keyword[TYPEID+$1]),$2); $$=$2; }
	;

varlist : var			{ $$=$1; $$->next=NULL; }
	| var COMMA varlist	{ $$=$1; $1->next=$3; }
	;

list_option
	:
	  { $$=NULL; }
	| list
	  { --tail_pt; }
	;

list	: expression
	  { ++tail_pt;
	    if (tail_pt >= TAIL_LEN) 
	      error ("Nestings of lists too deep: %d.\n",tail_pt);
	    $$=$1; $$->next=NULL; tail[tail_pt]=$$;
	   }
	| list COMMA expression
	  { $$=$1; tail[tail_pt]->next=$3; tail[tail_pt]=$3; }
	;

string_opt
	:	    { $$=NULL; }
	| ANY_STRING
	;

