#include "lie.h"

#ifdef __STDC__
static simpgrp* Compontype(entry** a,index n);
static void centroots(matrix* m,entry* h);
#endif

/* given connected component in root graph, get its type */
/* a[0:n-1] is the list of positive roots in root graph */
static simpgrp* Compontype(a,n) entry** a; index n;
{ simpgrp* g; index offset=0;
  if (type_of(grp)==SIMPGRP) g= &grp->s;
  else /* find relevant component of grp: */
  { index l=0,r=0,i=0; entry* p= *a;
    while(!*p++)l++; /* l is index of first nonzero entry in first root */
    do r+=Liecomp(grp,i++)->lierank; while (r<=l);
    g=Liecomp(grp,--i); offset=r-g->lierank;
  }
  if (n==Numproots((object)g)) return g;
  if (n==4) return mksimpgrp('B',2);
  if (n==63) return mksimpgrp('E',7);
/* At this point we have handled the cases B2,E7,E8,F4 and G2 */

  { char g_type=g->lietype,btype='B'; entry arank=1,brank=3, r=g->lierank;
/* find rank of type A system, if any, with n roots */
    { entry n_roots=1;
      while (n!=n_roots) if ((n_roots+= ++arank)>n) {arank=0; break;}
    }
    if (arank && (g_type=='A' || g_type=='F' || g_type=='G'))
      return mksimpgrp('A',arank); /* in these cases it can only be type A */
/* At this point we have also treated all cases where g_type is A or G */
    { entry n_roots=9; /* for type B3/C3 */
      while (n!=n_roots)
      { if ((n_roots+=brank)>n || ++brank>r) {brank=0; break;}
	if (n==n_roots) { btype='D'; break; } else n_roots+=brank;
      }
    }
    if (arank && !brank) return mksimpgrp('A',arank);
/* type C system has no subsystem of type B (except B2): */
    if (btype=='B' && g_type=='C') btype='C';
/* only for g_type='F' and n=9 is there doubt about btype: */
    if (g_type=='F' && n==9)
    { entry i,n_short=0; entry* v;
      for (i=0; i<9; i++)
      { v= &a[i][offset]; if (v[3]==1 || v[2]-v[3]==1) n_short++; }
      return mksimpgrp(n_short==3 ? 'B' : 'C' ,3);
    }
    if (brank && !arank) /* then also n!=36 */ return mksimpgrp(btype,brank);
/* At this point we have both arank and brank positive */

/* The type may be A[arank], btype[brank], or if n=36, E6
   We recognise the A and E types by the number of roots not perpendicular
   to an arbitrarily chosen one. The expected number is
   type A[arank]: 2*arank-1
   type B[brank]: 2*brank-1 (for short root) or 4*brank-7 (long root)
   type C[brank]: 2*brank-1 (for long root) or 4*brank-7 (short root)
   type D[brank]: 4*brank-7
   type E[7]:	  21
   One checks that (apart from the B/C distinction which is already made)
   none of these can be equal under the additional condition
   arank*(arank+1)/2 = if type=B,C then brank^2 else brank*(brank-1) fi (=n),
   except for the irrelevant cases A1:B/C1 and A3:D3 (and A-1:D1).
*/
    { entry* v= *a; index i,not_perp=1; /* for root v itself */
      for (i=1; i<n; i++) if (Inprod(v,a[i])) not_perp++;
      if (not_perp==2*arank-1) return mksimpgrp('A',arank);
      if (n==36 && not_perp==21) return mksimpgrp('E',6);
      return mksimpgrp(btype,brank);
    }
  }
}


/* isolate a component of roots from `a' starting at root `i',collecting all
   roots above it in the same component in the succeeding positions. Return
   the first position not containing a root in this component
*/
index isolcomp(ma,i) matrix* ma; index i;
{ register index next=i+1,j,n=ma->nrows; entry** a=ma->elm;
  do
    for (j=next; j<n; j++) if (Inprod(a[i],a[j]))
      swap_rows(&a[j],&a[next++]);
  while(++i<next);
  return(next);
}

/* remove roots from m that don't centralise h */
static void centroots(m,h) matrix* m; entry* h;
{ index r=Lierank(grp),s=Ssrank(grp),n_ok=0; entry i,value,*hh;

  hh=mkintarray(s); mulmatvecelm(Cartan()->elm,h,hh,s,r);
  for (i=0; i<m->nrows; i++)
  { value=inprow(hh,m->elm[i],s); if (h[r]) value %= h[r];
    if (value==0) swap_rows(&m->elm[i],&m->elm[n_ok++]); /* no row aliases */
  }
  m->nrows=n_ok;
  freearr(hh);
}

matrix* Centroots(mm) matrix* mm; /* matrix of toral elements */
{ entry i; matrix* result=copymatrix(Posroots(grp));
  for(i=0; i<mm->nrows; i++)
  { centroots(result,mm->elm[i]); if (!result->nrows) break; }
  return result;
}

object Centrtype(h) matrix* h; 
{ index n_comp=0,i,next;
  matrix* c=Centroots(h); group* ans=mkgroup(Ssrank(grp));
  ans->toraldim=Lierank(grp);
  for (i=0; i<c->nrows; i=next)
  { next=isolcomp(c,i);
    ans->liecomp[n_comp]=Compontype(&c->elm[i],next-i);
    ans->toraldim -= ans->liecomp[n_comp++]->lierank;
  }
  ans->ncomp=n_comp; freemem(c); return (object)ans;
}
