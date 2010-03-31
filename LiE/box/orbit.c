#include "lie.h"

matrix* Orbit(limit,vec,m,n) index limit,n; vector* vec; entry** m;
{ index i, j, l=vec->ncomp, last, cur;
  matrix* result = mkmatrix(limit+1,l); entry** res= result->elm;
  copyrow(vec->compon,res[0],l);
  for (cur=0,last=1; cur<last; cur++)
    for (i=0; i<n; i++)
    { mulvecmatelm(res[cur],m+i*l,res[last],l,l);
      j=0; while (!eqrow(res[j++],res[last],l)) {}
      if (j>last) if (++last>limit) error("Orbit size exceeds given limit\n");
    }
  result->nrows=last; return result;
}
