/* Group operations */
#include "lie.h"

void check_wt(vector* lambda, index r)
{ if (lambda->ncomp!=r) error("Size of weight should equal Lie rank.\n"); }

void check_wts(matrix* m, index r)
{ if (m->ncols!=r) error("Size of weights should equal Lie rank.\n"); }

void check_rt(vector* rt, index s)
{ if (rt->ncomp!=s) error("Size of root should equal semisimple rank.\n"); }

void check_rts(matrix* m, index s)
{ if (m->ncols!=s) error("Size of roots should equal semisimple rank.\n"); }

void check_toral(vector* t, index r, index lim)
{ if (t->ncomp!=r+1)
    error("Size of toral element should equal Lie rank + 1.\n");
  if (t->compon[r]<lim)
  { error("Final entry of toral element should %stive.\n"
	 , lim ? "be posi" : "not be nega");
  }
}

void check_torals(matrix* m, index r)
{ index i;
  if (m->ncols!=r+1)
    error("Size of toral elements should equal Lie rank + 1.\n");
  for (i=0; i<m->nrows; i++) if (m->elm[i][r]<0)
    error("Final entry of toral elements should not be negative.\n");
}
/*
void testdom(entry* v, object grp)
{ index j, s=Ssrank(grp);
  for(j=0; j<s; j++) if (*v++<0) error ("Weight is not dominant\n");
}
*/
void check_Ww(vector* ww, index s)
{ index i,l=ww->ncomp; entry* w=ww->compon;
  for (i=0; i<l; i++)
    if (w[i]<0) error("Weyl word entries should not be negative.\n");
    else if (w[i]>s)
      error("Weyl word entries should not exceed semisimple rank.\n");
}

void check_Wws(matrix* m, index s)
{ index i,j,l=m->ncols;
  for (i=0; i<m->nrows; i++)
  { entry* w=m->elm[i];
    for (j=0; j<l; j++) 
      if (w[j]<0) error("Weyl word entries should not be negative.\n");
      else if (w[j]>s)
	error("Weyl word entries should not exceed semisimple rank.\n");
  }
}
/*
object int_eq_grp_grp(g1,g2) object g1,g2; /* used externally (decomp) !!!->* /
{ index i;
  if (g1->g.ncomp!=g2->g.ncomp || g1->g.toraldim!=g2->g.toraldim)
    return (object) bool_false;
  for (i=0; i<g1->g.ncomp; i++)
  { simpgrp* s1=Liecomp(g1,i); simpgrp* s2=Liecomp(g2,i);
    if (s1->lietype!=s2->lietype || s1->lierank!=s2->lierank)
      return (object) bool_false; 
  }
  return (object) bool_true;
}
*/
object grp_mul_grp_grp(g1, g2) object g1,g2;
{ index i, ng1=g1->g.ncomp, ng2=g2->g.ncomp;
  object result = (object) mkgroup(ng1+ng2);
  for (i = 0; i<ng1; i++) Liecomp(result,i) = Liecomp(g1,i);
  for (i = 0; i<ng2; i++) Liecomp(result,ng1+i) = Liecomp(g2,i);
  result->g.toraldim = g1->g.toraldim + g2->g.toraldim;
  return (result);
}

object grp_select_grp_int(g,n) object g; index n;
{ object result;
  index i = n;
  if (i<0 || i> g->g.ncomp) error("Index into group out of range.\n");
  if (i>0)
  { result= (object) mkgroup(1); Liecomp(result,0) = Liecomp(g,i-1);
    return result;
  }
  result= (object) mkgroup(0); result->g.toraldim = g->g.toraldim;
  return result;
}

object vec_sort_vec(v) object v;
{ vector* w=copyvector(&v->v); sortrow(w->compon,w->ncomp);
  return (object) w;
}

object mat_sort_mat(m) matrix* m;
{ matrix* w=copymatrix(m); Qksortmat(w,cmpfn); return (object) w; }

object mat_unique_mat(m) matrix* m;
{ return (object) Unique(copymatrix(m),cmpfn); }

object mat_blockmat_mat_mat(a,b) matrix* a,* b;
{ return (object) Blockmat(a,b); }

object vid_setdefault_grp(g) object g;
{ if ((cmpfn==height_decr||cmpfn==height_incr) && defaultgrp) unmark_sorted();
  defaultgrp=g; return (object) NULL;
}

object vid_setdefault(void)
{ if (!defaultgrp) error("No default group defined yet.\n");
  Printf("%5s"," "); printgrp(defaultgrp); Printf("\n"); return (object) NULL;
}

object grp_setdefault(void)
{
    if (!defaultgrp) error("No default group defined yet.\n");
    return (object) defaultgrp;
}

object mat_center_grp(g) object g;
{ return (object) Center(g); }

object int_dim_grp(object g)
{ return (object) mkintcel(Dimgrp(g)); }

object vid_diagram_grp(g) object g;
{ return Diagram(g); }

object vec_liecode_grp(g) object g;
{ object result;
  if (g->g.ncomp && !simpgroup(g)) error("No liecode for composite groups.\n");
  result= (object) mkvector(2);
  if (g->g.ncomp == 0)
    { result->v.compon[0]=0; result->v.compon[1]=g->g.toraldim; }
  else
  { simpgrp* s=Liecomp(g,0);
    result->v.compon[0]=(s->lietype - 'A' + 1);
    result->v.compon[1]=s->lierank;
  }
  return result;
}

object groupmake(char lietype,index rank)
{ object result;
  if (wronggroup(lietype,rank))
      error("Result %c%ld is illegal group.\n", lietype, (long)rank);
  if (lietype=='T')
  { result=(object) mkgroup((index) 0); result->g.toraldim=rank; }
  else
  { result=(object) mkgroup((index) 1);
    Liecomp(result,0)=mksimpgrp(lietype,rank);
  }
  return result;
}

object grp_liegroup_int_int(typ,rank)  entry typ, rank;
{ entry n = typ;
  if (n<0 && n>7) error ("There is no group of such a type.\n");
  return groupmake(n ? 'A'-1+n : 'T',rank);
}

/*
object int_ncomp_grp(g) group* g; { return (object) mkintcel(g->ncomp); }
*/

object int_lierank_grp(g) object g;
{ return (object) mkintcel(Lierank(g)); }

object int_cartan_vec_vec_grp(v,w,g) vector* v,* w; object g;
{ index s=Ssrank(grp=g); check_rt(v,s); check_rt(w,s); checkroot(w->compon);
  return (object) mkintcel(Cart_inprod(v->compon,w->compon));
}

object mat_cartan_grp(object g) { grp=g; return (object) Cartan(); }

object grp_carttype_mat_grp(m,g) matrix* m; object g;
{ check_rts(m,Ssrank(grp=g)); return (object) Carttype(m); }

object mat_centroots_vec_grp(t,g) vector* t; object g;
{ index r=Lierank(grp=g); matrix* m=mkmatrix(1,r+1); check_toral(t,r,0);
  copyrow(t->compon,*m->elm,r+1); return (object) Centroots(m);
}

object mat_centroots_mat_grp(m,g) matrix* m; object g;
{ check_torals(m,Lierank(grp=g)); return (object) Centroots(m); }

object grp_centrtype_vec_grp(t,g) vector* t; object g;
{ index r=Lierank(grp=g); matrix* m=mkmatrix(1,r+1); check_toral(t,r,0);
  copyrow(t->compon,*m->elm,r+1); return (object) Centrtype(m);
}

object grp_centrtype_mat_grp(m,g) matrix* m; object g;
{ check_torals(m,Lierank(grp=g)); return (object) Centrtype(m); }

/*
object mat_closure_mat_grp(m,g) matrix* m; object g;
{ check_rts(m,Ssrank(grp=g)); return (object) Closure (m,true,0); }
*/

object int_detcart_grp(object g)
{ grp=g; return (object) mkintcel(Detcartan()); }

object mat_domweights_vec_grp(v,g) vector* v; object g;
{ check_wt(v,Lierank(grp=g)); testdom(v->compon,g);
  return (object) Domweights(v);
}
/*
object mat_fundam_mat_grp(m,g) matrix* m; object g;
{ check_rts(m,Ssrank(grp=g)); return (object) Closure (m,false,0); }
*/
object vec_highroot_grp(grp) object grp;
{ if (grp->g.ncomp!=1)
    error("Only groups with one simple component have a highroot.\n");
  return (object) Highroot(Liecomp(grp,0));
}

object mat_icartan_grp(object g) { grp=g; return (object) Icartan(); }

object int_inprod_vec_vec_grp(v,w,g) vector* v,* w; object g;
{ index s=Ssrank(grp=g); check_rt(v,s); check_rt(w,s);
  return (object) mkintcel(Inprod(v->compon,w->compon));
}

object int_norm_vec_grp(root,g) vector* root; object g;
{ check_rt(root,Ssrank(grp=g));
  return (object) mkintcel(Inprod(root->compon,root->compon));
}

object int_numproots_grp(g) object g;
{ return (object) mkintcel(Numproots(g)); }

object mat_posroots_grp(g) object g; { return ((object) Posroots(g)); }


object mat_bhdesc_vec_grp(w,g) vector* w; object g;
{ vector* rw=(check_Ww(w,Ssrank(grp=g)),Reduce(w));
  matrix* result=Bh_desc(rw->compon,rw->ncomp);
  freemem(rw); return (object) result;
}

object mat_bhdesc_vec_vec_grp(v,w,g) vector* v,* w; object g;
{ index i,s=Ssrank(grp=g); vector* rw; entry* x=mkintarray(s); matrix* result;
  check_Ww(v,s); check_Ww(w,s);
  for (i=0; i<s; i++) x[i]=1; Waction(x,v); rw=Reduce(w); 
  result = Bh_desc_rel(rw->compon,rw->ncomp,x);
  freearr(x); freemem(rw); return (object) result;
}

object int_bhleq_vec_vec_grp(v,w,g) vector* v,* w; object g;
{ index s=Ssrank(grp=g); check_Ww(v,s); check_Ww(w,s);
  return (object) (Bh_leq(v,w) ? bool_true : bool_false);
}

object vec_canonical_vec_grp(v,g) vector* v; object g;
{ check_Ww(v,Ssrank(grp=g)); return (object) Canonical(v); }

object mat_canonical_mat_grp(m,g) matrix* m; object g;
{ check_Wws(m,Ssrank(grp=g)); return (object) Canonicals(m); }

object vec_dominant_vec_grp(v,g) vector* v; object g;
{ check_wt(v,Lierank(grp=g)); return (object) Dominant(v); }

object mat_dominant_mat_grp(m,g) matrix* m; object g;
{ index i; matrix* result=(check_wts(m,Lierank(grp=g)),copymatrix(m));
  for (i=0; i<m->nrows; i++) make_dominant(result->elm[i]);
  return (object) result;
}

object pol_dominant_pol_grp(p,g) poly* p; object g;
{ index i; poly* result=(check_pol(p,Lierank(grp=g)),copypoly(p));
  for (i=0; i<p->nrows; i++) make_dominant(result->elm[i]);
  return (object) Reduce_pol(result);
}

object vec_exponents_grp(g) object g;
{return ((object) Exponents(g));}

object mat_filterdom_mat (m,g) matrix* m; object g;
{ check_wts(m,Lierank(grp=g)); return (object) Filter_dom_m(m); }

object pol_filterdom_pol (p,g) poly* p; object g;
{ check_pol(p,Lierank(grp=g)); return (object) Filter_dom(p); }

object pol_klpoly_vec_vec_grp(x,y,g) vector* x,* y; object g;
{ index s=Ssrank(grp=g); poly* result; cmpfn_tp sav_cmp=cmpfn;
  check_Ww(x,s); check_Ww(y,s);
  cmpfn=lex_decr; result=KLpoly(x,y); cmpfn=sav_cmp;
  if (cmpfn!=lex_decr) clrsorted(result); return (object) result;
}

object int_length_vec_grp(v,g) vector* v; object g;
{ check_Ww(v,Ssrank(grp=g)); return (object) mkintcel(Length(v)); }
/*
object vec_longword_grp(g) object g;
{ index i,s=Ssrank(grp=g); entry* minus_rho=mkintarray(s); vector* result;
  for (i=0; i<s; i++) minus_rho[i]= -1;
  result=Wwordv(minus_rho,Numproots(g)); freearr(minus_rho);
  return (object) result;
}
*/
object vec_lreduce_vec_vec_grp(vector* L,vector* ww,object g)
{ vector* result; index l=ww->ncomp; entry* w=mkintarray(l);
  check_Ww(ww,Ssrank(grp=g)); copyrow(ww->compon,w,l);
  result=L_red(L,w,l); freearr(w); return (object) result;
}

object vec_lrreduce_vec_vec_vec_grp(L,ww,R,g)
  vector* L,* ww,* R; object g;
{ vector* result; index l=ww->ncomp; entry* w=mkintarray(l);
  check_Ww(ww,Ssrank(grp=g)); copyrow(ww->compon,w,l);
  result=LR_red(L,w,l,R); freearr(w); return (object) result;
}

object mat_orbit_int_vec_mat(i,v,m) entry i; vector* v; matrix* m;
{ if (v->ncomp != m->ncols)
    error ("Size of vector should match number of columns of matrix");
  if (!v->ncomp) return (object) mkmatrix(1,0); /* one empty vector */
  if (m->nrows % v->ncomp)
    error ("Size of vector should divide number of rows of matrix");
  if (i<1) error ("limit for orbit must be positive");
  return (object) Orbit(i,v,m->elm,m->nrows/v->ncomp);
}

object mat_orbit_vec_mat(v,m) vector* v; matrix* m;
{ if (v->ncomp != m->ncols)
    error ("Size of vector should match number of columns of matrix");
  if (!v->ncomp) return (object) mkmatrix(1,0); /* one empty vector */
  if (m->nrows % v->ncomp)
    error ("Size of vector should divide number of rows of matrix");
  return (object) Orbit(1000,v,m->elm,m->nrows/v->ncomp);
}

object vec_reduce_vec_grp(ww,g) vector* ww; object g;
{ vector* empty=mkvector(0);
  object result=vec_lreduce_vec_vec_grp(empty,ww,g); freemem(empty);
  return result;
}

object mat_reflection_vec_grp(rt,g) vector* rt; object g;
{ check_rt(rt,Ssrank(grp=g)); checkroot(rt->compon);
  return (object) Reflection(rt->compon);
}

object vec_rreduce_vec_vec_grp(ww,R,g) vector* ww,* R; object g;
{ vector* result; index i,l=ww->ncomp; entry* w=mkintarray(l);
  check_Ww(ww,Ssrank(grp=g)); for (i=0; i<l; i++) w[i]=ww->compon[l-1-i];
  result=L_red(R,w,l); freearr(w);
  { index n=result->ncomp-1; entry* res=result->compon; /* reverse */
    for (i=0; i<n-i; i++) swap(&res[i],&res[n-i]);
  }
  return (object) result;
}

object pol_rpoly_vec_vec_grp(x,y,g) vector* x,* y; object g;
{ index s=Ssrank(grp=g); poly* result; check_Ww(x,s); check_Ww(y,s);
  make_q(); result=Rpoly(Canonical(x),Canonical(y)); clear_q();
  return (object) result;
}

object vec_waction_vec_vec_grp(v,word,g) vector* v,* word; object g;
{ vector* result; check_wt(v,Lierank(grp=g)); check_Ww(word,Ssrank(g));
  result=copyvector(v); Waction(result->compon,word);
  return (object) result;
}

object mat_waction_vec_grp(word,g) vector* word; object g;
{ check_Ww(word,Ssrank(grp=g)); return (object) Weyl_mat(word); }

object mat_waction_mat_vec_grp(matrix* m, vector* word, object g)
{ index i; matrix* result;
  check_wts(m,Lierank(grp=g)); check_Ww(word,Ssrank(g)); result=copymatrix(m);
  for (i=0; i<result->nrows; ++i) Waction(result->elm[i],word);
  return (object) result;
}

object pol_waction_pol_vec_grp(poly* p, vector* word, object g)
{ index i; poly* result;
  check_pol(p,Lierank(grp=g)); check_Ww(word,Ssrank(g)); result=copypoly(p);
  for (i=0; i<result->nrows; ++i) Waction(result->elm[i],word);
  return (object) result;
}

object mat_worbit_vec_grp(vector* v, object g)
{ check_wt(v,Lierank(grp=g)); return (object) Weyl_orbit(v->compon,NULL); }

object mat_worbitgraph_vec_grp(vector* v, object g)
{ matrix* r,* orbit_graph; check_wt(v,Lierank(grp=g));
  r=Weyl_orbit(v->compon,&orbit_graph); freemem(r);
  return (object) orbit_graph;
}

object pol_worbit_pol_grp(p,g) poly* p; object g;
{ check_pol(p,Lierank(grp=g)); return (object) Worbit_p(p); }

object bin_worbitsize_vec_grp(v,g) vector* v; object g;
{ check_wt(v,Lierank(grp=g)); return (object) Orbitsize(v->compon); }

object bin_worder_grp(g) object g;
{ return (object) Worder(grp=g); }

object bin_worder_vec_grp(I,g) object I,g;
{ index i,s=Ssrank(grp=g);
  for(i=0; i<I->v.ncomp; i++) if (I->v.compon[i]>s || I->v.compon[i]<=0)
    { Printf("Reflection %ld",(long)I->v.compon[i]);
      error(" is out of range.\n");
    }
  return (object) sub_Worder(&I->v);
}

object vec_wrtaction_vec_vec_grp(v,word,g) vector* v,* word; object g;
{ index s=Ssrank(grp=g); vector* result; check_rt(v,s); check_Ww(word,s);
  result=copyvector(v); Wrtaction(result->compon,word);
  return (object) result;
}

object mat_wrtaction_vec_grp(word,g) vector* word; object g;
{ check_Ww(word,Ssrank(grp=g)); return (object) Weyl_rt_mat(word); }

object mat_wrtaction_mat_vec_grp(matrix* m, vector* word, object g)
{ index i; matrix* result;
  check_wts(m,Lierank(grp=g)); check_Ww(word,Ssrank(g)); result=copymatrix(m);
  for (i=0; i<result->nrows; ++i) Wrtaction(result->elm[i],word);
  return (object) result;
}

object mat_wrtorbit_vec_grp(v,g) vector* v; object g;
{ check_rt(v,Ssrank(grp=g)); return (object) Weyl_root_orbit(v->compon); }
/*
object vec_wword_vec_grp(w,g) vector* w; object g;
{ check_wt(w,Lierank(grp=g)); return (object) Wwordv(w->compon,-1); }
*/
/*
object vec_wword_mat_grp(m,g) matrix* m; object g;
{ if (m->nrows != m->ncols  ||	m->nrows != Lierank(grp=g))
    error("Matrix should be square, of size Lie rank.\n");
  return (object) Wwordm(m);
}
*/
object bin_classord_vec(lambda) vector* lambda;
{ index l=lambda->ncomp; check_part(lambda->compon,l);
  return (object) Classord(lambda->compon,l);
}

object vec_frompart_vec(lambda) vector* lambda;
{ if (lambda->ncomp==0) error("Partition should not be empty.\n");
  return (object) From_Part_v(lambda->compon,lambda->ncomp);
}

object mat_frompart_mat(m) matrix* m;
{ if (m->ncols==0) error("Partitions should not be empty.\n");
  return (object) From_Part_m(m->elm,m->nrows,m->ncols);
}

object pol_frompart_pol(p) poly* p;
{ if (p->ncols==0) error("Partitions should not be empty.\n");
  return (object) From_Part_p(p);
}

object vec_nextpart_vec(v) vector* v;
{ index l=v->ncomp; vector* result;
  if (check_part(v->compon,l)==0) return (object) v;
  while (l>0 && v->compon[l-1]==0) l--;
  result=mkvector(l+1);
  copyrow(v->compon,result->compon,l); result->compon[l]=0;
  Nextpart (result->compon,l); while (result->compon[l]==0) l--;
  result->ncomp=l+1; return (object) result;
}

object vec_nextperm_vec(v) vector* v;
{ index n=v->ncomp; vector* result;
  result=mkvector(n); copyrow(v->compon,result->compon,n);
  Nextperm(result->compon,n); return (object) result;
}
/*
object vec_nexttab_vec(t) vector* t;
{ index l=t->ncomp; vector* result; entry* res;
  result=check_tabl(t); /* perform tests !!!->* / freemem(result);
  result=mkvector(l); res=result->compon; copyrow(t->compon,res,l);
  Nexttableau(res,l); return (object) result;
}
*/
/*
object bin_ntabl_vec(lambda) vector* lambda;
{ index l=lambda->ncomp; check_part(lambda->compon,l);
  return (object) n_tableaux(lambda->compon,l);
}
*/
object mat_partitions_int(p) entry p;
{ index n=p;
  if(n<0) error("Partitions should be taken of non-negative numbers only.\n");
  return (object) Partitions(n);
}

object vid_prtab_vec(v) vector* v;
{ vector* s; entry* square=v->compon;
  index n= v->ncomp, i,r, d=2, m=10;
  while(n>=m) {d++; m*=10;} /* d=number of positions needed per entry */
  s=check_tabl(v); m=s->ncomp; freemem(s);
  for (r=1; r<=m; r++)
  { for (i=0; i<n; i++) if (square[i]==r) Printf("%*ld",(int)d,(long)i+1);
    Printf("\n");
  }
  Printf("\n"); return (object) NULL;
}

object vec_RS_vec_vec(P,Q) vector* P,* Q;
{ index n=P->ncomp; entry* p,* q; vector* w;
  if (n!=Q->ncomp) error("Tableaux not of same size");
  { index i; vector* sp=check_tabl(P),* sq=check_tabl(Q);
    boolean ok=(sp->ncomp==sq->ncomp);
    if (ok) for(i=0;i<sp->ncomp;i++) if(sp->compon[i]!=sq->compon[i]) ok=0;
    freemem(sp); freemem(sq);
    if(!ok) error("Tableaux not of same shape\n");
  }
  if (isshared(P)) { p=mkintarray(n); copyrow(P->compon,p,n); }
  else p=P->compon;
  if (isshared(Q)) { q=mkintarray(n); copyrow(Q->compon,q,n); }
  else q=Q->compon;
  w=mkvector(n); Robinson_Schensted(p,q,n,w->compon);
  if (p!=P->compon) freearr(p); if (q!=Q->compon) freearr(q);
  return (object) w;
}

object mat_RS_vec(W) vector* W;
{ index i,n=W->ncomp; entry* w=W->compon; matrix* pq;
  for(i=0; i<n; i++)
    if (w[i]<1 || w[i]>n) error("No permutation: entry out of range.\n");
  pq=mkmatrix(2,n); Schensted_Robinson(w,n,pq->elm[0],pq->elm[1]);
  return (object) pq;
}

object int_signpart_vec(v) vector* v;
{ entry* lambda=v->compon; index l=v->ncomp;
  check_part(lambda,l); return (object) mkintcel(Sign_part(lambda,l));
}

object vec_shape_vec(v) vector* v; { return (object) check_tabl(v); }

object bin_symchar_vec_vec(vector* a,vector* b)
{ entry* lambda=a->compon,* mu=b->compon;
  index l=a->ncomp,m=b->ncomp,n=check_part(lambda,l);
  if (n!=check_part(mu,m))
    error ("Partitions should be of the same number.\n");
  return (object) MN_char_val(lambda,mu,l,m);
}

object poly_symchar_vec(a) vector* a;
{ return (object) MN_char(a->compon,a->ncomp); }

object mat_symorbit_vec(v) vector* v;
{ return (object) Permutations(v->compon,v->ncomp); }

object mat_tableaux_vec(lambda) vector* lambda;
{ return (object) Tableaux(lambda->compon,lambda->ncomp);}

object vec_topart_vec(v) vector* v;
{ return (object) To_Part_v(v->compon,v->ncomp); }

object mat_topart_mat(m) matrix* m;
{ return (object) To_Part_m(m->elm,m->nrows,m->ncols); }

object pol_topart_pol(p) poly* p; { return (object) To_Part_p(p); }

object vec_transpart_vec(v) vector* v;
{ entry* lambda=v->compon; index l=v->ncomp;
  check_part(lambda,l); return (object) Trans_part(lambda,l);
}


object pol_adams_int_vec_grp(d,v,g) entry d; vector* v; object g;
{ index r=Lierank(grp=g),n=d;
  if (n<=0) error("Scalar factor in Adams should be positive.\n");
  check_wt(v,r); return (object) Adams(n,Pol_from_vec(v));
}

object pol_adams_int_pol_grp(d,p,g) entry d; poly* p; object g;
{ index r=Lierank(grp=g),n=d;
  if (n<=0) error("Scalar factor in Adams should be positive.\n");
  check_pol(p,r); return (object) Adams(n,p);
}

object pol_adjoint_grp(g) object g;
{ grp=g; return (object) Adjoint(grp); }

object pol_altdom_pol_grp(p,g) poly* p; object g;
{ check_pol(p,Lierank(grp=g)); return (object) Alt_dom(p); }

object pol_altdom_vec_grp(v,g) vector* v; object g;
{ check_wt(v,Lierank(grp=g)); return (object) Alt_dom(Pol_from_vec(v)); }

object pol_altdom_pol_vec_grp(p,w,g) poly* p; vector* w; object g;
{ check_Ww(w,Ssrank(grp=g)); check_pol(p,Lierank(g));
  return (object) Alt_dom_w(p,w);
}

object pol_altdom_vec_vec_grp(v,w,g) vector* v,* w; object g;
{ check_Ww(w,Ssrank(grp=g)); check_wt(v,Lierank(g));
  return (object) Alt_dom_w(Pol_from_vec(v),w);
}

object pol_alttensor_int_pol_grp(d,p,g) entry d; object g; poly* p;
{ index r=Lierank(grp=g),n=d;
  if (n<0) error("Cannot take negative tensor power.\n");
  p=check_pol(p,r); return (object) SAtensor(true,n,p);
}

object pol_alttensor_int_vec_grp(d,v,g) entry d; object g; vector* v;
{ index r=Lierank(grp=g),n=d;
  if (n<0) error("Cannot take negative tensor power.\n");
  check_wt(v,r); return (object) SAtensor(true,n,Pol_from_vec(v));
}

object pol_altwsum_pol_grp(p,g) poly* p; object g;
{ check_pol(p,Lierank(grp=g)); return (object) alt_Wsum(p); }

object pol_altwsum_vec_grp(v,g) vector* v; object g;
{ check_wt(v,Lierank(grp=g)); return (object) alt_Wsum(Pol_from_vec(v)); }

object pol_branch_vec_grp_mat_grp(v,h,m,g) vector *v; object h,m,g;
{ index R=Lierank(g),r=Lierank(grp=h); entry* lambda=v->compon;
  check_wt(v,R);
  if (m->m.nrows!=R)
    error ("Number of rows of restriction matrix should match Lie rank.\n");
  if (m->m.ncols!=r) error (
    "Number of columns of restriction matrix should match rank subgroup.\n");
  testdom(lambda,g); return (object) Branch_irr(lambda,m->m.elm,g);
}

object pol_branch_pol_grp_mat_grp(p,h,m,g) poly *p; object h,m,g;
{ index R=Lierank(g),r=Lierank(grp=h);
  p=check_pol(p,R);
  if (m->m.nrows!=R)
    error ("Number of rows of restriction matrix should match Lie rank.\n");
  if (m->m.ncols!=r)
    error ("Number of columns of restriction matrix\
 should match rank subgroup.\n");
  { index i; for (i=0; i<p->nrows; i++) testdom(p->elm[i],g);}
  return (object) Branch(p,m->m.elm,g);
}
/*
object pol_collect_pol_grp_mat_grp(p,h,m,g) poly* p; matrix* m; object h,g;
{ index i,r=Lierank(grp=h),R=Lierank(g); check_pol(p,r);
  for(i=0;i<p->nrows;i++) testdom(p->elm[i],h);
  if (m->nrows!=r) error ("Number of rows of inverse restriction matrix\
 should match rank subgroup.\n");
  if (m->ncols!=R) error ("Number of columns of inverse restriction matrix\
 should match Lie rank.\n");
  return (object) Collect(p,m,1,g);
}
*/
/*
object pol_collect_pol_grp_mat_int_grp(p,h,m,n,g)
  poly* p; matrix* m; intcel* n; object h,g;
{ index i,r=Lierank(grp=h),R=Lierank(g); entry d=n->intval; check_pol(p,r);
  for(i=0;i<p->nrows;i++) testdom(p->elm[i],h);
  if (m->nrows!=r) error ("Number of rows of inverse restriction matrix\
 should match rank subgroup.\n");
  if (m->ncols!=R) error ("Number of columns of inverse restriction matrix\
 should match Lie rank.\n");
  if (d<=0) error("Denominator in collect should be positive.\n");
  return (object) Collect(p,m,d,g);
}
*/
object vec_contragr_vec_grp(v,g) vector* v; object g;
{ check_wt(v,Lierank(grp=g)); return (object) Contragr(v,grp); }

object pol_contragr_pol_grp(p,g) poly* p; object g;
{ check_pol(p,Lierank(grp=g)); return (object) Contragr_p(p,grp); }

object pol_decomp_pol_grp(p,g) poly* p; object g;
{ index i,r=Lierank(grp=g);
  check_pol(p,r); for(i=0; i<p->nrows; i++) testdom(p->elm[i],g);
  return (object) Decomp(p);
}

object pol_Demazure_pol_vec_grp(p,w,g) poly* p; vector* w; object g;
{ check_pol(p,Lierank(grp=g)); check_Ww(w,Ssrank(g));
  return (object) Demazure(p,w);
}

object pol_Demazure_vec_vec_grp(v,w,g) vector* v,* w; object g;
{ check_wt(v,Lierank(grp=g)); check_Ww(w,Ssrank(g));
  return (object) Demazure(Pol_from_vec(v),w);
}

object pol_Demazure_pol_grp(p,g) poly* p; object g;
{ index i,s=Ssrank(grp=g); entry* minus_rho=mkintarray(s); vector* w;
  poly*result; check_pol(p,Lierank(grp=g));
  for (i=0; i<s; i++) minus_rho[i]= -1;
  w=Wwordv(minus_rho,Numproots(g)); freearr(minus_rho);
  result=Demazure(p,w); freemem(w); return (object) result;
}

object pol_Demazure_vec_grp(v,g) vector* v; object g;
{ index i,s=Ssrank(grp=g); entry* minus_rho=mkintarray(s); vector* w;
  poly*result; check_wt(v,Lierank(grp=g));
  for (i=0; i<s; i++) minus_rho[i]= -1;
  w=Wwordv(minus_rho,Numproots(g)); freearr(minus_rho);
  result=Demazure(Pol_from_vec(v),w); freemem(w); return (object) result;
}

object bin_dim_vec_grp(v,g) vector* v; object g;
{ index r=Lierank(grp=g); vector* t=(check_wt(v,r),Dominant(v));
  bigint* result=DimIrr(t->compon); freemem(t); return (object) result;
}

object bin_dim_pol_grp(p,g) poly* p; object g;
{ check_pol(p,Lierank(grp=g)); return (object) Dim(p); }

object pol_domchar_vec_grp(v,g) vector* v; object g;
{ check_wt(v,Lierank(grp=g)); testdom(v->compon,g);
  return (object) Domchar_irr(v->compon,NULL);
}

object pol_domchar_pol_grp(p,g) poly* p; object g;
{ check_pol(p,Lierank(grp=g));
  return (object) Domchar_p(p); /* Domchar_p does testdom itself */ 
}

object bin_domchar_vec_vec_grp(lambda,w,g) vector* lambda,* w; object g;
{ index r=Lierank(grp=g); entry* mu=mkintarray(r); copyrow(w->compon,mu,r);
  check_wt(lambda,r); check_wt(w,r); testdom(lambda->compon,g);
  make_dominant(mu);
  { poly* t=Domchar_irr(lambda->compon,mu); bigint* result=t->coef[0];
    freearr(mu); freemem(t); return (object) result;
  }
}

object bin_domchar_pol_vec_grp(p,w,g) poly* p; vector* w; object g;
{ index i,r=Lierank(grp=g); entry* mu=mkintarray(r); bigint* result=null;
  check_pol(p,r); check_wt(w,r); copyrow(w->compon,mu,r); make_dominant(mu);
  for (i=0; i<p->nrows; i++)
  { poly* t=(testdom(p->elm[i],g),Domchar_irr(p->elm[i],mu));
    result=add(result,mult(p->coef[i],t->coef[0])); freemem(t);
  }
  freearr(mu); return (object) result;
}

object pol_lrtensor_vec_vec(lambda,mu) vector* lambda,* mu;
{ index l=lambda->ncomp;
  if (l!=mu->ncomp) error
    ("partitions for LR_tensor should have same number of parts.\n");
  check_part(lambda->compon,l); check_part(mu->compon,l);
  return (object) LR_tensor_irr(lambda->compon,mu->compon,l);
}

object pol_lrtensor_pol_pol(p,q) poly* p,* q;
{ index i,l=p->ncols;
  if (l!=q->ncols) error
    ("exponents for LR_tensor should have same number of parts.\n");
  for (i=0; i<p->nrows; i++) check_part(p->elm[i],l);
  for (i=0; i<q->nrows; i++) check_part(q->elm[i],l);
  return (object) LR_tensor(p,q);
}

object pol_plethysm_vec_pol_grp(vector* lambda, poly* p, object g)
{ index n=check_part(lambda->compon,lambda->ncomp);
  check_pol(p,Lierank(grp=g));
  return (object) Plethysm(lambda->compon,lambda->ncomp,n,p);
}


object pol_plethysm_vec_vec_grp(vector* lambda,vector* mu,object g)
{ index n=check_part(lambda->compon,lambda->ncomp);
  check_wt(mu,Lierank(grp=g));
  return (object) Plethysm(lambda->compon,lambda->ncomp,n,Pol_from_vec(mu));
}

object pol_ptensor_int_pol_grp(d,p,g) entry d; object g; poly* p;
{ index r=Lierank(grp=g),n=d;
  if (n<0) error("Cannot take negative tensor power.\n");
  p=check_pol(p,r); return (object) Ptensor(n,p);
}

object pol_ptensor_int_vec_grp(d,v,g) entry d; object g; vector* v;
{ index r=Lierank(grp=g),n=d;
  if (n<0) error("Cannot take negative tensor power.\n");
  check_wt(v,r); return (object) Ptensor(n,Pol_from_vec(v));
}
/*
object mat_resmat_mat_grp(m,g) matrix* m; object g;
{ check_rts(m,Ssrank(grp=g)); return (object) Resmat(m); }
*/
object pol_spectrum_vec_vec_grp (wt,t,g) vector* wt,* t; object g;
{ index r=Lierank(grp=g); check_wt(wt,r); check_toral(t,r,1);
  return (object) Spectrum(Pol_from_vec(wt),t);
}

object pol_spectrum_pol_vec_grp (p,t,g) poly* p; vector* t; object g;
{ index r=Lierank(grp=g); check_pol(p,r); check_toral(t,r,1);
  return (object) Spectrum(p,t);
}

object pol_symtensor_int_pol_grp(d,p,g) entry d; object g; poly* p;
{ index r=Lierank(grp=g),n=d;
  if (n<0) error("Cannot take negative tensor power.\n");
  p=check_pol(p,r); return (object) SAtensor(false,n,p);
}

object pol_symtensor_int_vec_grp(d,v,g) entry d; object g; vector* v;
{ index r=Lierank(grp=g),n=d;
  if (n<0) error("Cannot take negative tensor power.\n");
  check_wt(v,r); return (object) SAtensor(false,n,Pol_from_vec(v));
}

object pol_tensor_pol_pol_grp(p,q,g) poly* p,* q; object g;
{ entry r=Lierank(grp=g); p=check_pol(p,r); q=check_pol(q,r);
  return (object) Tensor(p,q);
}

object pol_tensor_vec_vec_grp(x,y,g) vector* x,* y; object g;
{ entry r=Lierank(grp=g); check_wt(x,r); check_wt(y,r);
  return (object) Tensor(Pol_from_vec(x),Pol_from_vec(y));
}

object bin_tensor_pol_pol_vec_grp(p,q,nu,g)
  poly* p,* q; vector* nu; object g;
{ entry r=Lierank(grp=g); p=check_pol(p,r); q=check_pol(q,r);
  check_wt(nu,r); return (object) Tensor_coef(p,q,nu);
}

object bin_tensor_vec_vec_vec_grp(x,y,z,g) vector* x,* y,* z; object g;
{ entry r=Lierank(grp=g); check_wt(x,r); check_wt(y,r); check_wt(z,r);
  return (object) Tensor_coef(Pol_from_vec(x),Pol_from_vec(y),z);
}

object pol_vdecomp_pol_grp(p,g) poly* p; object g;
{ index i,r=Lierank(grp=g);
  check_pol(p,r); for(i=0; i<p->nrows; i++) testdom(p->elm[i],g);
  return (object) Vdecomp(p);
}

object pol_vdecomp_vec_grp(v,g) vector* v; object g;
{ poly* p=Pol_from_vec(v),* result;
  check_wt(v,Lierank(grp=g)); testdom(v->compon,g);
  result=Vdecomp(p); freepol(p); return (object) result;
}
