#include "lie.h"

object Diagram(grp) object grp;
{ register index i,j,k,r; index offset=0; simpgrp* gk; Printf("\n");
/* Repeat over simple components: */
  for (k=0; k<grp->g.ncomp; k++,offset+=r)
  { gk=Liecomp(grp,k); r=gk->lierank;
    if(r<12)
    switch (gk->lietype)
    {	   case 'A':
      for (i=1; i<=r-1; i++) Printf("O---");	     Printf("O\n");
      for (i=1; i<=r; i++) Printf("%-4ld",offset+i); Printf("\n");
    break; case 'B':
      for (i=1; i<=r-2; i++) Printf("O---");	     Printf("O=>=O\n");
      for (i=1; i<=r; i++) Printf("%-4ld",offset+i); Printf("\n");
    break; case 'C':
      for (i=1; i<=r-2; i++) Printf("O---");	     Printf("O=<=O\n");
      for (i=1; i<=r; i++) Printf("%-4ld",offset+i); Printf("\n");
    break; case 'D':
      for (j=0; j<3; j++)
      { for (i=1; i<=4*(r-3); i++) Printf(" ");
	if (j) Printf("|\n"); else Printf("O %ld\n",(long)(offset+r-1));
      }
      for (i=2; i<r; i++) Printf("O---"); Printf("O\n");
      for (i=1; i<=r-2; i++) Printf("%-4ld",offset+i);
      Printf("%ld\n",(long)(offset+r));
    break; case 'E':
      Printf("        O %ld\n        |\n        |\n",(long)(offset+2));
      for (i=2; i<=r-1; i++) Printf("O---");		       Printf("O\n");
      for (i=1; i<=r; i++) if (i!=2) Printf("%-4ld",offset+i); Printf("\n");
    break; case 'F':
      Printf("O---O=>=O---O\n");
      for (i=1; i<=4; i++) Printf("%-4ld",offset+i); Printf("\n");
    break; case 'G':
      Printf("  3\nO=<=O\n%-4ld%ld\n",(long)(offset+1),(long)(offset+2));
    }
    else /* r>=12 */
    { char t=gk->lietype;
      if (t=='D') for (i=0; i<3; i++)
      { for (j=32; --j>=0; ) Printf(" ");
	if (i) Printf("|\n"); else Printf("O %ld\n",(long)(offset+r-1));
      }
      Printf("O---O---O---O-- . . . --O---O---O%sO\n"
	    , t=='B' ? "=>=" : t=='C' ? "=<=" : "---");
      for (i=1; i<=4; i++) Printf("%-4ld",offset+i);
      for (i=0; i<8; i++) Printf(" ");
      for (i=r-(t=='D'?4:3); i<=r; i++) if (t!='D' || i!=r-1)
	Printf("%-4ld",offset+i);
      Printf("\n");
    }
    Printf("%c%ld\n\n",gk->lietype,(long)(gk->lierank));
  } /* End repeat over components */
  if (grp->g.toraldim)
  { if (grp->g.ncomp)
    Printf("With %ld-dimensional central torus.\n\n",(long)grp->g.toraldim);
    else Printf ("A %ld-dimensional torus.\n\n",(long)grp->g.toraldim);
  }
  else if (grp->g.ncomp==0) Printf("Trivial group.\n\n");
  return (object)NULL;
}
