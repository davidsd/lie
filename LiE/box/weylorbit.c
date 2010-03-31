#include  "lie.h"
#define local  static
#define NDEBUG 


local simpgrp* the_g;  /* the simple group in question */
local index rnk,n_pos_roots;     /* Lie rank, number of positive roots */
local void (* the_Weyl_loop) (void(*)(entry*),entry*);
 /* the function doing the Weyl loop */
local entry* word;
        /* stack of reflection indices, describing how we got here */
local index depth; /* index into |word|, current depth of word */
local entry* max_neighbour; /* last larger index of neighbour in diagram */
local entry* pred_coef; 


void generic_Weyl_loop(void(* action )(entry*),entry* mu);

void A_Weyl_loop(void(* action )(entry*),entry* mu);
void C_Weyl_loop(void(* action )(entry*),entry* mu);
void ABF_Weyl_loop(void(* action )(entry*),entry* mu);
void CG_Weyl_loop(void(* action )(entry*),entry* mu);


void Weylloopinit(simpgrp* g)
{ index i;
  the_g=g;  rnk=g->lierank; n_pos_roots=simp_numproots(g);
    /* save values used by other functions */
  the_Weyl_loop=
    strchr("ABF",g->lietype)!=NULL ? ABF_Weyl_loop : generic_Weyl_loop;
  if (g->lietype=='A') the_Weyl_loop= A_Weyl_loop;
  word= mkintarray(n_pos_roots);
  max_neighbour= mkintarray(2*rnk); pred_coef=&max_neighbour[rnk];
  for (i=0; i<rnk-1; ++i)
  { max_neighbour[i]=i+1; pred_coef[i]=1; } /* defaults */
  max_neighbour[i]=0; /* last node has no successor neighbour */
  if (g->lietype=='E') { max_neighbour[0]=2; max_neighbour[1]=3; }
  else if (g->lietype=='D') { max_neighbour[i-1]=0; max_neighbour[i-2]=i; }
  pred_coef[i]= g->lietype=='C' ? 2 : g->lietype=='G' ? 3 : 1;
    /* last Cartan coefficient with predecessor is not |1| in types $C,G$ */
}

void Weylloop(void(* action )(entry*),entry* mu)
{ (*the_Weyl_loop)(action,mu); } 

void Weylloopexit(void)
{ freearr(word); freearr(max_neighbour); }

void generic_Weyl_loop(void(* action )(entry*),entry* mu)
{ index j=rnk;
  depth=0; simp_make_dominant(mu,the_g);
  do
  { if (--j>=0)
    do
    { 
      { if (mu[j]<=0) break; /* child must be further from dominant chamber */
        if (depth>0) /* at level |0| every |s_j| not stabilising $\mu$ is good */
        { index i=word[depth-1]; /* index of the reflection that brought us here */
          if (j>i) /* every |s_j| with $j<i$ not stabilising $\mu$ is also good */
          { if (the_g->lietype=='E' && j==i+1 && i<2) break;
            if (mu[i]+pred_coef[j]*mu[j]<0) break;
            if (j==i+2 && (the_g->lietype=='E' && j==3
                            ? mu[2]+mu[3]<0
                            : mu[j-1]<0
                            )
               ) break;
          }
        }
      }
      assert(depth<n_pos_roots);
        /* stack overflow should not be possible */
      word[depth++]=j; /* push |j| on stack */
      simp_w_refl(mu,j,the_g); /* and move to weight at child node */
      if (max_neighbour[j]==0) break; /* no followers, start with |j=j-1| */
      j=max_neighbour[j]; /* otherwise, start considering last follower */
    } while(1);
    else
    { action(mu);
      if (--depth<0) break; /* if nothing left on stack, we are done */
      j=word[depth]; /* otherwise pop |j| from stack */
      simp_w_refl(mu,j,the_g); /* and find weight at parent node */
    }
  } while(1);
}

void ABF_Weyl_loop(void(* action )(entry*),entry* mu)
{ index j=rnk;
  depth=0; simp_make_dominant(mu,the_g);
  do
  { while (--j>=0)
      while (mu[j]>0 && (depth==0 || j<=word[depth-1] || mu[j-1]+mu[j]>=0))
      { word[depth++]=j; /* push |j| on stack */
      	simp_w_refl(mu,j,the_g); /* and move to weight at child node */
      	if (j==rnk-1) break; /* no followers, start with |j=j-1| */
      	++j; /* otherwise, start considering next node */
      }
    action(mu);
    if (--depth<0) break; /* if nothing left on stack, we are done */
    j=word[depth]; /* otherwise pop |j| from stack */
    simp_w_refl(mu,j,the_g); /* and find weight at parent node */
  } while(1);
}

void A_Weyl_loop(void(* action )(entry*),entry* mu)
{ index j=rnk-1;
  depth=0; simp_make_dominant(mu,the_g); goto enter;
  do
  { entry muj=mu[j=word[depth]];  mu[j]= -muj;
    if (j<rnk-1) mu[j+1]+=muj;
    if (j>0)
    { mu[--j]+=muj;
    enter:
      do
  	while (mu[j]>0 && (depth==0 || j<=word[depth-1] || mu[j-1]+mu[j]>=0))
  	{ entry muj=mu[j];  word[depth++]=j; /* push |j| on stack */
  	   mu[j]= -muj;  if (j>0) mu[j-1]+=muj;
  	  if (j==rnk-1) break; /* no followers, consider |j=j-1| next */
  	  mu[++j]+=muj;
          /* otherwise complete reflection, and consider |j+1| next */
  	}
      while (--j>=0);
    }
    action(mu);
  } while(--depth>=0); /* if nothing left on stack, we are done */
}

void C_Weyl_loop(void(* action )(entry*),entry* mu)
{ index j=rnk;
  depth=0; simp_make_dominant(mu,the_g);
  do
  { while (--j>=0)
      while (mu[j]>0 &&
        (depth==0 || j<=word[depth-1]
        || mu[j-1]+(j!=rnk-1 ? mu[j] : 2*mu[j])>=0))
      { entry muj=mu[j];  word[depth++]=j; /* push |j| on stack */
      	{ if (j>0) mu[j-1]+=muj; } mu[j]= -muj;
      	if (j==rnk-1) { mu[j-1]+=muj; break; } /* add again for long root */
        mu[++j]+=muj; /* otherwise complete reflection, consider |j+1| next */
      }
    action(mu);
    if (--depth<0) break; /* if nothing left on stack, we are done */
    j=word[depth]; /* otherwise pop |j| from stack */
    { entry muj=mu[j];  mu[j]= -muj;
      if (j==rnk-1) mu[j-1]+=2*muj;
      else { mu[j+1]+=muj; if (j>0) mu[j-1]+=muj; }
    }
  } while(1);
}

void CG_Weyl_loop(void(* action )(entry*),entry* mu)
{ index j=rnk;
  depth=0; simp_make_dominant(mu,the_g);
  do
  { if (--j>=0)
    { do
      { 
        { if (mu[j]<=0) break; /* child must be further from dominant chamber */
          if (depth>0) /* at level |0| every |s_j| not stabilising $\mu$ is good */
          { index i=word[depth-1]; /* index of the reflection that brought us here */
            if (j>i && mu[i]+pred_coef[j]*mu[j]<0) break;
          }
        }
  	assert(depth<n_pos_roots);
  	  /* stack overflow should not be possible */
  	word[depth++]=j; /* push |j| on stack */
  	simp_w_refl(mu,j,the_g); /* and move to weight at child node */
  	if (j==rnk-1) break; /* no followers, start with |j=j-1| */
  	++j; /* otherwise, start considering next node */
      } while(1);
    }
    else
    { action(mu);
      if (--depth<0) break; /* if nothing left on stack, we are done */
      j=word[depth]; /* otherwise pop |j| from stack */
      simp_w_refl(mu,j,the_g); /* and find weight at parent node */
    }
  } while(1);
}

