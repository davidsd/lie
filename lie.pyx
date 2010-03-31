from fractions import Fraction, gcd
from functools import reduce
import signal

cdef extern from "lie-py.h":

    # from Python.h
    object PyLong_FromString(char *str, char **pend, int base)

    # from error.c, replacing output.c
    void error(char *format, ...)

    # from lie.h

    ctypedef long index
    ctypedef long entry
    ctypedef int boolean
    ctypedef int cmp_tp
    ctypedef cmp_tp (*cmpfn_tp) (entry*,entry*,index)
    ctypedef char* string
    
    # memtype.h
    ctypedef unsigned short objtype
    ctypedef unsigned short reftype
    ctypedef struct intcel:
        entry intval
    ctypedef struct bigint:
        pass
    ctypedef struct vector:
        index ncomp
        entry *compon
    ctypedef struct matrix:
        index nrows
        index ncols
        entry **elm
    ctypedef struct poly:
        index nrows
        index ncols
        entry **elm
        bigint **coef
        
    ctypedef struct simpgrp_struct:
        pass
    ctypedef simpgrp_struct simpgrp
    ctypedef struct group:
        index ncomp, toraldim
        simpgrp **liecomp
    ctypedef struct tekst:
        char *string

    cdef struct any_struct:
        objtype type
        reftype nref
    ctypedef union objcel:
        any_struct any
    ctypedef objcel *lieobj "object"

    extern unsigned long  maxnodes, maxptrs, gccrit, maxenters, maxlabels

    # from gc.c, replacing gc() in mem.c
    void simple_gc()
    void c_print_objs()
    void gc_print_obj(lieobj o)


    # nodetype.h
    ctypedef unsigned int strtype
    cdef struct labelrec:
        strtype name
        int line
        char *fname
    ctypedef labelrec *labeltp
    labeltp label_null
    labeltp label

    # mem.h
    void **ptr
    group* mkgroup(index ncomp)
    vector* mkvector(index n)
    matrix* mkmatrix(index r, index c)
    poly* mkpoly(index r,index c)
    # actually macros
    void setshared(lieobj x)
    void clrshared(lieobj x)
    int islonglife(lieobj x)
    int issorted(lieobj x)
    void freem(void* x)
    void freemem(lieobj x)
    void freepol(poly* p)
    void LiE_gc "gc" ()

    # lie-py extensions
    void setpyobj(lieobj x)
    void clrpyobj(lieobj x)

    # main.h
    int redirected_input # actually a bool
    labeltp label

    # init.h
    intcel* bool_false
    intcel* bool_true
    poly* poly_null(index r)
    void init()

    # grpdata.h
    boolean simpgroup(lieobj g)
    vector* Simproot_norms(lieobj grp)
    index Lierank(lieobj grp)
    vector* Exponents(lieobj grp)
    index Numproots(lieobj grp)
    
    entry Dimgrp(lieobj grp)
    matrix* Center(lieobj grp)
    
    #index find_root(entry* alpha, entry level, simpgrp* g)
    
    # symg.h
    bigint* n_tableaux(entry* lam, index l)
    bigint* Classord(entry* kappa, index l)
    index check_part(entry* lam, index l)
    vector* check_tabl(vector* v)
    boolean Nextperm(entry* w, index n)
    boolean Nextpart(entry* lam, index l)
    boolean Nexttableau(entry* t, index n)
    matrix* Permutations(entry* v,index n)
    matrix* Partitions(index n)
    matrix* Tableaux(entry* lam, index l)
    vector* Trans_part(entry* lam, index l)
    entry Sign_part(entry* lam, index l)
    void Robinson_Schensted (entry* P, entry* Q, index n, entry* sigma)
    void Schensted_Robinson (entry* sigma, index n, entry* P, entry* Q)
    poly* MN_char(entry* lam, index l)
    bigint* MN_char_val(entry* lam, entry* mu, index l, index m)
    
    # weylloop.h
    void Weylloopinit(simpgrp* g)
    void Weylloop(void (*action)(entry*),entry* v)
    void Weylloopexit()

    # /* print.c */ from ansi.h
    char* grp2str(group* grp)

    # poly.c from ansi.h
    poly* Add_pol_pol(poly* a, poly* b, int sign)
    entry Degree_pol(poly* p)
    poly* Mul_bin_pol(bigint* a, poly* b)
    poly* Mul_pol_pol(poly* a, poly* b)
    poly* Div_pol_bin(poly* a, bigint* b)
    poly* Div_pol_vec(poly* a, vector* b)
    poly* Mod_pol_bin(poly* a, bigint* b)
    poly* Mod_pol_vec(poly* a, vector* b)
    
    # ansi.h
    # bigints
    bigint* str2bigint(string s)
    string bigint2str(bigint* num)

    # /* box/diagram.c */
    lieobj Diagram(lieobj grp)
    
    # /* box/matrix.c */
    void copyrow(entry* v,entry* w,index n) # /* ubiquitous, as are following: */
    #void addrow(entry* v,entry* w,entry* x,index n)
    #void subrow(entry* v,entry* w,entry* x,index n)
    #boolean pos_subrow(entry* v,entry* w,entry* x,index n)
    matrix* extendmat(matrix* old)
    matrix* copymatrix(matrix* old)
    
    poly* Reduce_pol(poly* p)
    
    # /* static/static1.c */
    matrix* mat_id(entry size)
    lieobj mat_add_mat_vec(lieobj m, lieobj v)
    lieobj mat_add_mat_mat(lieobj m, lieobj v)
    lieobj vec_add_vec_vec(lieobj m, lieobj v)
    
    # /* static/static2.c */
    lieobj vec_addelm_vec_int(lieobj v, lieobj i)
    
    # /* box/static3.c */
    lieobj int_eq_grp_grp(lieobj g, lieobj h) # /* used in decomp */
    
    # from c-helpers/static1.c

    cdef long chunks
    vector* vec_min_vec(vector* a)
    matrix* mat_min_mat(matrix* a)
    vector* vec_mod_vec_int(vector* a, entry b)
    matrix* mat_div_mat_int(matrix* a, entry b)
    matrix* mat_mod_mat_int(matrix* a, entry b)
    vector* vec_mul_int_vec(entry a, vector *b)
    vector* vec_div_vec_int(vector* a, entry b)
    vector* vec_mul_mat_vec(matrix *a, vector *b)
    vector* vec_mul_vec_mat(vector *v, matrix *m)
    vector* vec_append_vec_vec(vector *v, vector *w)
    matrix* mat_mul_mat_mat(matrix *a, matrix *b)
    poly* pol_mul_pol_mat(poly *a, matrix *b)
    matrix* mat_append_mat_mat(matrix* a, matrix* b)
    matrix* mat_transpose_mat(matrix *a) # /* replace with Transpose? */
    index int_mul_vec_vec(vector *a, vector *b)
    lieobj mat_sub_mat_int(lieobj a, entry obj_k)
    vector *vec_dif_vec_vec(vector *a, vector *b)
    matrix* mat_sub_mat_mat(matrix *a, matrix *b)
    matrix* mat_mul_int_mat(entry a, matrix *b)

    # from c-helpers/static2.c

    lieobj int_eq_mat_mat(lieobj a, lieobj b)
    lieobj int_eq_pol_pol(lieobj a, lieobj b)
    lieobj int_ne_mat_mat(lieobj a, lieobj b)
    lieobj int_ne_pol_pol(lieobj a, lieobj b)
    lieobj int_eq_vec_vec(lieobj a, lieobj b)
    lieobj int_ne_vec_vec(lieobj a, lieobj b)
    lieobj vec_startaddelm_int_vec(entry a, lieobj b)
    lieobj vec_subelm_vec_int(lieobj a, entry b)
    lieobj vec_select_mat_int(lieobj m, entry indexobj)
    lieobj vec_select_pol_int(poly *m, entry indexobj)
    lieobj bin_select_pol_vec(poly *p, vector *v)
    lieobj Int_search_mat_vec(matrix* m, vector* v, index low, index up)
    lieobj int_search_mat_vec(matrix *m, vector *v)
    lieobj int_search_mat_vec_int_int(matrix *m, vector *v, entry up, entry low)
    lieobj int_select_mat_int_int(matrix *m, entry n1, entry n2)
    lieobj vec_join_mat(lieobj a)
    lieobj vec_diag_mat(lieobj a)
    poly* pol_monom_vec(vector *v)
    poly* pol_polynom_mat(matrix* p)
    poly* pol_polynom_int(entry k)
    matrix* mat_polynom_pol(poly *p)
    lieobj pol_polynom_bin_vec(bigint *d, vector *v)
    lieobj pol_polynom_vec(vector *v)
    lieobj pol_min_pol(poly	*a)
    lieobj pol_atensor_pol_pol(lieobj p1, lieobj p2)

    lieobj tex_maxsub_grp(group *g)
    group* create_group(string s)
    lieobj grp_maxsub_grp_int(group *g, entry nobj)
    lieobj mat_resmat_grp_grp(group *g1, group *g2)
    lieobj mat_resmat_grp_int_grp(group *g1,entry d, group *g2)

    # from c-helpers/static3.c

    # /* Group operations */
    void check_wt(vector* lam, index r)
    void check_wts(matrix* m, index r)
    void check_rt(vector* rt, index s)
    void check_rts(matrix* m, index s)
    void check_toral(vector* t, index r, index lim)
    void check_torals(matrix* m, index r)
    void check_Ww(vector* ww, index s)
    void check_Wws(matrix* m, index s)
    lieobj grp_mul_grp_grp(lieobj g1, lieobj g2)
    lieobj grp_select_grp_int(lieobj g, entry n)
    lieobj vec_sort_vec(lieobj v)
    lieobj mat_sort_mat(matrix* m)
    lieobj mat_unique_mat(matrix* m)
    lieobj mat_blockmat_mat_mat(matrix* a,matrix* b)
    lieobj vid_setdefault_grp(lieobj g)
    lieobj vid_setdefault()
    lieobj grp_setdefault()
    lieobj mat_center_grp(lieobj g)
    lieobj int_dim_grp(lieobj g)
    lieobj vid_diagram_grp(lieobj g)
    lieobj vec_liecode_grp(lieobj g)
    lieobj groupmake(char lietype,index rank)
    lieobj grp_liegroup_int_int(entry typ, entry rank)
    # /*lieobj int_ncomp_grp(group* g)*/
    lieobj int_lierank_grp(lieobj g)
    lieobj int_cartan_vec_vec_grp(vector* v, vector* w, lieobj g)
    lieobj mat_cartan_grp(lieobj g)
    lieobj grp_carttype_mat_grp(matrix* m, lieobj g)
    lieobj mat_centroots_vec_grp(vector* t, lieobj g)
    lieobj mat_centroots_mat_grp(matrix* m, lieobj g)
    lieobj grp_centrtype_vec_grp(vector* t, lieobj g)
    lieobj grp_centrtype_mat_grp(matrix* m, lieobj g)
    lieobj mat_closure_mat_grp(matrix* m, lieobj g) # from box/static3.c
    lieobj int_detcart_grp(lieobj g)
    lieobj mat_domweights_vec_grp(vector* v, lieobj g)
    lieobj mat_fundam_mat_grp(matrix* m, lieobj g) # from box/static3.c
    lieobj vec_highroot_grp(lieobj grp)
    lieobj mat_icartan_grp(lieobj g)
    lieobj int_inprod_vec_vec_grp(vector* v, vector* w, lieobj g)
    lieobj int_norm_vec_grp(vector* root, lieobj g)
    lieobj int_numproots_grp(lieobj g)
    lieobj mat_posroots_grp(lieobj g)
    lieobj mat_bhdesc_vec_grp(vector* w, lieobj g)
    lieobj mat_bhdesc_vec_vec_grp(vector* v, vector* w, lieobj g)
    lieobj int_bhleq_vec_vec_grp(vector* v, vector* w, lieobj g)
    lieobj vec_canonical_vec_grp(vector* v, lieobj g)
    lieobj mat_canonical_mat_grp(matrix* m, lieobj g)
    lieobj vec_dominant_vec_grp(vector* v, lieobj g)
    lieobj mat_dominant_mat_grp(matrix* m, lieobj g)
    lieobj pol_dominant_pol_grp(poly* p, lieobj g)
    lieobj vec_exponents_grp(lieobj g)
    lieobj mat_filterdom_mat (matrix* m, lieobj g)
    lieobj pol_filterdom_pol (poly* p, lieobj g)
    lieobj pol_klpoly_vec_vec_grp(vector* x, vector* y, lieobj g)
    lieobj int_length_vec_grp(vector* v, lieobj g)
    lieobj vec_longword_grp(lieobj g)
    lieobj vec_lreduce_vec_vec_grp(vector* L,vector* ww,lieobj g)
    # /*lieobj vec_lrreduce_vec_vec_vec_grp(L,ww,R,g) ???? */
    lieobj mat_orbit_int_vec_mat(entry i, vector* v, matrix* m)
    lieobj mat_orbit_vec_mat(vector* v, matrix* m)
    lieobj vec_reduce_vec_grp(vector* ww, lieobj g)
    lieobj mat_reflection_vec_grp(vector* rt, lieobj g)
    lieobj vec_rreduce_vec_vec_grp(vector* ww, vector* R, lieobj g)
    lieobj pol_rpoly_vec_vec_grp(vector* x, vector* y, lieobj g)
    lieobj vec_waction_vec_vec_grp(vector* v, vector* word, lieobj g)
    lieobj mat_waction_vec_grp(vector* word, lieobj g)
    lieobj mat_waction_mat_vec_grp(matrix* m, vector* word, lieobj g)
    lieobj pol_waction_pol_vec_grp(poly* p, vector* word, lieobj g)
    lieobj mat_worbit_vec_grp(vector* v, lieobj g)
    lieobj mat_worbitgraph_vec_grp(vector* v, lieobj g)
    lieobj pol_worbit_pol_grp(poly* p, lieobj g)
    lieobj bin_worbitsize_vec_grp(vector* v, lieobj g)
    lieobj bin_worder_grp(lieobj g)
    lieobj bin_worder_vec_grp(lieobj I, lieobj g)
    lieobj vec_wrtaction_vec_vec_grp(vector* v, vector* word, lieobj g)
    lieobj mat_wrtaction_vec_grp(vector* word, lieobj g)
    lieobj mat_wrtaction_mat_vec_grp(matrix* m, vector* word, lieobj g)
    lieobj mat_wrtorbit_vec_grp(vector* v, lieobj g)
    lieobj vec_wword_vec_grp(vector* w, lieobj g)
    lieobj vec_wword_mat_grp(matrix* m, lieobj g)
    lieobj bin_classord_vec(vector* lam)
    lieobj vec_frompart_vec(vector* lam)
    lieobj mat_frompart_mat(matrix* m)
    lieobj pol_frompart_pol(poly* p)
    lieobj vec_nextpart_vec(vector* v)
    lieobj vec_nextperm_vec(vector* v)
    lieobj vec_nexttab_vec(vector* t)
    lieobj bin_ntabl_vec(vector* lam)
    lieobj mat_partitions_int(entry p)
    lieobj vid_prtab_vec(vector* v)
    lieobj vec_RS_vec_vec(vector* P, vector* Q)
    lieobj mat_RS_vec(vector* W)
    lieobj int_signpart_vec(vector* v)
    lieobj vec_shape_vec(vector* v)
    lieobj bin_symchar_vec_vec(vector* a, vector* b)
    lieobj poly_symchar_vec(vector* a)
    lieobj mat_symorbit_vec(vector* v)
    lieobj mat_tableaux_vec(vector* lam)
    lieobj vec_topart_vec(vector* v)
    lieobj mat_topart_mat(matrix* m)
    lieobj pol_topart_pol(poly* p)
    lieobj vec_transpart_vec(vector* v)
    lieobj pol_adams_int_vec_grp(entry d, vector* v, lieobj g)
    lieobj pol_adams_int_pol_grp(entry d, poly* p, lieobj g)
    lieobj pol_adjoint_grp(lieobj g)
    lieobj pol_altdom_pol_grp(poly* p, lieobj g)
    lieobj pol_altdom_vec_grp(vector* v, lieobj g)
    lieobj pol_altdom_pol_vec_grp(poly* p, vector* w, lieobj g)
    lieobj pol_altdom_vec_vec_grp(vector* v, vector* w, lieobj g)
    lieobj pol_alttensor_int_pol_grp(entry d, poly* p, lieobj g)
    lieobj pol_alttensor_int_vec_grp(entry d, vector* v, lieobj g)
    lieobj pol_altwsum_pol_grp(poly* p, lieobj g)
    lieobj pol_altwsum_vec_grp(vector* v, lieobj g)
    lieobj pol_branch_vec_grp_mat_grp(vector *v, lieobj h, lieobj m, lieobj g)
    lieobj pol_branch_pol_grp_mat_grp(poly *p, lieobj h, lieobj m, lieobj g)
    lieobj pol_collect_pol_grp_mat_grp(poly* p, matrix* m, lieobj h, lieobj g)
    # /*lieobj pol_collect_pol_grp_mat_int_grp()*/
    lieobj vec_contragr_vec_grp(vector* v, lieobj g)
    lieobj pol_contragr_pol_grp(poly* p, lieobj g)
    lieobj pol_decomp_pol_grp(poly* p, lieobj g)
    lieobj pol_Demazure_pol_vec_grp(poly* p, vector* w, lieobj g)
    lieobj pol_Demazure_vec_vec_grp(vector* v, vector* w, lieobj g)
    lieobj pol_Demazure_pol_grp(poly* p, lieobj g)
    lieobj pol_Demazure_vec_grp(vector* v, lieobj g)
    lieobj bin_dim_vec_grp(vector* v, lieobj g)
    lieobj bin_dim_pol_grp(poly* p, lieobj g)
    lieobj pol_domchar_vec_grp(vector* v, lieobj g)
    lieobj pol_domchar_pol_grp(poly* p, lieobj g)
    lieobj bin_domchar_vec_vec_grp(vector* lam, vector* w, lieobj g)
    lieobj bin_domchar_pol_vec_grp(poly* p, vector* w, lieobj g)
    lieobj pol_lrtensor_vec_vec(vector* lam, vector* mu)
    lieobj pol_lrtensor_pol_pol(poly* p, poly* q)
    lieobj pol_plethysm_vec_pol_grp(vector* lam, poly* p, lieobj g)
    lieobj pol_plethysm_vec_vec_grp(vector* lam,vector* mu,lieobj g)
    lieobj pol_ptensor_int_pol_grp(entry d, poly* p, lieobj g)
    lieobj pol_ptensor_int_vec_grp(entry d, vector* v, lieobj g)
    lieobj mat_resmat_mat_grp(matrix* m, lieobj g)
    lieobj pol_spectrum_vec_vec_grp (vector* wt, vector* t, lieobj g)
    lieobj pol_spectrum_pol_vec_grp (poly* p, vector* t, lieobj g)
    lieobj pol_symtensor_int_pol_grp(entry d, poly* p, lieobj g)
    lieobj pol_symtensor_int_vec_grp(entry d, vector* v, lieobj g)
    lieobj pol_tensor_pol_pol_grp(poly* p, poly* q, lieobj g)
    lieobj pol_tensor_vec_vec_grp(vector* x, vector* y, lieobj g)
    lieobj bin_tensor_pol_pol_vec_grp(poly* p, poly* q, vector* nu, lieobj g)
    lieobj bin_tensor_vec_vec_vec_grp(vector* x, vector* y, vector* z, lieobj g)
    lieobj pol_vdecomp_pol_grp(poly* p, lieobj g)
    lieobj pol_vdecomp_vec_grp(vector* v, lieobj g)

# utilities

def lcm(a,b): return (a*b)//gcd(a,b)

def denom(x):
    if isinstance(x, Fraction): return x.denominator
    else: return 1

def powermod(x, n, b):
    if n <= 0: raise ValueError(u"Cannot raise object to negative power.")
    elif n == 1: return x % b
    y = power(x, n//2)
    if n % 2 == 1: return x*y*y % b
    else: return y*y % b
    
def power(x, n):
    if n <= 0: raise ValueError(u"Cannot raise object to negative power.")
    elif n == 1: return x
    y = power(x, n//2)
    if n % 2 == 1: return x*y*y
    else: return y*y

cdef class grp:
    cdef lieobj g

    def __init__(self, val=u"T0", rk=None, *args):
        if isinstance(val, grp):
            self.g = (<grp>val).g
        elif type(val) == type(0) and type(rk) == type(0):
            self.g = grp_liegroup_int_int(<int>val, <int>rk)
        elif val:
            self.g = <lieobj>create_group(pyobj2charptr(val))
        protect(self.g)

    def __dealloc__(self):
        neglect(<lieobj>self.g)
        gc.maybe_run()

    def __str__(self): return repr(self)
    def __repr__(self): return grp2str(<group*> self.g).decode(u"utf-8")
    def __hash__(self): return hash(repr(self))

    def __richcmp__(grp self, grp other, int op):
        if op == 0: return NotImplemented
        elif op == 2: return bool_from_lieobj(int_eq_grp_grp(self.g, other.g))
        elif op == 4: return NotImplemented
        elif op == 1: return NotImplemented
        elif op == 3: return not (self == other)
        elif op == 5: return NotImplemented
        else: raise ValueError(u"Bad op argument to grp.__richcmp__")

    def __len__(self):
        if self.toral_dim() == 0: return (<group*>(self.g)).ncomp
        else: return 1 + (<group*>(self.g)).ncomp

    def __getitem__(grp self, key):
        if isinstance(key, slice):
            i,j,s = key.indices(len(self))
            return reduce(grp.__mul__, [self[k] for k in range(i, j, s)], T0)
        else:
            n = len(self)
            m = 1 if self.toral_dim() == 0 else 0
            i,_,_ = slice(key, None).indices(n)
            if abs(i) >= n: raise IndexError(u"grp index out of range")
            return grp_from_lieobj(grp_select_grp_int(self.g, (i+1) % (n+m)))
    def to_list(self): return [self[i] for i in range(len(self))]
    def __iter__(self): return iter(self.to_list())

    def __mul__(grp self, grp other):
        return grp_from_lieobj(grp_mul_grp_grp(self.g, other.g))
    def __pow__(self, n, b):
        if b: return powermod(self, n, b)
        else: return power(self, n)

    def toral_dim(self): return (<group*>(self.g)).toraldim
    def toral_part(self): return grp(0, self.toral_dim())
    def ss_dim(self): return (<group*>self.g).ncomp
    def dim(self): return Dimgrp(self.g)
    def rank(self): return Lierank(self.g)
    def num_proots(self): return Numproots(self.g)
    def diagram(self): Diagram(self.g)
    def w_order(self): return lieobj2pyint(bin_worder_grp(self.g))
    def det_cartan(self): return int_from_lieobj(int_detcart_grp(self.g))
    def lie_code(self):
        v = vec_from_lieobj(vec_liecode_grp(self.g))
        return (v[0], v[1])
    def pos_roots(self):
        return [self.root(w) for w in mat_from_lieobj(mat_posroots_grp(self.g))]
    def center(self): return [self.toral(t, fromlie=True) \
                              for t in mat_from_liematrix(Center(self.g))]
    def simproot_norms(self): return vec_from_lievector(Simproot_norms(self.g))
    def exponents(self): return vec_from_lievector(Exponents(self.g))
    def highroot(self): return vec_from_lieobj(vec_highroot_grp(self.g))
    def cartan(self):   return mat_from_lieobj(mat_cartan_grp(self.g))
    def inv_cartan(self):  return mat_from_lieobj(mat_icartan_grp(self.g))
    def longword(self): return vec_from_lieobj(vec_longword_grp(self.g))
    def max_subgrps(self):
        s = str_from_lieobj(tex_maxsub_grp(<group*>self.g))
        return [grp(x.decode(u"utf-8")) for x in s.split(',')]
    def max_subgrp(self, i): return self.max_subgrps()[i]
    def res_mat(self, val, i=None):
        if isinstance(val, grp) and i == None:
            return mat_from_lieobj(mat_resmat_grp_grp( \
                    <group*>(<grp>val).g, <group*>self.g))
        elif isinstance(val, grp):
            return mat_from_lieobj(mat_resmat_grp_int_grp( \
                    <group*>(<grp>val).g, int(i), <group*>self.g))
        else: return mat_from_lieobj(mat_resmat_mat_grp((<mat>mat(val)).m, self.g))
    
    def simple(self): return simpgroup(self.g) != 0
    def rep(self, x): return rep(x, self)
    def weyl(self, rfls): return weyl(rfls, self)
    def root(self, coords): return AbstractVec(coords, AbstractVec.ROOT, self)
    def weight(self, coords): return AbstractVec(coords, AbstractVec.WEIGHT, self)
    def toral(self, coords, order=1, fromlie=False): return toral(coords, self, order, fromlie)
    def adjoint(grp self): return self.rep(pol_from_lieobj(pol_adjoint_grp(self.g)))
    def trivial(self): return self.rep(self.rank()*[0])
    def fund(self): return self.rep([1] + (self.rank()-1)*[0])
    def alt_tensor(self, n): return self.fund().alt_tensor(n)
    def sym_tensor(self, n): return self.fund().sym_tensor(n)
    def rho(self):
        t = self.toral_dim()
        return self.weight((self.rank() - t)*[1] + t*[0])

    def factor_res_mat(self, i):
        r = self[i].rank()
        if isinstance(i, slice): i = i.indices(len(self))[0]
        br = self[:i].rank()
        return mat([[int(n == m) for n in range(br, br+r)] for m in range(self.rank())])

    def pprint(self):
        s = []
        for x in self:
            ty, rk = x.lie_code()
            if   ty == 0: s.append(u"U(1)" + (u"^%d" % rk if rk!=1 else u""))
            elif ty == 1: s.append(u"SU(%d)" % (rk + 1))
            elif ty == 2: s.append(u"SO(%d)" % (2*rk + 1))
            elif ty == 3: s.append(u"Sp(2*%d)" % rk)
            elif ty == 4: s.append(u"SO(%d)" % (2*rk))
            else: s.append(repr(x))
        return u"x".join(s)

cdef class vec:
    cdef vector* v

    def __init__(self, vals, *args):
        if isinstance(vals, vec): self.v = (<vec>vals).v
        if isinstance(vals, rep): self.v = (<vec>vals.hw().numerator).v
        if isinstance(vals, AbstractVec) and vals.denominator == 1:
            self.v = (<vec> vals.numerator).v
        else:
            self.v = mkvector(len(vals))
            for 0 <= i < len(vals):
                self.v.compon[i] = int(vals[i])
        protect(<lieobj>self.v)

    def __dealloc__(self):
        neglect(<lieobj>self.v)
        gc.maybe_run()

    def __len__(self):
        return self.v.ncomp # same as self.v.size

    def to_list(self):
        return [self.v.compon[i] for i in range(len(self))]

    #def __hash__(self): return hash(repr(self))
    
    # could be optimized much more
    def __richcmp__(vec self, vec other, int op):
        if op == 0: return self.to_list() < other.to_list()
        elif op == 1: return self.to_list() <= other.to_list()
        elif op == 2: return bool_from_lieobj(int_eq_vec_vec(<lieobj>self.v, <lieobj>other.v))
        elif op == 3: return bool_from_lieobj(int_ne_vec_vec(<lieobj>self.v, <lieobj>other.v))
        elif op == 4: return not (self <= other)
        elif op == 5: return not (self < other)
        else: raise ValueError(u"Bad op argument to vec.__richcmp__")

    def __iter__(self):
        return iter(self.to_list())
    def __getitem__(self, key):
        if isinstance(key, slice):
            i,j,_ = key.indices(len(self))
            return vec([self.v.compon[k] for k in range(i, j)])
        else:
            i,_,_ = slice(key, None).indices(len(self))
            if i >= len(self): raise IndexError(u"vec index out of range")
            return self.v.compon[int(i)]
    def __setitem__(vec self, int key, val):
        cdef vector* oldv
        if islonglife(<lieobj>self.v): # can't modify persistant vector. make a copy.
            oldv = self.v
            self.v = mkvector(oldv.ncomp)
            copyrow(oldv.compon, self.v.compon, oldv.ncomp)
            protect(<lieobj>self.v)
        self.v.compon[key] = int(val)
    def __delitem__(vec self, int i):
        cdef vector* oldv = self.v
        self.v = <vector*>vec_subelm_vec_int(<lieobj>oldv, i + 1) # LiE doesn't use zero-indexing
        neglect(<lieobj>oldv)
        protect(<lieobj>self.v)
    def __repr__(self):
        return u"<" + u", ".join([repr(x) for x in self]) + u">"
    def __str__(self): return repr(self)

    def __mod__(vec self, entry n):
        return vec_from_lievector(vec_mod_vec_int(self.v, n))
    
    def __mul__(self, other):
        if isinstance(self, vec) and type(other) == type(0):
            return vec_from_lievector(vec_mul_int_vec(<int>other, (<vec>self).v))
        elif isinstance(self, vec) and isinstance(other, vec):
            return int_mul_vec_vec((<vec>self).v, (<vec>other).v)
        elif isinstance(self, vec) and isinstance(other, mat):
            return vec_from_lievector(vec_mul_vec_mat((<vec>self).v, (<mat>other).m))
        else: return vec.__mul__(other, self)
    
    def __truediv__(vec self, entry n):
        return vec_from_lievector(vec_div_vec_int(self.v, n))
    def __add__(vec self, vec other):
        cdef lieobj o = <lieobj>(self.v)
        return vec_from_lieobj(vec_add_vec_vec(o, <lieobj>(other.v)))
    def __sub__(vec self, vec other):
        return vec_from_lievector(vec_dif_vec_vec(self.v, other.v))

    def __neg__(vec self):
        return vec_from_lievector(vec_min_vec(self.v))

    def frac(self, n):
        g = reduce(gcd, self, n)
        return self/g, n//g
    def concat(vec self, vec other):
        return vec_from_lievector(vec_append_vec_vec(self.v, other.v))


cdef class mat:
    cdef matrix* m

    def __init__(self, val, *args):
        if isinstance(val, mat): self.m = (<mat>val).m
        elif isinstance(val, pol): self.m = mat_polynom_pol((<pol>val).p)
        elif len(val) == 0 or len(val[0]) == 0: raise ValueError(u"Bad argument to mat().")
        else:
            r, c = len(val), len(val[0])
            self.m = mkmatrix(r, c)
            for 0 <= i < r:
                for 0 <= j < c:
                    self.m.elm[i][j] = int(val[i][j])
        protect(<lieobj>(<mat> self).m)

    def __dealloc__(self):
        neglect(<lieobj>self.m)
        gc.maybe_run()
                        
    def ncols(self): return self.m.ncols
    def nrows(self): return self.m.nrows
    def __len__(self): return self.m.nrows

    #def __hash__(self): return hash(repr(self))
    
    def rows(self): return [self[i] for i in range(len(self))]
    
    def __iter__(self): return iter(self.rows())
    def __getitem__(self, key):
        cdef mat M
        if isinstance(key, slice):
            i,j,_ = key.indices(len(self))
            M = NEW_MAT(mat)
            M.m = mkmatrix(j-i, self.m.ncols)
            for 0 <= k < j-i:
                copyrow(self.m.elm[i+k], M.m.elm[k], self.m.ncols)
            protect(<lieobj>M.m)
            return M
        else:
            i,_,_ = slice(key, None).indices(len(self))
            return vec_from_lieobj(vec_select_mat_int(<lieobj>(<mat>self).m, i+1))

    def __delitem__(self, i):
        cdef matrix* oldm = self.m
        neglect(<lieobj>oldm)
        self.m = <matrix*>mat_sub_mat_int(<lieobj>oldm, i + 1) # LiE doesn't use zero-indexing
        protect(<lieobj>self.m)
    # def __setitem__(self, i, val):
    # no __setitem__ for matrices, since for example cartan matrices are
    # persistent objects, and shouldn't be modified def

    # could be optimized much more
    def __richcmp__(mat self, mat other, int op):
        if op == 0: return self.rows() < other.rows()
        elif op == 1: return self.rows() <= other.rows()
        elif op == 2: return bool_from_lieobj(int_eq_mat_mat(<lieobj>self.m, <lieobj>other.m))
        elif op == 3: return bool_from_lieobj(int_ne_mat_mat(<lieobj>self.m, <lieobj>other.m))
        elif op == 4: return not (self <= other)
        elif op == 5: return not (self < other)
        else: raise ValueError(u"Bad op argument to mat.__richcmp__")
    
    def __repr__(mat self): return u"<" + u", ".join([repr(x) for x in self]) + u">"

    def __truediv__(mat self, n):
        return mat_from_liematrix(mat_div_mat_int(self.m, n))
    def __mod__(mat self, n):
        return mat_from_liematrix(mat_mod_mat_int(self.m, n))

    def transpose(mat self):
        return mat_from_liematrix(mat_transpose_mat(self.m))

    def __mul__(self, other):
        cdef matrix* m
        if isinstance(self, mat) and type(other) == type(0):
            return mat_from_liematrix(mat_mul_int_mat(<entry>other, (<mat>self).m))
        elif isinstance(self, mat) and isinstance(other, mat):
            #return mat_from_liematrix(mat_mul_mat_mat((<mat>self).m, (<mat>other).m))
            m = mat_mul_mat_mat((<mat>self).m, (<mat>other).m)
            print u"hello"
            #return mat_from_liematrix(m)
            return "foo"
        elif isinstance(self, mat) and isinstance(other, vec):
            return vec_from_lievector(vec_mul_mat_vec((<mat>self).m, (<vec>other).v))
        #elif isinstance(self, mat) and isinstance(other, pol):
        #    return pol_from_liepoly(pol_mul_mat_pol((<mat>other).m, (<pol>self).p))
        else: return mat.__mul__(other, self)
    def __neg__(mat self):
        return mat_from_liematrix(mat_min_mat(self.m))

    def __pow__(self, n, b):
        if b: return powermod(self, n, b)
        else: return power(self, n)
    def __add__(mat self, other):
        if isinstance(other, mat):
            return mat_from_lieobj(mat_add_mat_mat(<lieobj>(self.m), <lieobj>((<mat>other).m)))
        elif isinstance(other, vec):
            return mat_from_lieobj(mat_add_mat_vec(<lieobj>self.m, <lieobj>(<vec>other).v))
        else: return NotImplemented
    def __sub__(mat self, mat other):
        return mat_from_liematrix(mat_sub_mat_mat(self.m, other.m))

    def diag(self):
        return vec_from_lieobj(vec_diag_mat(<lieobj>self.m))
    def concat(mat self, mat other):
        return mat_from_liematrix(mat_append_mat_mat(self.m, other.m))

    Id = staticmethod(mat_Id)

def mat_Id(n): return mat_from_liematrix(mat_id(n))

# An extension class for the LiE pol type.
cdef class pol:
    cdef poly* p
    
    def __init__(self, val=0, *args):
        cdef poly* q
        
        # we do some manipulation of polynomials in here, while
        # calling python methods that could invoke the garbage
        # collector. It's easiest to turn off garbage collection until
        # finished. See below for more on gc.
        gc.off()
        
        # If val is an integer, create a constant polynomial.
        if type(val) == type(0):
            self.p = <poly*>pol_polynom_int(int(val))

        # If val is a polynomial, steal its pointer.  To copy a
        # polynomial p, use pol(p[:]).
        elif isinstance(val, pol):
            self.p = (<pol>val).p
        # for a rep, steal its heighest weight polynomial
        elif isinstance(val, rep):
            self.p = (<pol>val.hw_pol).p

        # LiE provides special functions to create polynomials from
        # vectors or matrices.
        elif isinstance(val, vec):
            self.p = <poly*>pol_monom_vec((<vec> val).v)
        elif isinstance(val, mat):
            self.p = <poly*>pol_polynom_mat((<mat> val).m)

        # Be sure to use the weight basis when creating a pol from an
        # AbstractVec.  This convention is particularly good for use
        # with rep(), since we want a weight w = [a,b,...c] to turn
        # into a representation with heighest weight polynomial
        # 1X[a,b,...,c].
        elif isinstance(val, AbstractVec):
            w = val.to_weight()
            if w.denominator != 1: raise ValueError(u"pol must have integral exponents.")
            else: self.p = <poly*>pol_polynom_vec((<vec> val.numerator).v)

        # Otherwise, val should be a nontrivial list or tuple.
        elif len(val) == 0:
            raise ValueError(u"Bad argument to pol(): %s" % val)

        # Finally, we handle the following types of arguments:
        # - a list of ints: [a,...,b] -> 1X[a,...,b]
        # - a tuple: (n, [a,...,b])   -> nX[a,...,b]
        # - a list of tuples: [(n, [a,...,b]), (m, [c,...,d]), ...]
        #   -> nX[a,...,b] + mX[c,...,d] + ...
        elif type(val) == type([]) or type(val) == type(()):
            if type(val)    == type(()): val = [val]
            if type(val[0]) != type(()): val = [(1, val)]
            if type(val[0]) == type(()):
                for i, (c, v) in enumerate(val):
                    q = <poly*>pol_polynom_bin_vec(pyint2bigint(int(c)), (<vec> vec(v)).v)
                    if i == 0: self.p = q
                    else: self.p = Add_pol_pol(self.p, q, 0)
        else:
            raise ValueError(u"Bad argument to pol(): %s" % val)
        protectpol(self.p)
        self.normalize()
        gc.on()

    cdef void normalize(pol self):
        if not issorted(<lieobj>self.p):
            neglectpol(self.p)
            self.p = Reduce_pol(self.p)
            protectpol(self.p)
        if self.p.nrows == 0:
            neglectpol(self.p)
            self.p = poly_null(self.p.ncols)
            protectpol(self.p)

    def __dealloc__(self):
        neglectpol(self.p)

    def nvars(self): return self.p.ncols
    def __len__(self):
        self.normalize()
        return self.p.nrows
    #def __hash__(self): return hash(repr(self))

    def __richcmp__(pol self, pol other, int op):
        if op == 0: return self.to_list() < other.to_list()
        elif op == 1: return self.to_list() <= other.to_list()
        elif op == 2: return bool_from_lieobj(int_eq_pol_pol(<lieobj> self.p, <lieobj> other.p))
        elif op == 3: return bool_from_lieobj(int_ne_pol_pol(<lieobj> self.p, <lieobj> other.p))
        elif op == 4: return not (self <= other)
        elif op == 5: return not (self < other)
        else: raise ValueError(u"Bad op argument to pol.__richcmp__")

    def to_list(self): return [self[i] for i in range(len(self))]
    def __iter__(self): return iter(self.to_list())
    def __getitem__(self, key):
        self.normalize()
        if isinstance(key, slice):
            i,j,_ = key.indices(len(self))
            return [self[k] for k in range(i, j)]
        else:
            i,_,_ = slice(key, None).indices(len(self))
            if i >= len(self): raise IndexError(u"poly index out of range.")
            return bigint2pyint(self.p.coef[int(i)]), \
                   vec_from_lieobj(vec_select_pol_int(self.p, i+1)) # LiE uses 1-indexing

    def __repr__(self):
        return u" + ".join([repr(c) + u"X" + repr(v) for c, v in self])

    def __mul__(self, other):
        if isinstance(self, pol) and isinstance(other, pol):
            return pol_from_liepoly(Mul_pol_pol((<pol>self).p, (<pol>other).p))
        elif isinstance(self, pol) and type(other) == type(0):
            return pol_from_liepoly(Mul_bin_pol(pyint2bigint(int(other)), (<pol>self).p))
        elif isinstance(self, pol) and isinstance(other, mat):
            return pol_from_liepoly(pol_mul_pol_mat((<pol>self).p, (<mat>other).m))
        else: return pol.__mul__(other, self)
    def __neg__(self): return -1*self

    def __add__(pol self, pol other):
        return pol_from_liepoly(Add_pol_pol(self.p, other.p, 0))
    def __sub__(pol self, pol other):
        return pol_from_liepoly(Add_pol_pol(self.p, other.p, 1))
    def __truediv__(pol self, other):
        if type(other) == type(0):
            return pol_from_liepoly(Div_pol_bin(self.p, pyint2bigint(int(other))))
        elif isinstance(other, vec):
            return pol_from_liepoly(Div_pol_vec(self.p, (<vec>other).v))
        else: return NotImplemented
    def __mod__(pol self, other):
        if type(other) == type(0):
            return pol_from_liepoly(Mod_pol_bin(self.p, pyint2bigint(int(other))))
        elif isinstance(other, vec):
            return pol_from_liepoly(Mod_pol_vec(self.p, (<vec>other).v))
        else: return NotImplemented

    def __pow__(self, n, b):
        if b: return powermod(self, n, b)
        else: return power(self, n)

    def coef(self, val):
        if isinstance(val, vec):
            return lieobj2pyint(bin_select_pol_vec(self.p, (<vec>val).v))
        elif isinstance(val, rep): return self.coef(vec(val.hw()))
        else: return self.coef(vec(val))

    def atensor(pol self, pol other):
        return pol_from_lieobj(pol_atensor_pol_pol(<lieobj>self.p, <lieobj>other.p))
    def degree(pol self): return Degree_pol(self.p)
    def dominant(pol self, grp g):
        return pol_from_lieobj(pol_dominant_pol_grp(self.p, g.g))
    def filterdom(pol self, grp g):
        return pol_from_lieobj(pol_filterdom_pol(self.p, g.g))
    def w_orbit(pol self, grp g):
        return pol_from_lieobj(pol_worbit_pol_grp(self.p, g.g))
    def alt_dom(pol self, val):
        if isinstance(val, grp):
            return pol_from_lieobj(pol_altdom_pol_grp(self.p, (<grp>val).g))
        elif isinstance(val, weyl):
            return pol_from_lieobj(pol_altdom_pol_vec_grp(\
                self.p, (<vec> val.rfls).v, (<grp> val.grp).g))
        else: return NotImplemented
    def alt_W_sum(pol self, grp g):
        return pol_from_lieobj(pol_altwsum_pol_grp(self.p, g.g))

class AbstractVec:
    """An element of the dual space to the maximal torus of a group (a
    root or weight). An AbstractVec v contains:
    
    v.grp:         A grp object.
    v.coords:      A vec object, of length v.grp.rank().
    v.denominator: An integer.
    v.basis:       An enum, either AbstractVec.ROOT or AbstractVec.WEIGHT
                   (internally, a string)

    An AbstractVec v is a vec object together with information about
    what vector space v lives in (namely, the maximal torus of v.grp),
    whether it's in the root or weight basis (v.basis), and whether it
    has rational coefficients (if v.denominator != 1).  The operations
    of addition, multiplication and equality of AbstractVecs handle
    changes of basis automatically, without the user having to look up
    cartan matrices, etc.  Multiplication uses the killing inner product
    on the maximal torus.  Indeed, most functions that require a vector
    in the weigth basis in LiE have been modified to accept an
    AbstractVec, and put it into the appropriate basis.  For instance,

    A2.rep(A2.root([1,1])) == A2.adjoint() returns True

    To manually convert between the root and weight bais, use the functions
    v.to_weight(), v.to_root(), and v.to_basis(b).
    
    All operations on AbstractVecs are over the rational numbers (via
    the python fractions module).  Be careful passing an AbstractVec
    with non-integer coefficients to a LiE function, since those
    coefficients will get truncated."""

    def __init__(self, coords, basis, g, d=1, *args):
        if g.rank() != len(coords):
            raise ValueError(u"Number of components should match grp rank.")
        self.basis = basis
        self.grp = grp(g)
        if isinstance(coords, AbstractVec):
            v = coords.to_basis(basis)
            self.numerator = v.numerator
            self.denominator = v.denominator
        elif isinstance(coords, vec):
            self.numerator = coords
            if d != 1: self.numerator, self.denominator = self.numerator.frac(d)
            else: self.denominator = 1
        else:
            self.denominator = reduce(lcm, [denom(x) for x in coords], d)
            self.numerator = vec([self.denominator * x for x in coords])
    def __repr__(self):
        return u"%s.%s(%s)" % (repr(self.grp), self.basis, \
                                 u",".join([Fraction.__str__(x) for x in self]))
    #def __hash__(self): return hash(repr(self))
    def __len__(self): return len(self.numerator)
    def to_list(self): return [Fraction(self.numerator[i], self.denominator) for i in range(len(self))]
    def __iter__(self): return iter(self.to_list())
    def __getitem__(self, key):
        if isinstance(key, slice):
            i,j,_ = key.indices(len(self))
            return [self[k] for k in range(i,j)]
        else:
            i,_,_ = slice(key, None).indices(len(self))
            return Fraction(self.numerator[i], self.denominator)

    def __eq__(self, other):
        if not self or not other: return bool(self) == bool(other)
        return self.to_list() == other.to_list()
    def __lt__(self, other): return self.to_list() < other.to_list()
    def __le__(self, other): return self.to_list() <= other.to_list()

    def __setitem__(self, i, v):
        if isinstance(v, Fraction):
            d = lcm(v.denominator, self.denominator)
            self.numerator = (d//self.denominator)*self.numerator
            self.numerator[i] = d * v
            self.denominator = d
        else:            
            self.numerator[i] = v*self.denominator

    def __neg__(self): return AbstractVec(-self.numerator, self.basis, self.grp)
    def __add__(self, other):
        if self.grp != other.grp: raise ValueError(u"Can't add AbstractVec's in different spaces.")
        if self.basis == other.basis:
            return AbstractVec(other.denominator*self.numerator+self.denominator*other.numerator, \
                               self.basis, self.grp, self.denominator*other.denominator)
        else: return self + other.to_basis(self.basis)
    def __sub__(self, other): return self + (-other)

    def __truediv__(self, n):
        return AbstractVec(self.numerator, self.basis, self.grp, n*self.denominator)

    def __mul__(self, other):
        if isinstance(other, AbstractVec):
            a = self.to_root()
            b = other.to_root()
            return Fraction(int_from_lieobj(int_inprod_vec_vec_grp( \
                (<vec>a.numerator).v, (<vec>b.numerator).v, (<grp>self.grp).g)), \
                            a.denominator*b.denominator)
        else:
            return AbstractVec(other*self.numerator, self.basis, self.grp, self.denominator)
    def __rmul__(self,other):
        if not isinstance(other, mat): return self*other

    ROOT = u"root"
    WEIGHT = u"weight"
    
    def to_basis(self, b):
        if self.basis == b: return self
        elif b == AbstractVec.ROOT:
            return AbstractVec(self.numerator * self.grp.inv_cartan(), AbstractVec.ROOT, \
                               self.grp, self.grp.det_cartan()*self.denominator)
        elif b == AbstractVec.WEIGHT:
            return AbstractVec(self.numerator * self.grp.cartan(), AbstractVec.WEIGHT, \
                               self.grp, self.denominator)
    def to_root(self): return self.to_basis(AbstractVec.ROOT)
    def to_weight(self): return self.to_basis(AbstractVec.WEIGHT)

    def cartan(self, other): return 2*self*other/other.norm()
    def norm(self): return self*self
    def concat(self, other):
        v = other.to_basis(self.basis)
        d = lcm(v.denominator, self.denominator)
        return AbstractVec((d*self.numerator).concat(d*other.numerator), \
                           self.basis, self.grp*other.grp, d)
    def dominant(self):
        return vec_from_lieobj(vec_dominant_vec_grp( \
            (<vec>vec(self.to_weight())).v, (<grp>self.grp).g))
    def isdominant(self): return self == self.dominant()
    def domweights(self):
        return [self.grp.weight(w) for w in mat_from_lieobj(mat_domweights_vec_grp(\
            (<vec> vec(self.to_weight())).v, (<grp>self.grp).g))]
    def canonical(self):
        return self.grp.weyl(vec_from_lieobj(vec_canonical_vec_grp(\
            (<vec> vec(self.to_weight())).v, (<grp>self.grp).g)))
    def w_orbit(self):
        return [self.grp.weight(v) for v in mat_from_lieobj(mat_worbit_vec_grp(\
            (<vec> vec(self.to_weight())).v, (<grp>self.grp).g))]
    def w_orbit_size(self):
        return lieobj2pyint(bin_worbitsize_vec_grp( \
            (<vec> vec(self.to_weight())).v, (<grp>self.grp).g))

class weyl:
    """A Weyl group element. A weyl word w contains:

    w.grp: A grp object.
    w.rfls: A vec object.  A vector [i,j,...,k] stands for the Weyl
            group element obtained from the product of elementary
            reflections r_i r_j ... r_k through the roots alpha_i,
            alpha_j, alpha_k.  Really, this isn't properly a vector,
            but instead just a list of integers.  However, vec is used
            since that's the representation used by LiE."""

    def __init__(self, rfls, grp grp, *args):
        self.grp = grp
        self.rfls = vec(rfls)

    def __len__(self): return len(self.rfls)
    def __repr__(self):
        return u"%s.weyl(%s)" % (self.grp, u",".join([repr(x) for x in self]))
    def __getitem__(self,key):
        if isinstance(key, slice):
            i,j,s = key.indices(len(self))
            return [self.rfls[k] for k in range(i, j, s)]
        else: return self.rfls[k]
    def __iter__(self): return iter(self[0:])
    #def __eq__(self, other): 

    def Bruhat_desc(self, other=None):
        if other:
            return [self.grp.weyl(w) for w in mat_from_lieobj(mat_bhdesc_vec_vec_grp( \
                (<vec> self.rfls).v, (<vec> other.rfls).v, (<grp>self.grp).g))]
        else:
            return [self.grp.weyl(w) for w in mat_from_lieobj(mat_bhdesc_vec_grp( \
                (<vec> self.rfls).v, (<grp>self.grp).g))]
    def Bruhat_leq(self, other):
        return bool_from_lieobj(int_bhleq_vec_vec_grp( \
            (<vec> self.rfls).v, (<vec> other.rfls).v, (<grp>self.grp).g))
    def length(self):
        return int_from_lieobj(int_length_vec_grp((<vec> self.rfls).v, (<grp>self.grp).g))
    def lreduce(self, L):
        return self.grp.weyl(vec_from_lieobj(vec_lreduce_vec_vec_grp( \
            (<vec> vec(L)).v, (<vec>self.rfls).v, (<grp>self.grp).g)))
    def reduce(self):
        return self.grp.weyl(vec_from_lieobj(vec_reduce_vec_grp( \
            (<vec>self.rfls).v, (<grp>self.grp).g)))
    def reflection(self):
        return mat_from_lieobj(mat_reflection_vec_grp((<vec>self.rfls).v, (<grp>self.grp).g))
    def rreduce(self, R):
        return self.grp.weyl(vec_from_lieobj(vec_rreduce_vec_vec_grp( \
            (<vec>self.rfls).v, (<vec> vec(R)).v, (<grp>self.grp).g)))
    def w_action(self):
        return mat_from_lieobj(mat_waction_vec_grp((<vec>self.rfls).v, (<grp>self.grp).g))

class toral:
    """A toral element t contains:

    t.grp: A grp object, of rank r.
    t.vec: A vec object, of length r + 1.  This is the representation
           for toral elements used by LiE. A vector
           [a_1,a_2,...,a_r,n] stands for the element of maximal
           torus,

           Sum_j=1^r exp(2 pi*i*a_j/n) t_j

           Where t_j are the dual basis to the fundamental weights
           w_i: (w_i, t_j) = delta_ij.  See the LiE manual for
           details.

    Though the internals of a toral element are the same as those used
    in LiE, to a python user, a toral element looks like a list of r
    fractions a_j/n.  To create a toral element, you can pass in a
    list of fractions, or specify an integer order as the common
    denominator."""

    def __init__(self, coords, grp=None, order=1, fromlie=False, *args):
        if coords:
            self.grp = grp if grp else coords.grp
            
            if (not fromlie and len(coords) != self.grp.rank()) or \
                   (fromlie and len(coords) != self.grp.rank() + 1):
                raise ValueError(u"Wrong number of components for toral().")

            if fromlie and isinstance(coords, vec): self.vec = coords
            elif isinstance(coords[0], Fraction):
                d = reduce(lcm, [f.denominator for f in coords], order)
                self.vec = vec([int(d*f) % d for f in coords] + [d])
            elif type(coords[0]) == type(0):
                self.vec = vec([x % order for x in coords] + [order])

    def __repr__(self):
        return u"%s.toral(%s)" % (self.grp, \
                                  u",".join([Fraction.__str__(x) for x in self]))
    def __len__(self): return len(self.vec) - 1
    def __getitem__(self,key):
        if isinstance(key, slice):
            i,j,s = key.indices(len(self))
            return [Fraction(self.vec[k], self.vec[-1]) for k in range(i, j, s)]
        else:
            return Fraction(self.vec[key], self.vec[-1])
    def __iter__(self): return iter(self[0:])

    def cent_roots(self):
        return [self.grp.root(v) for v in mat_from_lieobj(mat_centroots_vec_grp( \
            (<vec> self.vec).v, (<grp> self.grp).g))]
    def centr_type(self):
        return grp_from_lieobj(grp_centrtype_vec_grp((<vec> self.vec).v, (<grp> self.grp).g))


class rep:
    """A lie group representation. A rep r contains

    r.grp:    A grp object.
    r.hw_pol: A pol object, where a term with coefficient c and
              exponents v stands for an irrep of r.grp with
              multiplicity c and heighest weight v.  This is what LiE
              uses for a representation.  The advantage here is that
              the group information is carried around as well.

    A rep object r behaves like an element of the representation ring
    of r.grp.  Namely, a product of two reps is their tensor product.
    A sum of reps is a direct sum, multiplication of r by an integer n
    is an n-fold direct sum of copies of r, etc.
    
    rep objects r can be indexed:

    r[k] is (m, irr), where irr is the k'th irreducible representation
    appearing in r, and m is its multiplicity.  rep(r[:], r.grp) == r
    returns True."""
    
    def __init__(self, val, g=None, *args):
        if val:
            self.grp = grp(g) if (g or g == T0) else val.grp
            if isinstance(val, pol): self.hw_pol = val
            else: self.hw_pol = pol(val)
            if (<pol>self.hw_pol).p.ncols != self.grp.rank():
                raise ValueError(u"Dimension of vector space doesn't match Lie rank")

    def __repr__(self):
        return u" + ".join([u"%s%s(%s)" % \
                            ((u"%d*" % c if c != 1 else u""), self.grp, \
                             u",".join([repr(x) for x in v])) \
                            for c, v in self.hw_pol])
    #def __hash__(self): return hash((self.grp, self.hw_pol))
    def __len__(self): return len(self.hw_pol)
    def __add__(self, other): return rep(self.hw_pol+other.hw_pol, self.grp)
    def __mul__(self, other):
        if isinstance(other, rep):
            return rep(pol_from_lieobj(pol_tensor_pol_pol_grp( \
                (<pol>self.hw_pol).p, (<pol>other.hw_pol).p, (<grp>self.grp).g)), self.grp)
        else: return rep(self.hw_pol*other, self.grp)
    def __rmul__(self, other): return self*other
    def __truediv__(self, other): return rep(self.hw_pol/other, self.grp)
    def __pow__(self, n):
        return rep(pol_from_lieobj(pol_ptensor_int_pol_grp( \
            n, (<pol>self.hw_pol).p, (<grp>self.grp).g)), self.grp)
    def __eq__(self, other):
        return (self.grp, self.hw_pol) == (other.grp, other.hw_pol)

    def to_list(self):
        return [(n, rep(w, self.grp)) for n,w in self.hw_pol]
    def __iter__(self): return iter(self.to_list()) 
    
    def __getitem__(self, key):
        if isinstance(key, slice):
            return [(n, rep(w, self.grp)) for n,w in self.hw_pol[key]]
        else:
            n,w = self.hw_pol[key]
            return (n, rep(w, self.grp))
    def hw(self): return self.grp.weight(self.hw_pol[-1][1])
    def hws(self):
        return [self.grp.weight(w) for (_,w) in self.hw_pol]
    def coef(self, val): return self.hw_pol.coef(val)

    def Adams(self, n):
        return pol_from_lieobj(pol_adams_int_pol_grp(n, (<pol>self.hw_pol).p, (<grp>self.grp).g))
    def alt_tensor(self, n):
        return self.grp.rep(pol_from_lieobj(pol_alttensor_int_pol_grp( \
            n, (<pol>self.hw_pol).p, (<grp>self.grp).g)))
    def branch(self, h, m):
        return h.rep(pol_from_lieobj(pol_branch_pol_grp_mat_grp( \
            (<pol>self.hw_pol).p, (<grp>h).g, <lieobj>(<mat>m).m, (<grp>self.grp).g)))
    def branch_n(self,n):
        return self.branch(self.grp[n], self.grp.factor_res_mat(n))
    def char(self): return self.dom_char().w_orbit(self.grp)
    def dual(self):
        return self.grp.rep(pol_from_lieobj(pol_contragr_pol_grp( \
            (<pol>self.hw_pol).p, (<grp>self.grp).g)))
    def dim(self):
        return lieobj2pyint(bin_dim_pol_grp((<pol>self.hw_pol).p, (<grp>self.grp).g))
    def dom_char(self):
        return pol_from_lieobj(pol_domchar_pol_grp((<pol>self.hw_pol).p, (<grp>self.grp).g))
    def dom_char_coef(self, val):
        return lieobj2pyint(bin_domchar_pol_vec_grp( \
            (<pol>self.hw_pol).p, (<vec> vec(val)).v, (<grp>self.grp).g))
    def spectrum(self, t):
        if not isinstance(t, toral): raise ValueError(u"Argument to spectrum should be a toral elt.")
        return pol_from_lieobj(pol_spectrum_pol_vec_grp( \
            (<pol>self.hw_pol).p, (<vec>t.vec).v, (<grp>self.grp).g))
    def sym_tensor(self, n):
        return self.grp.rep(pol_from_lieobj(pol_symtensor_int_pol_grp( \
            n, (<pol>self.hw_pol).p, (<grp>self.grp).g)))
    def tensor_coef(self, other, x):
        if isinstance(x, vec): v = x
        elif isinstance(x, rep): v = vec(x.hw())
        else: v = vec(x)
        return lieobj2pyint(bin_tensor_pol_pol_vec_grp( \
            (<pol>self.hw_pol).p, (<pol>other.hw_pol).p, (<vec>v).v, (<grp>self.grp).g))
        
    def dynkin_index(self):
        g = self.grp
        return sum([n*i.dim()*(i.hw() * (i.hw()+2*g.rho())) for n,i in self])/(2*g.dim())
    def prod(self, other):
        return (self.grp*other.grp).rep(self.hw_pol.atensor(other.hw_pol))

    def pprint(self):
        if len(self) == 1 and self[0][0] == 1 and len(self.grp) == 1:
            if self.grp.ss_dim() == 0:
                return u"(%s)" % u",".join([Fraction.__str__(x) for x in self.hw()])
            elif self.hw() < self.dual().hw():
                return u"%db" % self.dim()
            else: return repr(self.dim())
        else:
            irreps = []
            for n, i in self:
                dims = []
                for k in range(len(i.grp)):
                    x = i.branch_n(k)[0][1]
                    if i.grp[k].ss_dim() == 0: # if toral, use charges instead of dimensions
                        dims = dims + [Fraction.__str__(m) for m in x.hw()]
                    else: dims.append(x.pprint())
                irrep = u"%s(%s)" % (repr(n) if n != 1 else u'', u",".join(dims))
                irreps.append(irrep)
            return u" + ".join(irreps)
        
    def weights(self): return [(c, self.grp.weight(v)) for c,v in self.char()]
        
def block_mat(m1, m2):
    return mat_from_lieobj(mat_blockmat_mat_mat((<mat> mat(m1)).m, (<mat> mat(m2)).m))
def fundam(m, grp g=None):
    if not g: g = m[0].grp
    return [g.root(v) for v in \
            mat_from_lieobj(mat_fundam_mat_grp((<mat> mat(m)).m, g.g))]
def closure(m, grp g=None):
    if not g: g = m[0].grp
    return [g.root(v) for v in \
            mat_from_lieobj(mat_closure_mat_grp((<mat> mat(m)).m, g.g))]
def cartan_type(m, grp g=None):
    if not g: g = m[0].grp
    return grp_from_lieobj(grp_carttype_mat_grp((<mat> mat(m)).m, g.g))
def cent_roots(tors):
    return [tors[0].grp.root(v) for v in mat_from_lieobj(mat_centroots_mat_grp( \
                (<mat> mat([t.vec for t in tors])).m, (<grp> tors[0].grp).g))]
def centr_type(tors):
    return grp_from_lieobj(grp_centrtype_mat_grp((<mat> mat([t.vec for t in tors])).m, \
                                                    (<grp> tors[0].grp).g))
def decomp(p, grp g):
    return g.rep(pol_from_lieobj(pol_decomp_pol_grp((<pol>pol(p)).p, g.g)))
def vdecomp(p, grp g): # funniness with negative numbers and wraparound...
    return g.rep(pol_from_lieobj(pol_vdecomp_pol_grp((<pol>pol(p)).p, g.g)))
def KL_poly(x, y):
    return pol_from_lieobj(pol_klpoly_vec_vec_grp((<vec>x.rfls).v, (<vec>y.rfls).v, (<grp>x.grp).g))
def R_poly(x, y):
    return pol_from_lieobj(pol_rpoly_vec_vec_grp((<vec>x.rfls).v, (<vec>y.rfls).v, (<grp>x.grp).g))
def w_order(I, grp g):
    return lieobj2pyint(bin_worder_vec_grp(<lieobj>(<vec>vec(I)).v, g.g))

# =============== Conversion routines =================

# These routines handle conversion of LiE objects into python objects
# in such a way that we can throw away the original LiE object.

# Instead of writing some routine involving shifting and adding
# digits, I'm currently using the hack of using a char* string to
# translate between a LiE bigint and a PyLong.  This is an obvious
# opportunity for optimization that I just haven't had time for.

cdef object bigint2pyint(bigint *b):
    cdef string s = bigint2str(b)
    i = PyLong_FromString(s, NULL, 10)
    freem(<void*>s) # our responsibility after calling bigint2str
    return i

cdef lieobj2pyint(lieobj o): return bigint2pyint(<bigint*>o)

cdef bigint* pyint2bigint(object i):
    cdef char* s = pyobj2charptr(i)
    return str2bigint(s)

cdef char* pyobj2charptr(object o):
    s = repr(o).encode()
    return <char*>s

cdef char first_char(s):
    bs = s.encode()
    return (<char*> bs)[0]

# Here, we can just ignore the tekst and intcel objects when we're
# done, since they'll get garbage collected next time around.

cdef char* str_from_lieobj(lieobj t):
    cdef char* s = (<tekst*>t).string
    return s

cdef int_from_lieobj(lieobj o):
    return (<intcel*>o).intval

cdef bool_from_lieobj(lieobj o):
    return <intcel*>o == bool_true

# =============== Extension type constructors =================

# These are the routines that construct an extension type from an
# underlying pointer. This is often a good time to run the garbage
# collector, since LiE has just finished a computation and returned
# the answer.

# Macros for bypassing __init__ when instantiating objects. (They do
# NOT bypass __cinit__!)

cdef extern from "lie-py.h":
    cdef grp NEW_GRP  "PY_NEW" (object t)
    cdef vec NEW_VEC  "PY_NEW" (object t)
    cdef mat NEW_MAT  "PY_NEW" (object t)
    cdef pol NEW_POL  "PY_NEW" (object t)

cdef grp_from_lieobj(lieobj o):
    cdef grp G
    G = NEW_GRP(grp)
    G.g = o
    protect(o)
    gc.maybe_run()
    return G

cdef vec_from_lievector(vector *v):
    cdef vec V
    V = NEW_VEC(vec)
    V.v = v
    protect(<lieobj>V.v)
    gc.maybe_run()
    return V

cdef mat_from_liematrix(matrix *m):
    cdef mat M
    M = NEW_MAT(mat)
    M.m = m
    protect(<lieobj>M.m)
    gc.maybe_run()
    return M

cdef pol_from_liepoly(poly *p):
    cdef pol P
    P = NEW_POL(pol)
    P.p = p
    protectpol(P.p)
    gc.maybe_run()
    return P

cdef vec_from_lieobj(lieobj v): return vec_from_lievector(<vector*>v)
cdef mat_from_lieobj(lieobj m): return mat_from_liematrix(<matrix*>m)
cdef pol_from_lieobj(lieobj p): return pol_from_liepoly(<poly*>p)

cdef tor_from_lievector_grp(vector *v, grp g):
    cdef vec V
    V = NEW_VEC(vec)
    V.v = v
    T = toral(None, None)
    T.vec = V
    T.grp = g
    protect(<lieobj>(<vec>T.vec).v)
    gc.maybe_run()
    return T

# ================= Garbage Collection ======================

class GC:
    """For garbage collection, LiE uses a hybrid of refcounting and
    mark & sweep.  For an object o, the field o->any.nref is
    incremented and decremented by setshared(o) and clrshared(o),
    respectively.  If nref is zero, that's an invitation for any
    function to reuse the object if it likes.  However, there's no
    guarantee that will happen.

    To get rid of leftover objects, LiE uses a mark & sweep collector,
    which among other things, sets any positive nref value to 1.
    Thus, we can't use nref like a normal refcount, since it won't
    properly keep track of how many python objects point to it after
    gc() is called.  We'd rather not modify the object datatype, so
    instead, we added a table pyobj of the same size as the object
    table that holds a refcount specifically for LiE objects pointed
    to by python. setpyobj and clrpyobj inc/decrement this count.
    Finally, we added a routine to the gc() function in mem.c that
    marks all objects with positive pyobj count before sweeping.

    There are a couple dangerous moments, particularly in
    pol.__init__, where garbage collection could sweep away
    unprotected objects.  Instead of calling protect and neglect a
    billion times, it's easiest just to turn off garbage collection
    until we're done.  gc.hold provides a counter so that more than
    one function can turn gc on and off simultaneously, and it'll be
    handled correctly."""
    
    def __init__(self):
        self.hold = 0
        self.verbose = False

    def on(self):
        self.hold = self.hold - 1
    def off(self):
        self.hold = self.hold + 1
        
    def run(self):
        if self.verbose: print u"[gc:", chunks, u"->",
        LiE_gc()
        if self.verbose: print chunks, u"]"
    def maybe_run(self):
        if chunks > gccrit and not self.hold: self.run()
        elif chunks > gccrit and self.verbose:
            print u"[gc: on hold", self.hold, u"]"

    def nobjs(self): print chunks
    def nref(self, x):
        if isinstance(x, grp): return (<grp>x).g.any.nref
        elif isinstance(x, vec): return (<lieobj>(<vec>x).v).any.nref
        elif isinstance(x, mat): return (<lieobj>(<mat>x).m).any.nref
        elif isinstance(x, pol): return (<lieobj>(<pol>x).p).any.nref

    def print_objs(self):
        c_print_objs()

# the global instance of GC()
gc = GC()

cdef protect(lieobj o):
    # ensures o won't get swept until clrpyobj(o) is called:
    setpyobj(o)
    # ensures o won't get reused by some math routine:
    setshared(o)

cdef protectpol(poly* p):
    # protect from sweeping
    setpyobj(<lieobj> p)
    # protect from reuse
    setshared(<lieobj> p)
    for 0 <= i < p.nrows:
        # we need to do the same for each of the coefficient objects
        setpyobj(<lieobj>p.coef[i])
        setshared(<lieobj>p.coef[i])

cdef neglect(lieobj o):
    clrpyobj(o)
    # no need to call clrshared(o).  The object will get swept away soon.

cdef neglectpol(poly *p):
    clrpyobj(<lieobj>p)
    for 0 <= i < p.nrows:
        clrpyobj(<lieobj>p.coef[i])
        # a call to clrshared here would be fatal, since LiE's mark
        # and sweep collector asserts that each coefficient has
        # positive refcount before sweeping a polynomial.

# ================== Error Handling ====================

# When LiE routines encounter a problem, they call a function
# error(format, ...).  We've modified error() to set a PyErr
# exception, and raise SIGFPE.  Here, the call to signal.signal
# ensures that SIGFPE is handled by calling LiE_error.

def LiE_error(signum, frame):
    print u"LiE error"
    catch_error()
signal.signal(signal.SIGFPE, LiE_error)

# LiE_error calls catch_error(), which will detect and print the PyErr
# exception.  The code "except *" ensures that Cython will call
# PyErr_Occurred.

cdef void catch_error() except *: return

# ================== Initialization ====================

label = label_null   # prevent a bus error in gc()
redirected_input = 1 # true

init() # allocate the object table, among other things

# basic symbols
[T0, T1, T2, T3, T4, T5, T6, T7, T8, T9] = [grp(0,n) for n in range(0,10)]
[A1, A2, A3, A4, A5, A6, A7, A8, A9]     = [grp(1,n) for n in range(1,10)]
[B2, B3, B4, B5, B6, B7, B8, B9]         = [grp(2,n) for n in range(2,10)]
[C2, C3, C4, C5, C6, C7, C8, C9]         = [grp(3,n) for n in range(2,10)]
[D3, D4, D5, D6, D7, D8, D9]             = [grp(4,n) for n in range(3,10)]
[E6, E7, E8, F4, G2] = [grp(u'E6'), grp(u'E7'), grp(u'E8'), grp(u'F4'), grp(u'G2')]

