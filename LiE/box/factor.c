#include  "lie.h"
#define trial_limit  32768U 

static digit inc[] = { 1, 2, 2, 4, 2, 4, 2, 4, 6, 2, 6 };
  /* differences between members of $(\Zed/30)^\ast$ */

object Factor(bigint* num)
{ num=copybigint(num,NULL); 
  if (num->size<0) { Printf("- "); num->size=-num->size; }
  { bigint* temp=mkbigint(num->size); digit p; int i=0;
    if (num->size==0) { Printf("0"); goto quit; }
    for (p=2; p<=trial_limit; p+= inc[i++])
    { if (i==array_size(inc)) i=3; /* after |37-31| wrap to difference |11-7| */
      
      if (copybigint(num,temp),div1(temp,p)==0)
      { index n; digit pn=p; int e=1;  copybigint(temp,num);
        for (n=1; pn<=MaxDigit/p; ++n) pn*=p; /* highest $p^n$ fitting in |digit| */
        for (; div1(temp,pn)==0; e+=n) copybigint(temp,num);
          /* find factors $p^n$ */
        if (n>1) /* then there might be some factors |p| left */
          for (copybigint(num,temp); div1(temp,p)==0; ++e) copybigint(temp,num);
            /* factors |p| */
        Printf("%ld",(long)p);  if (e>1) Printf("^%ld",(long)e);
        if (cmp1(num,1)==0) goto quit; /* last factor was found */
        Printf(" * ");
      }
    }
    printbigint(num,0); 
    if (num->size>2) Printf(" (Last factor need not be a prime)");
  quit:  Printf("\n");
    freemem(num); freemem(temp);
  }
  return (object) NULL;
}

