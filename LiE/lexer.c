#include  "lie.h"
#include  "parser.h" 
#define local  static
#define STRBLSIZE  500 
#define hash_mod  1997 
#define shift  (c= backed_up ? backed_up=false, next_c : fetch_char())
#define unput(c) (backed_up=true,next_c=c)
#define first_semi_keyword  HELP 
#define first_type_id  (HELP-6) 
#define keyword_limit  array_size(keyword) 
#define subject_name_limit  64


int lineno=1;

local char* char_point=NULL;
local int chars_left=0;

char* name_tab[hash_mod]; /* actual names of identifiers */
local short hash_tab[hash_mod];
  /* non-negative entries point into |name_tab| array */
local short hash_count=0;
  

symblst symbol_tab[hash_mod];

local char* tok_buf; /* points to currently recorded string */
local int   tok_len,tok_buf_size;
  

local int c, next_c;
local boolean backed_up=false; /* whether we should use |next_c| next */
local boolean start_of_command;
local enum { normal, help, edit, learn, ended } command_type;
local boolean ignore_nl; /* whether |'\n'| is treated as space */
local int nesting; /* number of pending opening symbols */
local boolean short_ints; 

char* keyword[]=
{ "",
  "if", "then", "else", "fi",
  "for", "to", "downto", "in", "row",  "while", "do", "od",
  "loc", "return", "break",
"make", "iapply", "vapply", "mapply",
  "on", "off", "setdefault", "savestate", "restorestate",
"mat", "vec", "int", "grp", "tex", "pol",

  "help", "type",
  "edit", "read", "write", "monfil",
  "learn", "exec", "quit", "exit",
  "listvars", "listfuns", "listops",
  "maxnodes", "maxobjects"
};

char subject_name[subject_name_limit];


local char* store(char* s,int len)
{ int n= ++len>STRBLSIZE ? len : STRBLSIZE; /* size of new block if needed */
  char* result=char_point;
  if (len>chars_left)
     result=char_point=alloc_array(char,chars_left=n);
  char_point+=len; chars_left-=len; return strncpy(result,s,len);
}

local short hash(char* s) /* |s| is a non-empty string */
{ char c; long r=*s++; while ((c=*s++)!='\0') r=(c+(r<<8))%hash_mod;
  return (short)r;
}

strtype match(char* str, boolean copy)
{ short i,h=hash(str);
  while ((i=hash_tab[h])>=0)
    if (strcmp(name_tab[i],str)==0) return i; /* identifier found in table */
    else if (++h==hash_mod) h=0; /* move past occupied slot */
  if (hash_count>=hash_mod-1) fatal("Hash table overflow");
  hash_tab[h]= hash_count; /* this number represents the new identifier */
  name_tab[hash_count]=copy ? store(str,(int)strlen(str)): str;
  return hash_count++;
}

short creatmapname(char *a)
{ char *s=strcat(strcpy(alloc_array(char,strlen(a)+2),"."),a);
  short result=match(s,true); free(s); return result;
}

void clear_symbol_tab(void)
{ int i; for (i=0; i<hash_mod; ++i) symbol_tab[i]=NULL; }

local void app_c(char c)
{ if (tok_len==tok_buf_size)
  { char* p=tok_buf; 
    tok_buf=strncpy(alloc_array(char,tok_buf_size*=2),tok_buf,tok_len);
    freearr(p);
  }
  tok_buf[tok_len++]=c;
}

void init_scanner(void) { 
                          { int i; for(i=0; i<hash_mod; ++i) hash_tab[i]=-1; }
                          
                          clear_symbol_tab();
                          
                           tok_buf=alloc_array(char,tok_buf_size=8);
                          
                          { int i; for (i=0; i<keyword_limit; i++) match(keyword[i],false);
                          }
 }
void init_command(void)
{ start_of_command=true; command_type=normal; ignore_nl=false; nesting=0;
  short_ints=false; fetch_line(promptlabel);
}
void clear_input(void)
{ if (command_type!=ended)  do shift; while(c!='\n' && c!=EOF);  }

local void skip_space(void)
{ do
  { if (c=='\\') ignore_nl=true;
    else if (c=='\n')
    {  ++lineno;
      if (ignore_nl || nesting>0)
      { ignore_nl=false; fetch_line(PROMPT2); }
      else break; /* stop at non-ignored newline */
    }
    else if (c=='#')
    { do shift; while (c!='#' && c!='\n' && c!=EOF);
      if (c!='#') continue;
	/* |'\n'| requires action, |EOF| must not be skipped */
    }
    else if (!isspace(c)) break;
    shift;
  } while(true);
}

int yylex(void)
{ short code;
  if (command_type==ended) return 0;
restart: shift; skip_space(); ignore_nl=false;
  if (c==EOF || c=='\n')
  { if (c==EOF && cur_in==stdin) stop=true;
    command_type=ended; return ENTER;
  }
  if (command_type!=normal) 
                          if (c=='>' && command_type==help)
                          { code= shift!='>' ? unput(c),WRITEHELP : APPENDHELP; command_type=edit; }
                          else
                          { code=ANY_STRING; tok_len=0;
                            do app_c(c),shift;
                          while (c!='\n' && c!=EOF &&
                                   !(command_type!=learn &&
                          	   (isspace(c) || command_type==help && c=='>'))
                          	);
                            if (command_type==learn) /* then strip trailing spaces */
                              while (tok_len>0 && isspace(tok_buf[tok_len-1])) --tok_len;
                            app_c('\0'); unput(c);
                            if (command_type!=help) yylval.text=tok_buf;
                            else
                            { sprintf(subject_name,"%.*s",subject_name_limit-1,tok_buf);
                              yylval.text=subject_name;
                            }
                          }
  else
  { if (isupper(c)) 
                    { char type=c; int next=shift;  unput(next); c=type;
                      if (!isalpha(next) && next!='_')
                        /* else fall through for multi-letter identifier */
                      { if (strchr("ABCDEFGT",type)!=NULL)
                          { code=GROUPID; yylval.sub=type; short_ints=true; }
                        else if (type=='X' || type=='Y') code=X;
                        else error ("Lone `%c' not legal as identifier.\n",type);
                        goto finish;
                      }
                    }
    if (isalpha(c)) 
                    { tok_len=0; 
                      do app_c(c), shift; while(isalpha(c) || isdigit(c) || c=='_'); 
                      app_c('\0'); unput(c);
                    code=match(tok_buf,true);
                      if (code>= (start_of_command ? keyword_limit : first_semi_keyword))
                        { yylval.sub=code; code=IDENT; }
                      else if (code>=first_type_id && code<first_semi_keyword)
                        { yylval.sub=code-first_type_id; code=TYPEID; }
                      else switch(code)
                      {      case IF: case FOR: case WHILE: ++nesting;
                      break; case FI: case OD: --nesting;
                      break; case APPLY: case APPLY+1: case APPLY+2:
                        yylval.sub=code-APPLY; code=APPLY;
                      break; case LISTCOMMAND: case LISTCOMMAND+1: case LISTCOMMAND+2:
                        yylval.sub=code-LISTCOMMAND; code=LISTCOMMAND;
                      break; case QUIT+1: --code; /* \.{exit} $\to$ \.{quit} */
                      break; case HELP: yylval.sub=0; command_type=help;
                      break; case MAXNODES: case MAXOBJECTS: case ON:
                        short_ints=true; goto finish;
                      break; case EDIT: case READ: case WRITE: case MONFIL: command_type=edit;
                      break; case LEARN: case EXEC: command_type=learn;
                      }
                    }
    else if (isdigit(c)) 
                         { tok_len=0;  do app_c(c),shift; while (isdigit(c)); 
                           app_c('\0'); unput(c);
                           if (short_ints) { code=INT; yylval.val=atol(tok_buf); }
                           else { code=NUMBER; yylval.num=str2bigint(tok_buf); }
                         }
    else 
         switch(c)
         {
         default: printf("Unrecognised character `%c'\n",c); goto restart;
                case '"': 
                       { tok_len=0;
                         do
                         { while (shift!='"' && c!='\n' && c!=EOF) app_c(c);
                           if (c!='"')
                           { printf("Unclosed string denotation on line %d.\n" 
                             "\t(for multi-line strings use string-break.)\n", lineno);
                             break;
                           }
                           if (shift=='"') app_c(c);  else skip_space();
                         } while (c=='"'); /* continue after quote-image or string-break */
                         app_c('\0'); unput(c); yylval.sym=addtekstnode(tok_buf); code=TEXT;
                       }
          
         break; case '?': code=HELP; yylval.sub=1; command_type=help;
         break; case '@': code=QUIT;
         break; case ';': code=SEMICOLON; ignore_nl=true;
         break; case ',': code=COMMA;
         break; case '(': code=LPAR; ++nesting;
         break; case ')': code=RPAR; --nesting;
         break; case '{': code=LBRACE; ++nesting;
         break; case '}': code=RBRACE; --nesting;
         break; case '[': code=LBRACK; ++nesting;
         break; case ']': code=RBRACK; --nesting;
         break; case '|':  if (shift=='|') code=OR;  else { unput(c); code=BAR;}
         		  ignore_nl=true;
         break; case '&':  if (shift!='&') unput(c);  code=AND; ignore_nl=true;
         break; case '!':  if (shift=='=') { code=RELOP; yylval.sub=1; }
         		  else { unput(c); code=NOT; }
         break; case '=':  if (shift=='=') { code=RELOP; yylval.sub=0; }
         		  else { unput(c); code=BECOMES; }
         		 ignore_nl=true;
         break; case '<': code=RELOP; yylval.sub= shift=='=' ? 4 : (unput(c),2);
         		 ignore_nl=true;
         break; case '>': code=RELOP; yylval.sub= shift=='=' ? 5 : (unput(c),3);
         		 ignore_nl=true;
         break; case '+':  if (shift=='=') code=PLUSAB;
         		  else { unput(c); code=ADDOP; yylval.sub=0; }
         		 ignore_nl=true;
         break; case '-': code=ADDOP; yylval.sub=1; ignore_nl=true;
         break; case '*': code=TIMES; ignore_nl=true;
         break; case '/': code=DIVOP; yylval.sub=0; ignore_nl=true;
         break; case '%': code=DIVOP; yylval.sub=1; ignore_nl=true;
         break; case '^': code=POWER; ignore_nl=true;
         break; case '$': code=DOLLAR; tok_len=0;
         		 do app_c(c),shift; while(isdigit(c)); 
         		 app_c('\0'); unput(c);
         		 yylval.sub=match(tok_buf,true);
         break; case ':': code=COLON; command_type=learn;
         }
  }
  short_ints=false; /* flag lasts for one token only */
finish: start_of_command=false; return code;
}

