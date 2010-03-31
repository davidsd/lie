#include  "lie.h"
#define local  static
#define is_odd(x) ((x&1)!=0)
#define set_vec(v,s) \
  { char* p=s; int i=0; do v->compon[i++]=*p++-'0'; while (*p!='\0'); \
    v->ncomp=i; \
  }


local simpgrp* the_g;  /* the simple group in question */
local index rnk;     /* Lie rank of full group */
local index eps_dim; /* dimension of $\eps$-space */
local index perm_size;	/* number of entries to be permuted */
local char subtype;  /* type of classical subgroup used for suborbits */

typedef void (*trans)(entry const* from, entry* to);
local trans to_e, from_e;
  

local boolean alternate; 

local index cox_order, X_order;
local matrix* cox,* X_elt[9]; /* matrices of size |[eps_dim]|{}|[eps_dim]| */
local matrix* suborbit_reps; /* size |[cox_order*X_order]|{}|[eps_dim]| */
local vector* temp1,* temp2;  


local void w2eAn(entry const* w,entry* e)
{ index i=rnk; entry sum=0; while (e[i]=sum,--i>=0) sum+=w[i]; }

local void e2wAn(entry const* e,entry* w)
{ index i; for (i=0; i<rnk; ++i) w[i]=e[i]-e[i+1]; }

local void w2eBn(entry const* w,entry* e)
{ index i=rnk-1; entry sum=w[i]; while (e[i]=sum,--i>=0) sum+=2*w[i]; }

local void e2wBn(entry const* e,entry* w)
{ index i; for (i=0; i<rnk-1; ++i) w[i]=(e[i]-e[i+1])/2; w[i]=e[i]; }

local void w2eCn(entry const* w,entry* e)
{ index i=rnk-1; entry sum=w[i]; while (e[i]=sum,--i>=0) sum+=w[i]; }

local void e2wCn(entry const* e,entry* w)
{ index i; for (i=0; i<rnk-1; ++i) w[i]=e[i]-e[i+1]; w[i]=e[i]; }

local void w2eDn(entry const* w,entry* e)
{ index i=rnk-1; entry sum=w[i]-w[i-1]; while(e[i]=sum,--i>=0) sum+=2*w[i]; }

local void e2wDn(entry const* e,entry* w)
{ index i; for (i=0; i<rnk-1; ++i) w[i]=(e[i]-e[i+1])/2; w[i]=w[i-1]+e[i]; }

local void w2eE68(entry const* w,entry* e)
{ index i, u=perm_size;
  entry sum =e[u-1]= w[1]-w[2], sumsum=4*w[0] -sum;
  for (i=2; i<rnk; ++i) sumsum+=(e[u-i]=sum+= 2*w[i]);
  e[rnk==6 ? 5 : 0]= -sumsum;
}

local void e2wE68(entry const* e,entry* w)
{ index i,u= perm_size; entry sum=e[rnk==6 ? 5 : 0];
  for (i=rnk-1; i>=2; --i) {sum+=e[u-i]; w[i]=(e[u-i]-e[u-i+1])/2;}
  w[1]=w[2]+e[u-1]; w[0]=(e[u-1]-sum)/4;
}

local void w2eE7(entry const* w,entry* e)
{ index i; entry sum=e[7]=0;  for (i=6; i>=2; --i) e[i]=sum+=w[i];
  e[1]=sum+w[0];
  e[0]=e[6]+e[5]+e[4]-e[3]-e[2]-e[1]-2*w[1];
}

local void e2wE7(entry const* e,entry* w)
{ index i;  w[0]=e[1]-e[2];  for(i=2; i<7; i++) w[i]=e[i]-e[i+1];
  w[1]=(e[7]+e[6]+e[5]+e[4]-e[3]-e[2]-e[1]-e[0])/2;
}

local void w2eF4(entry const* w,entry* e)
{ e[1]=(e[2]=(e[3]=w[2])+2*w[1])+2*w[0]; e[0]=-2*w[3]-e[1]-e[2]-e[3]; }

local void e2wF4(entry const* e,entry* w)
{ w[2]=e[3]; w[1]=(e[2]-e[3])/2; w[0]=(e[1]-e[2])/2;
  w[3]=-(e[0]+e[1]+e[2]+e[3])/2;
}

local void w2eG2(entry const* w,entry* e) 
{ e[2]=0; e[0]=-w[0]-(e[1]=w[1]); }

local void e2wG2(entry const* e,entry* w) 
{ w[0]=e[2]-(w[1]=e[1]-e[2])-e[0]; }

#if 0
local void r2eAn (entry const* r,entry* e)
{ index i; e[0]=r[0]; for (i=1;i<rnk;++i) e[i]=r[i]-r[i-1]; e[i]=-r[i-1]; }

local void e2rAn (entry const* e,entry* r)
{ index i; entry sum=r[0]=e[0]; for (i=1;i<rnk;++i) r[i]=sum+=e[i]; }

local void r2eBn (entry const* r,entry* e)
{ index i; e[0]=r[0]; for (i=1;i<rnk;++i) e[i]=r[i]-r[i-1]; }


#define  e2rBn e2rAn /* these are identical */

local void r2eCn (entry const* r,entry* e)
{ index i; e[0]=r[0]; for (i=1;i<rnk;++i) e[i]=r[i]-r[i-1];
  e[i-1]+=r[i-1];
}

local void e2rCn (entry const* e,entry* r)
{ index i; entry sum=r[0]=e[0]; for (i=1;i<rnk;++i) r[i]=sum+=e[i];
  r[i-1]/=2;
}

local void r2eDn (entry const* r,entry* e)
{ index i; e[0]=r[0]; for (i=1;i<rnk;++i) e[i]=r[i]-r[i-1]; e[i-2]+=r[i-1]; }

local void e2rDn (entry const* e,entry* r)
{ index i; entry sum=r[0]=e[0];  for (i=1;i<rnk;++i) r[i]=sum+=e[i];
  r[i-2]-=r[i-1]/=2;
}
#endif

#if 0
local void r2eE68 (entry const* r,entry* e)
{ index i=rnk-1,u=perm_size; entry a=r[0];  e[rnk==6 ? 5 : 0]=-a; 
  e[u-i]=2*r[i]-a;  while (--i>1) e[u-i]=2*(r[i]-r[i+1])-a;
  e[u-1]=2*(r[1]-r[2])+a; e[u-2]+=2*r[1];
}

local void e2rE68 (entry const* e,entry* r)
{ index i,u=perm_size; entry sum=0,a=e[rnk==6 ? 5 : 0];  r[0]=-a;
  for (i=rnk-1; i>2; --i) r[i]=(sum+=e[u-i]-a)/2;
  r[1]=(sum+e[u-2]+e[u-1])/4; r[2]=r[1]-(a+e[u-1])/2;
}

local void r2eE7 (entry const* r,entry* e)
{ entry a=r[1]; 
  e[0]=-a; e[1]=2*r[0]-a; e[2]=2*(r[2]-r[0])-a; e[3]=2*(r[3]-r[2])-a;
  e[4]=a+2*(r[4]-r[3]); e[5]=a+2*(r[5]-r[4]); e[6]=a+2*(r[6]-r[5]);
  e[7]=a-2*r[6];
}

local void e2rE7 (entry const* e,entry* r)
{ entry a=e[0];  r[1]=-a;
  r[3]=(r[2]=(r[0]=(e[1]-a)/2)+(e[2]-a)/2)+(e[3]-a)/2;
  r[6]=(r[5]=(r[4]=r[3]+(e[4]+a)/2)+(e[5]+a)/2)+(e[6]+a)/2;
}
#endif

#if 0
local void r2eF4 (entry const* r,entry* e)
{ e[0]=-r[3]; e[1]=2*r[0]-r[3]; e[2]=2*(r[1]-r[0])-r[3];
  e[3]=2*(r[2]-r[1])-r[3];
}

local void e2rF4 (entry const* e,entry* r)
{ r[3]=-e[0];
  r[2]=(r[1]=(r[0]=(e[1]-e[0])/2)+(e[2]-e[0])/2)+(e[3]-e[0])/2;
}

local void r2eG2 (entry const* r,entry* e) 
{ e[1]=r[0]+(e[0]=-r[1]); e[2]=-e[0]-e[1]; }

local void e2rG2 (entry const* e,entry* r) 
{ r[0]=(r[1]=-e[0])+e[1]; }
#endif


local void normalform(entry* w)
{ index i, parity=0;
  if (subtype=='A')
  { 
    { index i=0, j=perm_size-1;
      sortrow(w,perm_size); while (i<j) swap(&w[i++],&w[j--]);
    }
    if (w[0]!=0)
      /* then add constant to entries to make |w[0]==0| */
      for(i=perm_size-1; i>=0; --i) w[i]-=w[0];
  }
  else
  { for(i=0; i<perm_size; ++i) 
      if (w[i]<0) {w[i]*=-1; ++parity;} /* strip signs */
    
    { index i=0, j=perm_size-1;
      sortrow(w,perm_size); while (i<j) swap(&w[i++],&w[j--]);
    }
    if (subtype=='D' && is_odd(parity)) w[0]*=-1;
  }
}

local void tabulate_suborbits(entry* v)
{ index i, k=0;
  entry** p=suborbit_reps->elm; /* next row of |suborbit_reps| */
  entry* cur=temp1->compon,* alt=temp2->compon; /* will be swapped repeatedly */
  copyrow(v,cur,rnk); /* make working copy of |v|, which will be modified */

  do /* effectively |for (k=0; k<cox_order; ++k)|,
	but with exit in middle of loop body */
  { for (i=0; i<X_order; ++i) /* elements of $X$ */
    { mulvecmatelm(cur,X_elt[i]->elm,alt,rnk,rnk); /* |alt=cur*X_elt[i]| */
      (*to_e)(alt,*p); normalform(*p++);
	/* store converted vector after normalising it */
    }
    if (++k>=cox_order) break;
      /* note that if |cox_order==1|, |cox| is never used */
    
    { entry* t=cur; mulvecmatelm(cur,cox->elm,alt,rnk,rnk); cur=alt; alt=t; }
  } while(true);
  suborbit_reps->nrows=cox_order*X_order;
  Unique(suborbit_reps,cmpfn); /* remove duplicate suborbits */
}

void Weylloopinit(simpgrp* g)
{ the_g=g;  rnk=g->lierank;
    /* save values used by other functions */
  
  { int i;
    static trans tab [8][2]=
    { { w2eAn, e2wAn }
    , { w2eBn, e2wBn }
    , { w2eCn, e2wCn }
    , { w2eDn, e2wDn }
    , { w2eE68,e2wE68 }
    , { w2eF4, e2wF4 }
    , { w2eG2, e2wG2 }
    , { w2eE7, e2wE7 }
    };
  
    if (the_g->lietype=='E' && rnk==7) i=7;
    else 
         #if 'G'-'A'==6
           i=the_g->lietype-'A';
         #else
           { static char s[]="ABCDEFG"; i= (int)(strchr(s,the_g->lietype)-s);	}
         #endif
    subtype= "ABCDDBAA"[i];
    eps_dim= subtype=='A' ? rnk+1 : rnk;
    perm_size = the_g->lietype=='E' && rnk==6 ? eps_dim-1 : eps_dim;
  to_e=tab[i][0]; from_e=tab[i][1];
  }
  temp1=mkvector(eps_dim); temp2=mkvector(eps_dim); /* scratch vectors */
  X_elt[0]=mat_id(rnk);

  if (strchr("ABCD",g->lietype)!=NULL) { cox_order=X_order=1; cox=NULL; }
  else
  
  { matrix* tmpmat; index i;
    vector* coxw=mkvector(rnk); /* some Coxeter word */
    if (g->lietype=='E')
    { vector* gen=mkvector(15); /* large enough for largest use */
      if (rnk==6) 
                { X_order=3; cox_order=12;
                set_vec(coxw,"625431") cox=simp_Weylmat(coxw,the_g);
                set_vec(gen,"2431") X_elt[1]=simp_Weylmat(gen,the_g);
                set_vec(gen,"1452431") X_elt[2]=simp_Weylmat(gen,the_g);
                }
   
      else if (rnk==7) 
                     { X_order=4; cox_order=18;
                     set_vec(coxw,"1234567") cox=simp_Weylmat(coxw,the_g);
                     set_vec(gen,"76543215342") X_elt[1]=simp_Weylmat(gen,the_g);
                       X_elt[2]=Matmult(X_elt[1],X_elt[1]); /* $x^2$ */
                       X_elt[3]=Matmult(tmpmat=Matmult(X_elt[2],X_elt[2]),X_elt[1]); /* $x^5$ */
                       freemem(tmpmat);
                     }
   
      else 
           { X_order=9; cox_order=15;
             set_vec(coxw,"75328641") cox=simp_Weylmat(coxw,the_g);
             set_vec(gen,"743245613452431")
             X_elt[1]=simp_Weylmat(gen,the_g); /* $x_1$ */
             tmpmat=Matmult(X_elt[1],X_elt[1]);
             X_elt[2]=Matmult(tmpmat,X_elt[1]);  /* $x_1^3$ */
             X_elt[3]=Matmult(tmpmat,tmpmat);     /* $x_1^4$ */
             freemem(tmpmat);
             set_vec(gen,"867563452431")
             X_elt[4]=simp_Weylmat(gen,the_g); /* $x_2$ */
             for (i=5; i<9; ++i) X_elt[i]=Matmult(X_elt[4],X_elt[i-1]);
               /* $x_2^2$, $x_2^3$, $x_2^4$, $x_2^5$ */
           }
      freemem(gen);
    }
    else /* type |F4| or |G2| */
    { if (g->lietype=='F') { cox_order=3; set_vec(coxw,"1234") }
      else { cox_order=2; set_vec(coxw,"12") }
      cox=simp_Weylmat(coxw,the_g); X_order=1;
    }
    freemem(coxw);
  }
  suborbit_reps=mkmatrix(cox_order*X_order, eps_dim);
}

void Weylloopexit(void)
{ freemem(temp1); freemem(temp2); freemem(suborbit_reps);
  { index i; for (i=0; i<X_order; ++i) freemem(X_elt[i]); }
  if (cox!=NULL) freemem(cox);
}

void Weylloop(void(* action )(entry*),entry* v)
{ entry* tmp=temp1->compon;
  index k,* inx=NULL; /* indices of non-zero entries in |w| */

  tabulate_suborbits(v);

  if (subtype!='A') inx=alloc_array(index,perm_size+1);
    /* one extra for the terminating |-1| */
  for (k=0; k<suborbit_reps->nrows; ++k)
    /* traverse suborbit representatives */
  { entry* w=suborbit_reps->elm[k]; /* it is safe to modify this vector */
    if (subtype=='A')
      do { (*from_e)(w,tmp); (*action)(tmp); } while (Nextperm(w,perm_size));
    else
    { alternate= subtype=='D' && w[0]!=0; /* whether type $D_n$ and no 0's */
      do /* traverse permutations */
      { unsigned long signcount=0;
	
	{ index i,j=0;
	  { for (i=0; i<perm_size; ++i) if (w[i]!=0) inx[j++]=i;} inx[j]= -1;
	}
	do /* traverse sign combinations */
	{ (*from_e)(w,tmp); (*action)(tmp);
	  
	    { index i=0; unsigned long bits=signcount++;
	      if (alternate) {	w[0]*=-1; ++i; }
	        /* |inx[i]==i| for all |i<perm_size| here */
	      while (inx[i]>=0 && is_odd(bits)) { bits>>=1; ++i; }
	      if (inx[i]<0)
	        { if (--i>=0) w[inx[i]]*=-1; break;} /* done; restore |w| and quit */
	      w[inx[i]]*=-1; /* otherwise change this one sign */
	    }
	} while(true);
	
	{ boolean minus= alternate && w[0]<0;
	    /* if this holds, |w[0]| is unique negative entry */
	  if (minus) w[0]*=-1;
	  if (!Nextperm(w,perm_size)) break;
	  if (minus) w[0]*=-1;
	}
      } while(true);
    }
  }
  if (inx!=NULL) freearr(inx);
}

