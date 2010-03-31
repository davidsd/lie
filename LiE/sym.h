

void addsym(strtype name, symblst arg, symblst expr, int class);
void assignsym(symblst s, symblst t);
void push(symblst lst, symblst sym);
symblst push_value(symblst list);
symblst pop_value(symblst topoldsym);
void assignnames(symblst list, symblst name_list);
void add_dollar_names(symblst list);

symblst srchsym(symblst lst, strtype name, symblst arg);

symbrec* creatsym(strtype a);

void error_prototype(string name, symblst arg);

void assigntype(objtype t, symblst list);

void error_not_foundsym(symblst s);


#define  coerc_tab(t1,t2)   \
    (t1 == INTEGER && t2 == BIGINT ? (fobject)int2bin : \
     t1 == BIGINT  && t2 == INTEGER ? (fobject)bin2int : \
     (fobject) NULL)

#define  eq_types(t1,t2) ((t1 == t2) || coerc_tab(t1,t2))


