#include  "lie.h"
#define local  static
#define right_margin  70
#define indent(n) Printf("%*s",(int)(n),"") 
#define entry_width  20 
#define NCOLS  6
#define NRECS  512 
#define needs_nl  (level*strlen(tab) != nc)


typedef struct {
    strtype keyword;
    long start;
    short size;
    short seq;
    unsigned char narg,t[7];
    par_tp parents;
    boolean directory;
} info_ind_tp;

typedef struct { char *t[REPORT_LEN]; int pt;} table_tp;
typedef struct { info_ind_tp *t[REPORT_LEN]; int pt;} ind_table_tp;

static char tab[] = "   "; /* 3 spaces */
static short ninfo;

static boolean to_show;

par_tp info_depth;

local ind_table_tp	     index_table;
local table_tp		 function_table;

local entry* max_entry_widths(entry** m, index nrows, index ncols)
{ char s[entry_width+1]; index i,j;
  entry* result=mkintarray(ncols);
  for (j=0; j<ncols; ++j)
  { entry min=0, max=0;
    for (i=0; i<nrows; ++i)
    { entry e=m[i][j]; if (e>max) max=e; else if (e<min) min=e; }
    sprintf(s, "%ld", (long)(min<-(max/10) ? min : max));
    result[j]=strlen(s);
  }
  return result;
}

local boolean is_a(char* name)
{ if (name==NULL || name[0]=='\0') return false;
  if (name[0] == 'X' && name[1]=='\0') return false;
  return isalpha(name[0]);
}


local int print_row(index n, entry* v, entry* width, index start_col)
{ index i; boolean wrapped=false;
  int col=start_col+Printf("[");
  for (i=0; i<n; ++i)
  { col+= width==NULL ? Printf("%ld",(long)v[i])
                     : Printf("%*ld",(int)width[i],(long)v[i]);
    if (i<n-1 && (col+=Printf(","))>=right_margin)
    { Printf("\n"); col=indent(start_col+1); wrapped=true; }
  }
  col+=Printf("]"); return wrapped ? right_margin : col;
}

void print_vector(vector* v)
{ indent(lmargin); print_row(v->ncomp,v->compon,NULL,lmargin); Printf("\n"); }

void print_matrix(matrix* m)
{ if (m->nrows==0) { indent(lmargin); Printf("null(0,%ld)",(long)m->ncols); }
  else
  { index i,n=m->nrows; entry* widths=max_entry_widths(m->elm,n,m->ncols);
    indent(lmargin); Printf("[");
    for (i=0; i<n; ++i)
    { print_row(m->ncols, m->elm[i],widths,lmargin+1); Printf("\n");
    indent(lmargin); Printf(i<n-1 ? "," : "]\n" );
    }
    freearr(widths);
  }
}

void print_poly(poly* p)
{ index i; int coef_width=0;
  entry* widths=max_entry_widths(p->elm,p->nrows,p->ncols);
  
  { bigint* max_coef=null,* min_coef=null; char* c; size_t l;
    for (i=0; i<p->nrows; ++i)
    { bigint* c=p->coef[i];
      if (c->size>0) { if (cmp(c,max_coef)>0) max_coef=c; }
      else { if (cmp(c,min_coef)<0) min_coef=c; }
    }
    c=bigint2str(max_coef); l=strlen(c); freem(c);
  c=bigint2str(min_coef); coef_width=Max(l,strlen(c)); freem(c);
  }
  
  { int col=indent(lmargin), rowsize;
    { int start=col;     col+=Printf("[");
      col+=printbigint(p->coef[0],coef_width); col+=Printf(",");    col=print_row(p->ncols, p->elm[0],widths,col); rowsize=col-start;
    }
    for (i=1; i<p->nrows; ++i)
    { 
      if (col+rowsize>=right_margin)
      { Printf("\n"); col=indent(lmargin); Printf(",");
            col+=printbigint(p->coef[i],coef_width); }
      else { col+=Printf(","); col+=printbigint(p->coef[i],coef_width); }
      col+=Printf(","); col=print_row(p->ncols, p->elm[i],widths,col);
    }
    Printf("]");}
  freearr(widths);
  Printf("\n");
}

void print_mat_bars(matrix* m)
{ if (m->ncols == 0 || m->nrows == 0)
  { Printf("(a %ld by %ld matrix)\n", (long)m->nrows, (long)m->ncols); }
  else
  { index i,j; entry** a=m->elm;
    entry* widths=max_entry_widths(a,m->nrows,m->ncols);
    for (i=0; i<m->nrows; ++i)
    { indent(lmargin); Printf("|");
      for (j=0; j<m->ncols; ++j) Printf(" %*ld",(int)widths[j],(long)a[i][j]);
      Printf(" |\n");
    }
    freearr(widths);
  }
}

void print_poly_vertical(poly* p)
{ index i,j; int coef_width=0;
  entry** a=p->elm;
  entry* widths=max_entry_widths(a,p->nrows,p->ncols);
  
  { bigint* max_coef=null,* min_coef=null; char* c; size_t l;
    for (i=0; i<p->nrows; ++i)
    { bigint* c=p->coef[i];
      if (c->size>0) { if (cmp(c,max_coef)>0) max_coef=c; }
      else { if (cmp(c,min_coef)<0) min_coef=c; }
    }
    c=bigint2str(max_coef); l=strlen(c); freem(c);
  c=bigint2str(min_coef); coef_width=Max(l,strlen(c)); freem(c);
  }
  for (i=0; i<p->nrows; ++i)
  { indent(lmargin); printbigint(p->coef[i],coef_width); Printf(" *");
    for (j=0; j<p->ncols; ++j) Printf(" %*ld",(int)widths[j],(long)a[i][j]);
    Printf("\n");
  }
  freearr(widths);
}

int Printc(char* c) { return Printf("%s",c); }

char* grp2str(group* grp)
{ index i; index n=0; char* buf;

  for (i=0; i<grp->ncomp; ++i)
    n+=sprintf(buffer,"%ld",(long)grp->liecomp[i]->lierank);
  n+=sprintf(buffer,"%ld",(long)grp->toraldim);
  buf=allocmem(n+grp->ncomp+2); /* one for `\.T', one for |'\0'| */

  for (n=0,i=0; i<grp->ncomp; ++i)
    n+=sprintf(&buf[n],"%c%ld"
             , grp->liecomp[i]->lietype, (long)grp->liecomp[i]->lierank);
  if (grp->toraldim>0 ||  grp->ncomp==0)
    sprintf(&buf[n], "T%ld", (long)grp->toraldim);
  return buf;
}

int printgrp(object obj)
{ char* buf=grp2str((group*) obj); int n=Printf("%s",buf);
  freem(buf); return n;
}

local void listvar(symblst topsym, boolean pr)
{
  object	    obj;
  to_show=pr;
  Printf("\n%s", name_tab[topsym->a.name]);
  if (pr)
      Printf(" :%6s", code_type(topsym->type));
  else      Printf(":=");  obj=topsym->data.val;
  switch (type_of(topsym))
  { case INTEGER:
      if (pr)
	  Printf(".");
      Printf("%7ld", obj->i.intval);
      break;
  case BIGINT:
      if (pr)
	  Printf(".  ");
      else
	  Printf("%7s","");
      printbigint((bigint*) obj,0);
      Printf("\n");
      break;
  case VECTOR:
      if (pr)
	  Printf(". with %ld components", (long)obj->v.ncomp);
      else {
	  Printf("\n");
	  print_vector(&obj->v);
      }
      break;
  case MATRIX:
      if (pr)
	  Printf(". with %ld rows and %ld columns",
	     (long)obj->m.nrows, (long)obj->m.ncols);
      else {
	  Printf("\n");
	  print_matrix(&obj->m);
      }
      break;
  case POLY:
      if (pr)
	  Printf(". with %ld monomials and degree %ld",
	     (long) obj->m.nrows, (long) obj->m.ncols);
      else {
	  Printf("\n");
	  print_poly(&obj->pl);
      }
      break;
  case GROUP:
      if (pr) Printf(".");      Printf("\t\""); printgrp(obj); Printf("\"");      break;
  case TEKST:
      if (pr)
      Printf(". with %ld characters", (long)obj->t.len);
      break;
  default:
      if (pr)
	  Printf(".");
      break;
  }
  if (pr && (isshared(obj)>1)) Printf("\t(shared %ld)",(long)(isshared(obj)-1));  Printf(";\n");}

local void listop(symblst topsym, boolean pr)
{
  to_show=false;
  if (!pr)
      Printf("\n#%7s	"," ");
  else
      Printf("\n%8s  ", " ");
  writexpr(topsym, -2);
  Printf("-> %s\n" ,code_type(topsym->type));
}

local void listfun(symblst topsym, boolean pr)
{
  symblst arglst=topsym->arglst;
  to_show=pr;
  Printf("\n%s (", name_tab[topsym->a.name]);
  while (arglst)
  {   Printf("%4s", code_type(arglst->type));
      Printf(" %s", name_tab[arglst->a.label->name]);
      if (arglst->next)
	  Printf("; ");
      arglst=arglst->next;
  }
  Printf(") =\n");
  writexpr(topsym->data.func->top_expr, -1);
  Printf("\n");
}

local par_tp info_all; /* information independent of directory path */
local par_tp info_silence; 

local void define_parent(par_tp* a, char* c)
{ a->n=1; a->p[0] = match(c,false); }

local boolean ismap(char* a)
{ return strcmp(a,"make")==0 || strcmp(a,"iapply")==0
    || strcmp(a,"vapply")==0 || strcmp(a,"mapply")==0
    || strcmp(a,"imap")==0 || strcmp(a,"vmap")==0 || strcmp(a,"repeat")==0
    || strcmp(a,"!")==0;
}

local void printpath(void)
{
  int i, n=info_depth.n-1;
  int pt=0;
  for (i=0; i<n; ++i)
    pt+=sprintf(promptlabel+pt,"/%s",name_tab[info_depth.p[i]]);
  if (n == -1) sprintf(promptlabel,"/%s",PROMPT);
  else sprintf(promptlabel+pt,"/%s%s",name_tab[info_depth.p[i]],PROMPT);
}

local void printheader(void)
{
  int n=info_depth.n-1;
  if (n == -1) Printf("'home'");
  else Printf("'%s'",name_tab[info_depth.p[n]]);
}

local void printfunctions(void)
{
  int i;
  for (i=0; i<function_table.pt; ++i) {
      if (i % NCOLS == 0)
	  Printf("\n");
      Printf("%-12.12s ", function_table.t[i]);
  }
  Printf("\n");
}

local void printindex(void)
{
  int		    i;
  Printf("List of the directory ");
  printheader();
  Printf(".\n");
  for (i=0; i<index_table.pt; ++i)
  {   info_ind_tp *rec=index_table.t[i];
      if (i % NCOLS == 0)
	  Printf("\n");
      if (rec->directory)
      {  char name[KEYWORDLEN+1];
	  sprintf(name,"%s>",name_tab[rec->keyword]);
	  Printf("%-12.12s ", name);
      }
      else
	  Printf("%-12.12s ", name_tab[rec->keyword]);
  }
  Printf("\n");
  Printf("\n%-12.12s %-12.12s %-12.12s %-12.12s %-12.12s\n",
  "path", "index","functions","home","..");
}

local short build_info_index(info_ind_tp* info)
{
  FILE* indexpt;
  short i=0;
  info_index_tp readinfo;
  define_parent(&info_all, "all");
  define_parent(&info_silence, "silence");
  
  { indexpt=fopen(infoind,readmode);
    if (!indexpt) {
        error("File %s cannot be opened.\n",infoind);
        exit(0);
    }
  }
  rewind(indexpt);
  while (i<NRECS &&
      fread(&readinfo,sizeof(info_index_tp),1,indexpt)==1)
  { int j;
    info[i].keyword=match(readinfo.keyword,true);
    info[i].start=readinfo.start;
    info[i].size=readinfo.size;
    info[i].seq=readinfo.seq;
    info[i].narg=readinfo.narg;
    for (j=0;j<readinfo.narg;++j) info[i].t[j]=readinfo.t[j];
    info[i].parents.n=readinfo.parents.n;
    for (j=0;j<readinfo.parents.n;++j)
	info[i].parents.p[j]=match(readinfo.parents.p[j],true);
    info[i].directory=readinfo.directory;
    ++i;
  }

  fclose(indexpt);
  if (i >= NRECS) error("Indexfile to large.\n");
  return i;
}

local boolean eqarg(symblst arg,info_ind_tp* t)
{
  int i=0;
  while (i<(int) t->narg && arg &&
  (type_of(arg) == (objtype) t->t[i]
#ifdef WITHCOEF
       || coerc_tab((objtype)t->t[i],type_of(arg))
#endif
  )) {
      ++i;
      arg=arg->next;
  }
  return i == (int) t->narg && !arg;
}

local boolean eq_path(par_tp* p,par_tp* q)
{ int i=0;
  if (p->n == q->n)
  {   while (i<p->n && p->p[i] == q->p[i]) ++i;
      if (i == p->n) return true;
  }
  return false;
}

local info_ind_tp* srchinfo
(info_ind_tp* info, short ninfo,char* name,symblst arg)
{
  short i=0;
  while (i<ninfo && (strcmp(name_tab[info[i].keyword],name)
  || !eqarg(arg,&info[i])|| !eq_path(&info[i].parents,&info_depth)))
      ++i;
  if (i<ninfo)  return info+i;
  i=0;
  /* Second search */
  while (i<ninfo &&
  (strcmp(name_tab[info[i].keyword],name) || !eqarg(arg,&info[i]))) ++i;
  if (i<ninfo)  return info+i;
  return NULL;
}

local void add_table(ind_table_tp* table, info_ind_tp* rec)
{ int i=0, j=table->pt;
  char* name=name_tab[rec->keyword];
  if (j >= REPORT_LEN) error("Table too small.\n");
  while (i<j && strcmp(name_tab[table->t[i]->keyword], name)<0)
      ++i;
  if (i<j && strcmp(name_tab[table->t[i]->keyword], name)==0)
      return;
  while (j>i) { table->t[j]=table->t[j - 1]; j--; }
  table->t[i]=rec;
  table->pt++;
}

local void add_function_table(table_tp* table, char* name)
{
  int i=0, j=table->pt;
  if (j >= REPORT_LEN) error("Table too small.\n");
  while (i<j && strcmp(table->t[i], name)<0) ++i;
  if (i<j && strcmp(table->t[i], name)==0) return;
  while (j>i) { table->t[j]=table->t[j - 1]; j--; }
  table->t[i]=name;
  table->pt++;
}

local void build_index(info_ind_tp* info,short ninfo)
{
  short i;
  char* name;
  index_table.pt=0;
  for (i=0;i<ninfo;++i)
  { name=name_tab[info[i].keyword];
    if (is_a(name))
    { if ((!info_depth.n && eq_path(&info[i].parents,&info_all)) ||
	   eq_path(&info[i].parents,&info_depth))
	add_table(&index_table, &info[i]);
    }
  }
}

local void build_from_static(symbrec s[], int n)
{
  int i;
  for (i=n-1;i>=0;i-=s[i].formal)
  {   if (is_a(name_tab[s[i].a.name]) && s[i].class == OPERATOR)
	  add_function_table(&function_table,name_tab[s[i].a.name]);
  }
}

local void listop_static(symbrec s[], int n, boolean pr)
{
  int i;
  for (i=n-1;i>=0;i-=s[i].formal)
      if (is_a(name_tab[s[i].a.name]) && s[i].class == OPERATOR)
	  listop((symblst) s+i,pr);
}

local void build_function_table(void)
{
  symblst	    list=top_definitions;
  function_table.pt=0;
  while (list)
  {   if (is_a(name_tab[list->a.name]) && list->class == OPERATOR)
	    add_function_table(&function_table,name_tab[list->a.name]);
      list=list->next;
  }
  build_from_static((symblst)static1,nstatic1);
  build_from_static((symblst)static2,nstatic2);
  build_from_static((symblst)static3,nstatic3);
  build_from_static((symblst)static5,nstatic5);
}

local void change_path(info_ind_tp* info)
{
  int i;
  static strtype silence=0;
  if (!silence) silence=match("silence",false);
  if (info->parents.n == 1 &&
      info->parents.p[0]==silence) return;
  for (i=0;i<info->parents.n;++i)
      info_depth.p[i]=info->parents.p[i];
  info_depth.n=info->parents.n;
  if (info->directory) {
      info_depth.p[i]=info->keyword;
      info_depth.n++;
  }
}

local boolean printdocument
(info_ind_tp* info,short ninfo,char* name,symblst arg,boolean doc)
{
  char* result;
  info_ind_tp* foundinfo;
  char infofile[LABELLENGTH];
  FILE* infopt;
  if (!arg && strcmp(name, "functions")==0)
  {   if (!function_table.pt) build_function_table();
      if (doc)
	  printfunctions();
      return true;
  }
  if (!arg && strcmp(name, "index")==0)
  {   build_index(info,ninfo);
      if (doc) printindex();
      return true;
  }
  if (name[0] == '$' && strlen(name) != 1) return false;
  foundinfo=srchinfo(info, ninfo, name, arg);
  if (!foundinfo) return false;
  result=(char*) malloc((unsigned long) (foundinfo->size) + sizeof(char));
  if (!result) error("No memory left for printing document.\n");
  sprintf(infofile,"%s.%ld",infofil,(long)foundinfo->seq);
  infopt=fopen(infofile,readmode);
  if (!infopt) error("Not possible to open %s.\n",infofile);
  fseek(infopt,foundinfo->start,0);
  fread(result,sizeof(char),foundinfo->size,infopt);
  result[foundinfo->size]='\0';
/* change search path */
  change_path(foundinfo);
  if (!doc) 
            { int i,nlines=0 ;
              int n=foundinfo->size;
              char* resultpt=result;
              for (i=0;i<n;++i)
                  if (result[i]=='\n') {
            	  result[i]='\0';
            	  nlines++;
                  }
              for (i=0;i<nlines;++i) {
                  Printf("# %s\n",resultpt);
                  resultpt+=(strlen(resultpt)+1);
              }
            }
  else
  { Printf("%s\n", result);
    if (foundinfo->directory) /* Print index directory */
    {
      build_index(info,ninfo);
      if (doc) printindex();
    }
  }
  free(result);
  fclose(infopt);
  return true;
}

void printexpr(char* c, boolean pr, boolean doc)
{
  int		    count=0;
  strtype	      a;
  static info_ind_tp* info;
  symblst top=top_definitions;
  static char tmpfile[L_tmpnam];
  boolean invoke_pager=false;

  
  { if (info==NULL) info=(info_ind_tp*)
        malloc((unsigned long) sizeof(info_ind_tp) * NRECS);
    if (!info) error("No memory available.%ld %ld\n",
  		   (long)sizeof(info_ind_tp), (long)NRECS);
  }
  if (c!=NULL) 
             {
               if (strcmp(c,"..")==0)
               {   if (info_depth.n) info_depth.n--;
                   build_index(info,ninfo);
                   if (doc) {
             	  printindex();
             	  printpath();
                   }
                   return;
               }
               if (strcmp(c,"home")==0)
               {   info_depth.n=0;
                   build_index(info,ninfo);
                   if (doc) {
             	  printindex();
             	  printpath();
                   }
                   return;
               }
               if (strcmp(c,"path")==0) {
                   printpath();
                   return;
               }
             }
  if (cur_out==stdout)
    { tmpnam(tmpfile); cur_out=fopen(tmpfile, "w"); invoke_pager=true; }
  if (c && c[0] == '.') error("The name %s is reserved.\n",c);

  if (ninfo==0) ninfo=build_info_index(info); /* get index */

  if (strlen(c)>0)
      if (ismap(c)) a=creatmapname(c);  else a=match(c,true);
  else a=match("help",false);

  if (strcmp(name_tab[a], "if")!=0 && strcmp(name_tab[a], "for")!=0)
  for (; top!=NULL; top=top->next)
    if (top->a.name == a)
    { switch (top->class)
      {
      case FUNCTION:
	  listfun(top, pr);
	  break;
      case OPERATOR:
      case MAP:
	  listop(top,pr);
	  break;
      case VALUE:
	  listvar(top, pr);
	  break;
      default:
	  break;
      }
      count++;
      if (top->arglst)
      printdocument(info, ninfo, name_tab[top->a.name], top->arglst, doc);
    }
  top=symbol_tab[a];
  while (top)
  {   if (top->a.name == a) listop(top,pr);
      count++;
      if (top->arglst)
      printdocument(info, ninfo, name_tab[top->a.name], top->arglst,
      doc);
      top=top->next;
  }
  if (printdocument(info, ninfo, name_tab[a], NULL,doc))
      count++;
  if (count == 0)
  Printf("There is no information about the term '%s'.\n",c);
  if (invoke_pager)
    { fclose(cur_out); cur_out=stdout;
        invoke_prog(pager,tmpfile); remove(tmpfile);
    }
  if (pr) printpath();
}

boolean is_operator(char* aname)
{
  char first=aname&&strlen(aname)?aname[0]:'?';
  return (!is_a(aname) && (first != '.') && (first != '_')
      && (first != '$'));
}

local int nextline(int indent)
{
  int i;
  int result=0;
  /*|
  if (!to_show) Printf(" \\");
  |*/
  Printf("\n");
  for (i=0; i<indent; ++i) result+=Printc(tab);
  return result;
}


#define  isif  (name == match(".if",false))
#define  isseq  (name == match(".sequence",false))
#define  isblock  (name == block_name)
#define  iswhile  (name == match(".whilefun",false))
#define  isforint   (u->a.label->name == match(".forint",false))
#define  isdownforint   (u->a.label->name == match(".downforint",false))
#define  isforvec  (u->a.label->name == match(".forvec",false))
#define  isformat  (u->a.label->name == match(".format",false))
#define  isstruc  (name == match(".strucval",false))
#define  isassign  (name == match(".assign",false))
#define  isupdate  (name == match(".update",false))
#define  isvecupdate  (name == match(".vecupdate",false))
#define  isaddupdate  (name == match(".+=",false) \
			|| name == match(".add_and_assign",false))
#define  isassign_loc  (name == match(".assign_loc",false))

void writexpr(symblst s, short level)
{
  symblst t=s->arglst;
  boolean bracket=false, c=false, only_name=false;
  char first;
  boolean comma=true;
  static int nc;
  strtype name= s->a.label?
	  (level>-2 ? s->a.label->name: s->a.name):
	  (strtype) 0;
  if (level< 0) { nc=0; level=0;}
  if (name!=empty_str)
  {   first=name_tab[name][0];
      c=(t != NULL);
      if (!is_operator(name_tab[name]) || first == '.'
	  || first == '_' || first == '$')
      {
	  if (first == '.') 
	               { if (isstruc) { bracket=true; c=false; }
	                 else if (isseq) { c=false; comma=false; }
	                 else if (isif)
	                 { if (needs_nl) nc+=nextline(level);
	                   nc+=Printc("if ");
	                   writexpr(t, level + 1);
	                   nc+=Printc(" then ");
	                   nc+=nextline(++level);
	                   writexpr(t->next->data.expr, level + 1);
	                   if (t->next->next)
	                   { nc+=nextline(--level);
	                     nc+=Printc(" else ");
	                     nc+=nextline(++level);
	                     writexpr(t->next->next->data.expr, level + 1);
	                   }
	                   nextline(--level);
	                   nc+=Printc("fi");
	                   return;
	                 }
	                 else if (iswhile)
	                 { if (needs_nl) nc+=nextline(level);
	                   nc+=Printc("while ");
	                   writexpr(t->data.expr, level + 1);
	                   nc+=Printc(" do ");
	                   level++;
	                   nc+=nextline(level);
	                   writexpr(t->next->data.expr, level	+ 1);
	                   level--;
	                   nc+=nextline(level);
	                   nc+=Printc(" od");
	                   return;
	                 }
	                 else if (isassign)
	                 { nc+=Printc(name_tab[t->a.label->name]);
	                   nc+=Printc("=");
	                   writexpr(t->next, level + 1);
	                   return;
	                 }
	                 else if (isassign_loc)
	                 { nc+=Printc("loc ");
	                   nc+=Printc(name_tab[t->a.label->name]);
	                   nc+=Printc("=");
	                   writexpr(t->next, level + 1);
	                   return;
	                 }
	                 else if (isupdate)
	                 { symblst lst=t->next->next;
	                   nc+=Printc(name_tab[t->a.label->name]);
	                   nc+=Printc("[ ");
	                   while (lst!=NULL) {
	               	writexpr(lst,level+1);
	               	lst=lst->next;
	               	if (lst!=NULL) nc +=Printc(", ");
	                   }
	                   nc+=Printc("]=");
	                   writexpr(t->next,level+1);
	                   return;
	                 }
	                 else if (isvecupdate)
	                 { nc+=Printc(name_tab[t->a.label->name]);
	                   nc+=Printc("|");
	                   writexpr(t->next->next,level+1);
	                   nc+=Printc("=");
	                   writexpr(t->next,level+1);
	                   return;
	                 }
	                 else if (isaddupdate)
	                 { symblst s=t->next;
	                   nc+=Printc(name_tab[t->a.label->name]);
	                   t=t->next;
	                   if (t->next!=NULL) /* add update structured element */
	                   { t=t->next;
	                     nc+=Printc("[");
	                     writexpr(t,level+1);
	                     if (t->next!=NULL)
	               	{ t=t->next; nc+=Printc(","); writexpr(t,level+1); }
	                     nc+=Printc("]");
	                   }
	                   nc+=Printc("+=");
	                   writexpr(s,level+1);
	                   return;
	                 }
	                 else { nc+=Printc(name_tab[name]+1); only_name=!isif && !isseq; }
	               }
	  else if (first == '_') 
	                    if (name == return_name)
	                    { nc+=Printc("return "); c=false; }
	                    else if (name == break_name) { nc+=Printc("break "); c=false; }
	                    else if (name == setdefault_name)
	                      { nc+=Printc("setdefault "); c=false; }
	                    else if (name == match("_select",false))
	                      { c=false; bracket=true; writexpr(t, level + 1); t=t->next; }
	                    else if (name == match("_on",false))
	                    { entry n=t->data.val->i.intval;
	                      nc+=Printc("on ");
	                      if (n == -3) nc+=Printc("+ ");
	                      if (n == -4) nc+=Printc("- ");
	                      if (n >= 0) nc+=Printf("%ld ",(long)n);
	                      t=t->next;
	                      nc+=Printc(t->data.val->t.string);
	                      t=t->next;
	                      c=false;
	                    }
	                    else if (name == match("_off",false))
	                    { nc+=Printc("off ");
	                      t=t->next;
	                      nc+=Printc(t->data.val->t.string);
	                      t=t->next;
	                      c=false;
	                    }
	                    else nc+=Printc(name_tab[name]);
	  else
	  {   if (s->class == BLOCK) 
	                         { symblst u=s->data.expr;
	                           if (isforint || isdownforint)
	                           { if (needs_nl) nc=nextline(level);
	                             nc+=Printc("for ");
	                             nc+=Printc(name_tab[s->arglst->formal]);
	                             nc+=Printc("=");
	                             writexpr(s->arglst, level + 1);
	                             isforint? nc+=Printc(" to "):Printc(" downto ");
	                             writexpr(s->arglst->next, level + 1);
	                             nc+=Printc(" do ");
	                             ++level;
	                             nc=nextline(level);
	                             writexpr(u->arglst->next->data.expr, level);
	                             --level;
	                             nc=nextline(level);
	                             nc+=Printc("od");
	                             return;
	                           }
	                           if (isforvec || isformat)
	                           { if (needs_nl) nc=nextline(level);
	                             nc+=Printc("for ");
	                             nc+=Printc(name_tab[s->arglst->formal]);
	                             if (isforvec) nc+=Printc(" in ");
	                             else nc+=Printc(" row ");
	                             writexpr(s->arglst->next, level + 1);
	                             nc+=Printc(" do ");
	                             ++level;
	                             nc=nextline(level);
	                             writexpr(u->arglst->next->data.expr, level);
	                             --level;
	                             nc=nextline(level);
	                             nc+=Printc("od");
	                             return;
	                           }
	                           nc+=Printc("{");
	                           writexpr(s->data.expr, level+1);
	                           nc+=Printc("}");
	                           return;
	                         }
	      nc+=Printc(name_tab[name]);
	  }
	  if (c)
	      nc+=Printc("(");
	  if (bracket)
	      nc+=Printc("[");
	  while (t)
	  {   if (only_name)
	      {  strtype tname=t->a.label?t->a.label->name:0;
		  if (tname)
		      nc+=Printc(name_tab[tname]);
		  else
		      nc+=Printc(code_type(t->type));
		  only_name=false;
	      }
	      else
		  writexpr(t, level);
	      t=t->next;
	      if (t)
	      {  if (nc>right_margin) nc=nextline(level);
		  nc+=comma ? Printc(",") : Printc(";");
	      }
	  }
	  if (bracket)
	      nc+=Printc("]");
	  if (c)
	      nc+=Printc(")");
      }
      else 
           { char* aname=t->a.label?name_tab[t->a.label->name]:(char*) NULL;
             if (t->next) /* infix */
             { if (is_operator(aname)) nc+=Printc("(");
               writexpr(t, level);
               if (is_operator(aname)) nc+=Printc(")");  else nc+=Printc(" ");
             }
             nc+=Printc(name_tab[name]);
             if (t->next) t=t->next;
             if (is_operator(aname)) nc+=Printc("(");  else nc+=Printc(" ");
             writexpr(t, level);
             if (is_operator(aname)) nc+=Printc(")");
           }
  }
  else if (type_of(s) == INTEGER && s->class == VALUE)
      nc+=Printf("%ld", (long)s->data.val->i.intval);
   else if (type_of(s) == BIGINT && s->class == VALUE)
      nc+=printbigint((bigint*) s->data.val,0);
   else if (type_of(s) == GROUP && s->class == VALUE)
      nc+=printgrp(s->data.val);
  else if (type_of(s) == TEKST && s->class == VALUE)
  { nc+=Printc("\"");
    nc+=Printc((char*) s->data.val->t.string);
    nc+=Printc("\"");
  }
  else nc+=Printc(code_type(s->type));
}

#undef isif
#undef isseq
#undef isblock
#undef iswhile
#undef isforint
#undef isdownforint
#undef isforvec
#undef isformat
#undef isstruc
#undef isassign
#undef isupdate
#undef isvecupdate
#undef isaddupdate
#undef isassign_loc


void printint(object obj)
{
  indent(lmargin);
  Printf("%ld\n", (long)obj->i.intval);
}

void printvec(object obj)
{
  int		    i;
  indent(lmargin);
  Printf("[");
  for (i=0; i<obj->v.ncomp; ++i)
      Printf(" %ld", (long)obj->v.compon[i]);
  Printf(" ]\n");
}

void printtekst(object obj)
{
  indent(lmargin);
  Printf("%s\n", obj->t.string);
}

void printgroup(object obj)
{
  indent(lmargin);
  printgrp(obj);
  Printf("\n");
}

void printlst(symblst topsym)
{
  symblst arglst;
  if (topsym==NULL) Printf("topsym= NULL \n");
  while (topsym!=NULL)
  {   Printf("%10s", code_class(topsym->class));
      Printf("%10s ", name_tab[topsym->a.name]);
      Printf("%10s", code_type(topsym->type));
      if (topsym->class == OPERATOR || topsym->class == FUNCTION)
      { Printf(" <- ");
        for (arglst=topsym->arglst; arglst!=NULL; arglst=arglst->next)
	  Printf("%10s", code_type(arglst->type));
      }
      Printf("\n");
      topsym=topsym->next;
  }
}

void listvars(symblst topsym, char* filename)
{
  char tmpfile[L_tmpnam];
  if (topsym==NULL) Printf("topsym= NULL\n");
  
  if (pager[0]=='\0') cur_out=stdout;
  else
  { if (filename!=NULL) cur_out=fopen(filename, "w");
    else { tmpnam(tmpfile); cur_out=fopen(tmpfile, "w"); }
    if (cur_out==NULL) cur_out=stdout;
  }
  Printf("\n #  List of defined variables # \n\n");
  for (;topsym!=NULL;topsym=topsym->next)
    if ((topsym->class == VALUE)
	&& (!filename || type_of(topsym) == INTEGER
	|| type_of(topsym) == VECTOR || type_of(topsym) == MATRIX)
	&& (name_tab[topsym->a.name][0] != '$')
	&& (name_tab[topsym->a.name][0] != '.'))
      listvar(topsym,filename==0);
  
  { if (cur_out!=stdout)
    { fclose(cur_out); cur_out=stdout;
      if (filename==NULL) { invoke_prog(pager,tmpfile); remove(tmpfile); }
    }
  }
}

void listfuns(symblst topsym, char* filename)
{ char tmpfile[L_tmpnam];
  
  if (pager[0]=='\0') cur_out=stdout;
  else
  { if (filename!=NULL) cur_out=fopen(filename, "w");
    else { tmpnam(tmpfile); cur_out=fopen(tmpfile, "w"); }
    if (cur_out==NULL) cur_out=stdout;
  }
  Printf("\n #  List of defined functions # \n\n");
  for (;topsym!=NULL;topsym=topsym->next)
    if (topsym->class == FUNCTION && (name_tab[topsym->a.name][0] != '$'))
      listfun(topsym, filename==NULL);
  
  { if (cur_out!=stdout)
    { fclose(cur_out); cur_out=stdout;
      if (filename==NULL) { invoke_prog(pager,tmpfile); remove(tmpfile); }
    }
  }
}

void listops(symblst topsym, char* filename, boolean pr)
{ char tmpfile[L_tmpnam];
  
  if (pager[0]=='\0') cur_out=stdout;
  else
  { if (filename!=NULL) cur_out=fopen(filename, "w");
    else { tmpnam(tmpfile); cur_out=fopen(tmpfile, "w"); }
    if (cur_out==NULL) cur_out=stdout;
  }
  Printf("\n #  List of defined operators # \n\n");
  for (;topsym!=NULL;topsym=topsym->next)
    if (topsym->class == OPERATOR) listop(topsym,pr);
  listop_static((symblst)static3,nstatic3,pr);
  
  { if (cur_out!=stdout)
    { fclose(cur_out); cur_out=stdout;
      if (filename==NULL) { invoke_prog(pager,tmpfile); remove(tmpfile); }
    }
  }
}

