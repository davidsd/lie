/* bigint.c */

bigint* normalize(bigint* a);		   /* return a=normal(a) */
bigint* entry2bigint(entry n);
entry bigint2entry(bigint* num);
int cmp1(bigint* a, digit n);
int cmp(bigint* a, bigint* b);
bigint* mul1add(bigint* b, digit n, digit m); /* return b=b*n+m */
digit div1(bigint* b,digit n);		    /* b/=n; return b%n */
bigint* mult(bigint* a, bigint* b);
bigint* divq(bigint* a, bigint* b);		    /* a%=b; return a/b */
bigint* quotient(bigint* a, bigint* b);
bigint* mod(bigint* a, bigint* b);
bigint* str2bigint(string s);
string bigint2str(bigint* num);
int printbigint(bigint* num, int len);
bigint* norm(bigint* z);
bigint* add(bigint* a, bigint* b);
bigint* sub(bigint* a, bigint* b);
object power(object a, bigint* b, object unit, f2object f);
void addc(bigint** a, digit b);
void mulc(bigint** a, digit b);
void divc(bigint** a, digit b);

/* binmat.c */
 void Objectwrite(object m, group* g, char *t);
 void Objectsave(object m, char *t);
 object Objectread(group* g, char *t);
 object Objectget(char *t);

/* creatop.c */
symblst creatopsym(int n, strtype a, fobject f, int restype, ...);

/* date.c */

extern char date[];

/* learn.c */
boolean substring(char* key, char* name);
void build_directory(learn_index_tp* learn, long nitems);
boolean Learn(char* name);
entry exec_learn(char* name);

/* mapleread.c */
void maple_read(char* fname);

/* onoff.c */
void mark_defaultgrp_stack(void);
boolean set_on(entry num, strtype name);
boolean set_off(entry num, strtype name);
void save_state(void);
void restore_state(void);
void init_state(void);
void print_state(void);
void unmark_sorted(void);
cmp_tp height_incr(entry* v, entry* w, index len);
cmp_tp height_decr(entry* v, entry* w, index len);

/* output.c */
void monitor_cmd(char* file_arg);
void endmon(void);
void invoke_prog(char* prog, char* args);
boolean enter_input_file(char* fname);
boolean exit_input_file(boolean parse_error);
void print_runtime(void);
void registrate_cpu(void);

#ifdef __GNUC__
void error(char* format, ...)    /*  __attribute__ ((noreturn))*/
                                   __attribute__ ((format (printf,1,2)));
void fatal(char* format, ...)      __attribute__ ((noreturn))
                                   __attribute__ ((format (printf,1,2)));
void share_error(object o)         __attribute__ ((noreturn));
int Printf(char* format, ...)      __attribute__ ((format (printf,1,2)));
void err_Printf(char* format, ...) __attribute__ ((format (printf,1,2)));
void monprintf(char* format, ...)  __attribute__ ((format (printf,1,2)));
#else
void error(char* format, ...);
void fatal(char* format, ...);
void share_error(object o);
int Printf(char* format, ...);
void err_Printf(char* format, ...);
void monprintf(char* format, ...);
#endif

/* par.c */
void initpar(void);

/* print.c */
extern par_tp info_depth;
void print_vector(vector* v);
void print_matrix(matrix* m);
void print_poly(poly* p);
void print_mat_bars(matrix* m);
void print_poly_vertical(poly* p);
char* grp2str(group* grp);
int printgrp(object obj);
void printexpr(char* c, boolean pr, boolean doc);
boolean opname_test(char* name);
void writexpr(symblst s, short level);
void printint(object obj);
void printvec(object obj);
void printtekst(object obj);
void printgroup(object obj);
void printlst(symblst topsym);
boolean is_operator(char* aname);
int Printc(char* c);

void listops(symblst topsym, char* filename, boolean pr);
void listvars(symblst topsym, char* filename);
void listfuns(symblst topsym, char* filename);

/* poly.c */
poly* Add_pol_pol(poly* a, poly* b, int sign);
poly* Pol_from_vec(vector* v);
poly* Addmul_pol_pol_bin(poly* a, poly* b, bigint* c);
entry Degree_pol(poly* p);
entry Degree1_pol(poly* p);
poly* check_pol(poly* p, entry r);
poly* Mul_bin_pol(bigint* a, poly* b);
poly* Mul_pol_int(poly* a, intcel* b);
poly* Mul_pol_pol(poly* a, poly* b);
poly* Div_pol_bin(poly* a, bigint* b);
poly* Div_pol_vec(poly* a, vector* b);
poly* Mod_pol_bin(poly* a, bigint* b);
poly* Mod_pol_vec(poly* a, vector* b);
poly* Disjunct_mul_pol_pol(poly* p1, poly* p2);

/* box/altdom.c */
poly* Alt_dom(poly* p);
poly* Alt_dom_w(poly* p,vector* word);
poly* Demazure(poly* p,vector* word);

/* box/branch.c */
poly* Spectrum(poly* lambda, vector* toral_elm);
poly* Branch_irr(entry* lambda, entry** m, object G);
poly* Branch(poly* p, entry** m, object G);
poly* Collect(poly* p, matrix* iresmat, entry d, object g);

/* box/centr.c */
index isolcomp(matrix* ma, index i);
matrix* Centroots(matrix* mm);
object Centrtype(matrix* h);

/* box/closure.c */
matrix* Closure(matrix* m, boolean close, group* lietyp);
group* Carttype(matrix* ma);
matrix* Resmat(matrix* m);

/* box/contragr.c */
vector* Contragr(vector* g, object grp);
poly* Contragr_p(poly* p, object grp);

#include "coxeter.h"
#include "decomp.h"

/* box/defs.c */
void add_user_defined(void);

/* box/diagram.c */
object Diagram(object grp);

#include "domchar.h"

/* box/factor.c */
object Factor(bigint* num); /* static5.c */

#include "grpdata.h"

/* box/lr.c */
vector* To_Part_v(entry* wt,index l);
matrix* To_Part_m(entry** wts,index n,index l);
poly* To_Part_p(poly* p);
vector* From_Part_v(entry* wt,index l);
matrix* From_Part_m(entry** wts,index n,index l);
poly* From_Part_p(poly* p);
poly* LR_tensor_irr(entry* lambda, entry* mu, index n);
poly* LR_tensor(poly* p,poly* q);

/* box/matrix.c */
void copyrow(entry* v,entry* w,index n); /* ubiquitous, as are following: */
boolean eqrow(entry* v,entry* w,index n);
void add_xrow_to(entry* v,entry f,entry* w,index n);
void addrow(entry* v,entry* w,entry* x,index n);
void subrow(entry* v,entry* w,entry* x,index n);
boolean pos_subrow(entry* v,entry* w,entry* x,index n);
entry inprow(entry* v,entry* w,index n);
matrix* Transpose(matrix* m); /* static1.c */
void mulmatmatelm(entry** a,entry** b,entry** c,index l,index m,index n); 
void mulvecmatelm(entry* v,entry** b,entry* w,index m,index n); 
void mulmatvecelm(entry** a,entry* v,entry* w,index m,index n); 
matrix* Matmult(matrix* a, matrix* b);
matrix* extendmat(matrix* old);
matrix* copymatrix(matrix* old);
matrix* Blockmat(matrix* a,matrix* b);
void printarr(entry* a,index r); /* norm.c */

/* box/orbit.c */
matrix* Orbit(index limit, vector* v, entry** m, index n);

#include "plethysm.h"

/* box/sorting.w */
cmp_tp lex_incr(entry* v, entry* w, index len);
cmp_tp lex_decr(entry* v, entry* w, index len);
cmp_tp deg_incr(entry* v, entry* w, index len);
cmp_tp deg_decr(entry* v, entry* w, index len);
cmpfn_tp set_ordering(cmpfn_tp cmp, index n, object g);
void swap(entry* x,entry* y);
void sortrow(entry* a, index n);
void swap_rows(entry** x,entry** y); /* centr.c */
void swap_terms(entry** w, bigint** coef, index i, index j); /* altdom.c */
void Qksortmat(matrix* m, cmpfn_tp criterion);
matrix* Unique(matrix* m, cmpfn_tp criterion);
poly* Reduce_pol(poly* p);
index searchterm(poly* p, entry* t); /* altdom.c, decomp.c */

#include "symg.h"
#include "tensor.h"

/* box/weyl.c */
void simp_w_refl(entry* w,index i,simpgrp* g); /* dominant.c */
void w_refl(entry* almbda, index wi);
void Waction(entry* lambda, vector* word);
void Wrtaction(entry* root, vector* word);
matrix* simp_Weylmat(vector* w, simpgrp* g); /* weyl.c, weylloop.c */
matrix* Weyl_mat(vector* w);
matrix* Weyl_rt_mat(vector* w);
bigint* Worder(object grp);
bigint* sub_Worder(vector* v);
bigint* Orbitsize(entry* w);
matrix* Weyl_orbit(entry* v, matrix** orbit_graph);
matrix* Weyl_root_orbit(entry* v);
poly* Worbit_p(poly* p);
poly* alt_Wsum(poly* p);

#include "weylloop.h"

/* static/static1.c */
matrix* mat_null(entry r,entry c);
matrix* mat_id(entry size);
object mat_add_mat_vec(object m, object v);
object mat_add_mat_mat(object m, object v);
object vec_add_vec_vec(object m, object v);

/* static/static2.c */
object vec_addelm_vec_int(object v, object i);

/* box/static3.c */

void testdom(entry* v,object grp);
object int_eq_grp_grp(object g,object h); /* used in decomp */

/* static/static4.c */
object vid_print(object o);
object inside_vid_assign(symblst list,boolean glob,fobject update);

/* static/static5.c */

/* static/static6.c */

/* static/static7.c */
#undef digit
#undef objtype
