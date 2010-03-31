#include "Python.h"
#include "lie.h"

/* changes to lie.h: */
/*
- #define MAXPTRS_DFLT            99999
+ #define MAXPTRS_DFLT            9999999

+ #ifndef Py_LONGINTREPR_H
| typedef unsigned short digit;
| typedef unsigned long twodigits;
+ #endif
*/

/* changes to ansi.h */
/*
- void error(char* format, ...)      __attribute__ ((noreturn))
+ void error(char* format, ...)    / *  __attribute__ ((noreturn)) * /
*/

/* changes to mem.c */
/*
- static void **ptr;
+ / *static void **ptr;* / / * now globally defined * /
*/

/* macro for creating a new object without initializing it */
/* used to instantiate extension classes from cython. Does NOT bypass
   __cinit__! */
#define PY_NEW(T) \
  (((PyTypeObject*)(T))->tp_new( \
                                (PyTypeObject*)(T), __pyx_empty_tuple, NULL))

/* needed in gc.c */
extern void **ptr;
void c_print_objs(void);

/******* from static1.c *******/

extern long chunks;
/*void error_unequal(entry a,entry b);*/           /* not needed */
vector* vec_min_vec(vector* a);                    /* implemented */
/*object vec_not_vec(vector* a);*/                 /* not needed */
matrix* mat_min_mat(matrix* a);                    /* implemented */
/*entry imod(entry x,entry n);*/                   /* not needed */
vector* vec_mod_vec_int(vector* a, entry b);       /* implemented */
matrix* mat_div_mat_int(matrix* a, entry b);       /* implemented */
matrix* mat_mod_mat_int(matrix* a, entry b);       /* implemented */
/*object bin_pow_bin_bin(object a, object b);*/    /* not needed */
/*object pol_pow_pol_bin(object a, object b);*/    /* not needed */
vector* vec_mul_int_vec(long a, vector *b);        /* implemented */
vector* vec_div_vec_int(vector* a, long b);        /* implemented */
vector* vec_mul_mat_vec(matrix *a, vector *b);     /* implemented */
vector* vec_mul_vec_mat(vector *v, matrix *m);     /* implemented */
vector* vec_append_vec_vec(vector *v, vector *w);  /* implemented */
matrix* mat_mul_mat_mat(matrix *a, matrix *b);     /* implemented */
poly* pol_mul_pol_mat(poly *a, matrix *b);         /* implemented */
/*matrix* mat_id(index size);*/                    /* implemented - static/static1.c */
/*object mat_pow_mat_bin(object a, object b);*/    /* not needed */
matrix* mat_append_mat_mat(matrix* a, matrix* b);  /* implemented */
matrix* mat_transpose_mat(matrix *a);              /* implemented */
index int_mul_vec_vec(vector *a, vector *b);       /* implemented */
/*object vec_add_vec_vec(object v, object w);*/    /* implemented - static/static1.c */
/*object mat_add_mat_vec(object a, object v);*/    /* implemented - static/static1.c */
object mat_sub_mat_int(object a, entry obj_k);     /* implemented */
vector* vec_dif_vec_vec(vector *a, vector *b);     /* implemented */
/*object mat_add_mat_mat(object a, object b);*/    /* implemented */
matrix* mat_sub_mat_mat(matrix *a, matrix *b);     /* implemented */
matrix* mat_mul_int_mat(entry a, matrix *b);       /* implemented */
/*object int_siz_vec(object a);*/                  /* accessed directly */
/*object int_rowsiz_mat(object a);*/               /* accessed directly */
/*object int_colsiz_mat(object a);*/               /* accessed directly */
/*object int_n_vars_pol(object p);*/               /* accessed directly */
/*object mat_null_int_int(object m_obj,object n_obj);*/ /* not needed */
/*object mat_one_int_int(object m_obj, object n_obj);*/ /* not needed */
/*object pol_null_int(intcel *r);*/                     /* not needed */
/*object pol_one_int(intcel *r);*/                      /* not needed */
/*object mat_unit_int(intcel* nn);*/                    /* not needed */
/*object vec_unit_int_int(intcel* nn, intcel* ii);*/    /* not needed */
/*object vec_null_int(intcel* nn);*/                    /* not needed */
/*object vec_one_int(intcel* nn);*/                     /* not needed */

/******* from static2.c ********/

/* Boolean operations */
/*object Int_search_mat_vec(matrix* m,vector* v,index low,index up);*/
/*object int_and_int_int(symblst a);*/
/*object int_or_int_int(symblst a);*/
/*object int_not_int(object a);*/
object int_eq_mat_mat(object a, object b);           /* implemented */
object int_eq_pol_pol(object a, object b);           /* implemented */
object int_ne_mat_mat(object a, object b);           /* implemented */
object int_ne_pol_pol(object a, object b);           /* implemented */
object int_eq_vec_vec(object a, object b);           /* implemented */
object int_ne_vec_vec(object a, object b);           /* implemented */
/*object vec_addelm_vec_int(object a, object b);*/
object vec_startaddelm_int_vec(entry a, object b);   /*** TODO ***/
object vec_subelm_vec_int(object a, entry b);        /* implemented */
/*object int_select_vec_int(vector *t, entry n);*/   /* not needed */
object vec_select_mat_int(object m, entry indexobj); /* implemented */
/*object pol_select_pol_int(poly *m, entry indexobj);*/  /* not needed */
object vec_select_pol_int(poly *m, entry indexobj);  /* implemented */
object bin_select_pol_vec(poly *p, vector *v);       /* implemented */
object Int_search_mat_vec(matrix* m, vector* v, index low, index up);
object int_search_mat_vec(matrix *m, vector *v);
object int_search_mat_vec_int_int(matrix *m, vector *v, entry up, entry low);
object int_select_mat_int_int(matrix *m, entry n1, entry n2);
/*object int_used();*/
/*object arg_break_arg(object a);*/
/*object arg_return_arg(object a);*/
/*object vid_break();*/
/*object vid_return();*/
/*object apply(symblst funsym);*/
/*object vec_apply_int_int(symblst funsym);*/
object vec_join_mat(object a);                       /*** TODO ***/
object vec_diag_mat(object a);                       /* implemented */
poly* pol_monom_vec(vector *v);                      /* implemented */
/*object pol_polynom_bin_mat(object p, bigint* d);*/ /* not needed */
poly* pol_polynom_mat(matrix* p);                    /* implemented */
/*poly* pol_polynom_bin(bigint *k);*/                /* pol_polynom_int instead */
poly* pol_polynom_int(entry k);                      /* implemented */
matrix* mat_polynom_pol(poly *p);                    /* implemented */
object pol_polynom_bin_vec(bigint* d, vector* v);    /* implemented */
object pol_polynom_vec(vector *v);                   /* implemented */
/*object pol_polynom_bin_int(bigint *d, entry k);*/  /* not needed */
/*object pol_polynom_one_int(entry k);*/             /* not needed */
/*object bin_coef_pol_int(poly *p, entry k);*/       /* accessed directly */
object pol_min_pol(poly	*a);                         /* not needed */
/*object pol_add_pol_pol (object a, object b);*/     /* replaced by Add_pol_pol */
/*object pol_sub_pol_pol (object a, object b);*/     /* replaced by Add_pol_pol */
/*object pol_mul_bin_pol(object a, object b);*/      /* replaced by Mul_bin_pol */
/*object pol_mul_pol_int(object a, entry b);*/       /* replaced by Mul_bin_pol */
/*object pol_div_pol_bin(object a, object b);*/      /* replaced by Div_pol_bin */
/*object pol_div_pol_vec(object a, object b);*/      /* replaced by Div_pol_vec */
/*object pol_mod_pol_bin(object a, object b);*/      /* replaced by Mod_pol_bin */
/*object pol_mod_pol_vec(object a, object b);*/      /* replaced by Mod_pol_vec */
/*object pol_mul_pol_pol(object a, object b);*/      /* replaced by Mul_pol_pol */
/*object int_deg_pol(object p);*/                    /* replaced by Degree_pol */
/*object int_len_pol(object p);*/                    /* not needed */
/*object int_len_tek(tekst *t);*/                    /* not needed */
object pol_atensor_pol_pol(object p1, object p2);    /* implemented */

#ifndef NOSAVEMAT
/*void rank_of_group_okay(group* g);*/               /* not needed */
object tex_maxsub_grp(group *g);                     /* implemented */
group* create_group(string s);                       /* implemented */
object grp_maxsub_grp_int(group *g, entry nobj);     /* implemented */
object mat_resmat_grp_grp(group *g1, group *g2);     /* implemented */
object mat_resmat_grp_int_grp(group *g1,entry d, group *g2); /* implemented */
#endif

/******* from static3.c *******/

/* Group operations */
void check_wt(vector* lambda, index r);
void check_wts(matrix* m, index r);
void check_rt(vector* rt, index s);
void check_rts(matrix* m, index s);
void check_toral(vector* t, index r, index lim);
void check_torals(matrix* m, index r);
/*void testdom(entry* v, object grp);*/
void check_Ww(vector* ww, index s);
void check_Wws(matrix* m, index s);
/*object int_eq_grp_grp(object g1, object g2);*/     /* used externally (decomp) */
object grp_mul_grp_grp(object g1, object g2);        /* implemented */
object grp_select_grp_int(object g, entry n);        /* implemented */
object vec_sort_vec(object v);
object mat_sort_mat(matrix* m);
object mat_unique_mat(matrix* m);
object mat_blockmat_mat_mat(matrix* a,matrix* b);    /* implemented */
object vid_setdefault_grp(object g);                 /* not needed */
object vid_setdefault(void);                         /* not needed */
object grp_setdefault(void);                         /* not needed */
object mat_center_grp(object g);                     /* replaced with Center */
object int_dim_grp(object g);                        /* implemented */
object vid_diagram_grp(object g);                    /* replaced with Diagram */
object vec_liecode_grp(object g);                    /* implemented */
object groupmake(char lietype,index rank);           /* implemented */
object grp_liegroup_int_int(entry typ, entry rank);  /* implemented */
/*object int_ncomp_grp(group* g);*/ 
object int_lierank_grp(object g);                    /* implemented */
object int_cartan_vec_vec_grp(vector* v, vector* w, object g); /* implemented */
object mat_cartan_grp(object g);                     /* implemented */
object grp_carttype_mat_grp(matrix* m, object g);    /* implemented */
object mat_centroots_vec_grp(vector* t, object g);   /* implemented */
object mat_centroots_mat_grp(matrix* m, object g);   /* implemented */
object grp_centrtype_vec_grp(vector* t, object g);   /* implemented */
object grp_centrtype_mat_grp(matrix* m, object g);   /* implemented */
object mat_closure_mat_grp(matrix* m, object g);     /* from box/static3.c */ /* implemented */
object int_detcart_grp(object g);                    /* implemented */
object mat_domweights_vec_grp(vector* v, object g);  /* implemented */
object mat_fundam_mat_grp(matrix* m, object g);      /* from box/static3.c */ /* implemented */
object vec_highroot_grp(object grp);                 /* implemented */
object mat_icartan_grp(object g);                    /* implemented */
object int_inprod_vec_vec_grp(vector* v, vector* w, object g); /* implemented */
object int_norm_vec_grp(vector* root, object g);     /* implemented */
object int_numproots_grp(object g);                  /* implemented */
object mat_posroots_grp(object g);                   /* implemented */
object mat_bhdesc_vec_grp(vector* w, object g);      /* implemented */
object mat_bhdesc_vec_vec_grp(vector* v, vector* w, object g); /* implemented */
object int_bhleq_vec_vec_grp(vector* v, vector* w, object g); /* implemented */
object vec_canonical_vec_grp(vector* v, object g);   /* implemented */
object mat_canonical_mat_grp(matrix* m, object g);   /* not needed */
object vec_dominant_vec_grp(vector* v, object g);    /* implemented */
object mat_dominant_mat_grp(matrix* m, object g);    /* not needed */
object pol_dominant_pol_grp(poly* p, object g);      /* implemented */
object vec_exponents_grp(object g);                  /* implemented */
object mat_filterdom_mat (matrix* m, object g);      /* not needed */
object pol_filterdom_pol (poly* p, object g);        /* implemented */
object pol_klpoly_vec_vec_grp(vector* x, vector* y, object g);  /* implemented */
object int_length_vec_grp(vector* v, object g);      /* implemented */
object vec_longword_grp(object g);                   /* implemented - partial*/
object vec_lreduce_vec_vec_grp(vector* L,vector* ww,object g); /* implemented */
/*object vec_lrreduce_vec_vec_vec_grp(L,ww,R,g); ???? */
object mat_orbit_int_vec_mat(entry i, vector* v, matrix* m); 
object mat_orbit_vec_mat(vector* v, matrix* m);
object vec_reduce_vec_grp(vector* ww, object g);     /* implemented */
object mat_reflection_vec_grp(vector* rt, object g); /* implemented */
object vec_rreduce_vec_vec_grp(vector* ww, vector* R, object g); /* implemented */
object pol_rpoly_vec_vec_grp(vector* x, vector* y, object g); /* implemented */
object vec_waction_vec_vec_grp(vector* v, vector* word, object g); /* not needed? */
object mat_waction_vec_grp(vector* word, object g);  /* implemented */
object mat_waction_mat_vec_grp(matrix* m, vector* word, object g); /* not needed??? */
object pol_waction_pol_vec_grp(poly* p, vector* word, object g); /* not needed??? */
object mat_worbit_vec_grp(vector* v, object g);      /* implemented */
object mat_worbitgraph_vec_grp(vector* v, object g);
object pol_worbit_pol_grp(poly* p, object g);        /* implemented */
object bin_worbitsize_vec_grp(vector* v, object g);  /* implemented */
object bin_worder_grp(object g);                     /* implemented */
object bin_worder_vec_grp(object I, object g);       /* implemented */
object vec_wrtaction_vec_vec_grp(vector* v, vector* word, object g);
object mat_wrtaction_vec_grp(vector* word, object g);
object mat_wrtaction_mat_vec_grp(matrix* m, vector* word, object g);
object mat_wrtorbit_vec_grp(vector* v, object g);
object vec_wword_vec_grp(vector* w, object g);
object vec_wword_mat_grp(matrix* m, object g);
object bin_classord_vec(vector* lambda);
object vec_frompart_vec(vector* lambda);
object mat_frompart_mat(matrix* m);
object pol_frompart_pol(poly* p);
object vec_nextpart_vec(vector* v);
object vec_nextperm_vec(vector* v);
object vec_nexttab_vec(vector* t);
object bin_ntabl_vec(vector* lambda);
object mat_partitions_int(entry p);
object vid_prtab_vec(vector* v);
object vec_RS_vec_vec(vector* P, vector* Q);
object mat_RS_vec(vector* W);
object int_signpart_vec(vector* v);
object vec_shape_vec(vector* v);
object bin_symchar_vec_vec(vector* a, vector* b);
object poly_symchar_vec(vector* a);
object mat_symorbit_vec(vector* v);
object mat_tableaux_vec(vector* lambda);
object vec_topart_vec(vector* v);
object mat_topart_mat(matrix* m);
object pol_topart_pol(poly* p);
object vec_transpart_vec(vector* v);
object pol_adams_int_vec_grp(entry d, vector* v, object g);
object pol_adams_int_pol_grp(entry d, poly* p, object g);           /* implemented */
object pol_adjoint_grp(object g);                                   /* implemented */
object pol_altdom_pol_grp(poly* p, object g);                       /* implemented */
object pol_altdom_vec_grp(vector* v, object g);                     /* not needed */
object pol_altdom_pol_vec_grp(poly* p, vector* w, object g);        /* implemented */
object pol_altdom_vec_vec_grp(vector* v, vector* w, object g);      /* not needed */
object pol_alttensor_int_pol_grp(entry d, poly* p, object g);       /* implemented */
object pol_alttensor_int_vec_grp(entry d, vector* v, object g);     /* implemented */
object pol_altwsum_pol_grp(poly* p, object g);                      /* implemented */
object pol_altwsum_vec_grp(vector* v, object g);                    /* implemented */
object pol_branch_vec_grp_mat_grp(vector *v, object h, object m, object g); /* not needed */
object pol_branch_pol_grp_mat_grp(poly *p, object h, object m, object g); /* implemented */
object pol_collect_pol_grp_mat_grp(poly* p, matrix* m, object h, object g);
/*object pol_collect_pol_grp_mat_int_grp()*/
object vec_contragr_vec_grp(vector* v, object g);                   /* not needed */
object pol_contragr_pol_grp(poly* p, object g);                     /* implemented */
object pol_decomp_pol_grp(poly* p, object g);                       /* implemented */
object pol_Demazure_pol_vec_grp(poly* p, vector* w, object g);
object pol_Demazure_vec_vec_grp(vector* v, vector* w, object g);
object pol_Demazure_pol_grp(poly* p, object g);
object pol_Demazure_vec_grp(vector* v, object g);
object bin_dim_vec_grp(vector* v, object g);                        /* not needed */
object bin_dim_pol_grp(poly* p, object g);                          /* implemented */
object pol_domchar_vec_grp(vector* v, object g);                    /* not needed */
object pol_domchar_pol_grp(poly* p, object g);                      /* implemented */
object bin_domchar_vec_vec_grp(vector* lambda, vector* w, object g);/* not needed */
object bin_domchar_pol_vec_grp(poly* p, vector* w, object g);       /* implemented */
object pol_lrtensor_vec_vec(vector* lambda, vector* mu);
object pol_lrtensor_pol_pol(poly* p, poly* q);
object pol_plethysm_vec_pol_grp(vector* lambda, poly* p, object g);
object pol_plethysm_vec_vec_grp(vector* lambda,vector* mu,object g);
object pol_ptensor_int_pol_grp(entry d, poly* p, object g);         /* implemented */
object pol_ptensor_int_vec_grp(entry d, vector* v, object g);       /* implemented */
object mat_resmat_mat_grp(matrix* m, object g);                     /* implemented */
object pol_spectrum_vec_vec_grp (vector* wt, vector* t, object g);  /* not needed */
object pol_spectrum_pol_vec_grp (poly* p, vector* t, object g);     /* implemented */
object pol_symtensor_int_pol_grp(entry d, poly* p, object g);       /* implemented */
object pol_symtensor_int_vec_grp(entry d, vector* v, object g);     /* implemented */
object pol_tensor_pol_pol_grp(poly* p, poly* q, object g);          /* implemented */
object pol_tensor_vec_vec_grp(vector* x, vector* y, object g);      /* implemented */
object bin_tensor_pol_pol_vec_grp(poly* p, poly* q, vector* nu, object g); /* implemented */
object bin_tensor_vec_vec_vec_grp(vector* x, vector* y, vector* z, object g); /* not needed */
object pol_vdecomp_pol_grp(poly* p, object g);                      /* implemented */
object pol_vdecomp_vec_grp(vector* v, object g);                    /* not needed */

/******** from error.c *******/
/*void error(char *format, ...);*/ /* already in lie.h */


/******** from mem.c *******/
void setpyobj(object obj);
void clrpyobj(object obj);

