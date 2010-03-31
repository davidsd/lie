#define TAIL_LEN   10

extern YYSTYPE yylval; /* last parsing value set by lexical analyser */
extern int yychar; /* lookahead token code */


int tail_pt;

static char Select[] = "_select";
static char update[] = ".update";
static char vecupdate[] = ".vecupdate";
static char plus_and_becomes[] = ".add_and_assign";
static char help[] = "help";
static char dir[] = "index";
static symblst tail[TAIL_LEN];


char* last_token(short code)
{ if (code<SEMICOLON || code>RELOP)
    switch(code)
    { static char s[12];
    default: return keyword[code];
    case APPLY: case TYPEID: case LISTCOMMAND:
      return keyword[code+yylval.sub];
    case HELP: return "?";
    case QUIT: return "@";
    case WRITEHELP: return ">";
    case APPENDHELP: return ">>";
    case INT: sprintf(s,"%ld",yylval.val); return s;
    case NUMBER: return bigint2str(yylval.num);
    case GROUPID: sprintf(s,"%c",yylval.sub); return s;
    case IDENT: case DOLLAR: return name_tab[yylval.sub];
    case TEXT:
      { char* t=yylval.sym->data.val->t.string; size_t l=strlen(t);
	if (l<10) sprintf(s,"\"%s\"",t);  else sprintf(s,"\"%.6s...\"",t);
	return s;
      }
    case ANY_STRING: return yylval.text;
    case ENTER: return "<newline>";
    case ADDOP: return yylval.sub==0 ? "+" : "-";
    case DIVOP: return yylval.sub==0 ? "/" : "%";
  }
  else
  { static char* s[]={ ";", ",", "|", ":", "(", ")", "{", "}", "[", "]",
	      "||", "&&", "!", "*", "^", "X", "=", "+=" , 
	      "==", "!=", "<", ">", "<=", ">=" };
    return s[code<RELOP ?code-SEMICOLON : code-SEMICOLON+yylval.sub];
  }
}

static void yyerror(char* s)
{ if (no_terminal(cur_in))
    error("%s at token `%s' in line %ld of file \"%s\".\n"
	 ,s,last_token(yychar),(long)lineno,input_fname);
  else error("%s at token `%s'.\n",s,last_token(yychar));
}

static symblst B(strtype name, symblst progsym)
{
    symblst blocksym = addnode(0, name);
    blocksym->class = BLOCK;
    blocksym->data.expr = progsym;
    return blocksym;
}

void initpar(void)
{
    int i;
    for (i=0;i<tail_pt;i++) tail[i] = (symblst) NULL;
    tail_pt = -1;
}

static symblst addfor
( strtype name, symblst startsym, symblst endsym, symblst progsym
, strtype fname)
{
  static char endname[] = "$2";
  symblst result =
    B(block_name,
      addnode(2, fname
	       , addnode(0,match(endname,false))
	       , B(match("$block",false), progsym)
	     )
     );
    startsym->formal = name;
    endsym->formal = match(endname,false);
    result->arglst = startsym;
    result->arglst->next = endsym;
    return result;
}

static symblst addwhile(strtype fname,symblst condition,symblst prog)
{
    return addnode(2,fname,B(block_name,condition),B(block_name,prog));
}

static void check_variable(symblst s)
{
    char *name;
    name  = name_tab[s->a.label->name];
    if (s->arglst!=NULL)
       error("Cannot assign to subscripted compound expression\n");
    if (strlen(name)==0) error("Assigning to subscripted non-variable\n");
    if (strlen(name)==1 && (name[0] < 'a' || name[0] > 'z'))
       error("Assigning to subscripted improper variable\n");
}

static void write_key(char* name,char* fname, char* mode)
{
     cur_out=fopen(fname,mode);
     if (cur_out==NULL) {
	cur_out=stdout;
	error("Error open for write: %s\n", fname);

    }
    printexpr(name,TO_EDIT,false);
    fclose(cur_out);
    cur_out = stdout;
}

