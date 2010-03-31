

void inittree(void), newtree(void);

symblst newnode(labeltp a);

symblst addnode(int n, strtype name, ...);

symblst addintnode(entry n), addbigintnode(bigint* b), addvecnode(int n)
 , addgroupnode(char,index), addtekstnode(char* t);

symblst addnamenode(strtype name), addlabelnode(labeltp l)
  , addtypnode(objtype typno),addtypnamenode(objtype typno, strtype name)
  , addprogramnode(symblst prog);
void addstaynode(object o);

char* code_type(objtype typeno),* code_class(int classno);

symblst copynode(symblst t),copytree(symblst s);

symblst load_function_body(funclst func);

funclst store_function_body(symblst arg,symblst expr);

void mark_tree(void);

void assign_type_to_node(symblst s);

void assignobject(symblst s,object t);
object force_integer(symblst topstack);

extern symbrec blank_symbol;
extern int tree_pt, label_pt;
extern unsigned long maxnodes, maxlabels;
extern labeltp label_null;

#define  isarrtype(sym)  (((sym)->type&ARR)!=0)
#define  isfuntype(sym)  (((sym)->type&FUN)!=0)
#define  isdefault(sym)  ((sym)->next==NULL && ((sym)->type&DEFAULT)!=0)


