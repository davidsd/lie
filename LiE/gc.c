#include "lie.h"
/*# include "lie-py.h"*/
extern long chunks; 

void simple_gc(void) {
  int i;
  for (i = 0; i<maxptrs; i++)
    if (ptr[i]!=NULL && !isshared(ptr[i]))
      freem(ptr[i]);
}

void c_print_objs(void) {
  int i;
  int j,k;
  object obj;
  string s;
  for (i = 0; i<maxptrs; i++) {
    if (ptr[i] != NULL && isshared(ptr[i])) {
      obj = ptr[i];
      switch type_of(obj) {
        case INTEGER:
          printf("integer(%d):\t%d\n", obj->any.nref, obj->i.intval);
          break;
        case BIGINT:
          s = bigint2str((bigint*) obj);
          printf("bigint (%d):\t%s\n", obj->any.nref, s);
          freem(s);
          break;
        case VECTOR:
          printf("vector (%d):\t[", obj->any.nref);
          for (j = 0; j < obj->v.ncomp; j++) {
            if (j == 0) {
              printf("%d", obj->v.compon[j]);
            } else {
              printf(",%d", obj->v.compon[j]);
            }
          }
          printf("]\n");
          break;
        case MATRIX:
          printf("matrix (%d):\t[", obj->any.nref);
          for (j = 0; (j < 3) && (j < obj->m.nrows); j++) {
            if (j != 0)
              printf(",");
            printf("[");
            for (k = 0; k < obj->m.ncols; k++) {
              if (k != 0)
                printf(",");
              printf("%d", obj->m.elm[j][k]);
            }
            printf("]");
          }
          if (obj->m.nrows > 3)
            printf("...");
          printf("]\n");
          break;
        case POLY:
          printf("poly   (%d):\t", obj->any.nref);
          for (j = 0; (j < 3) && (j < obj->pl.nrows); j++) {
            s = bigint2str(obj->pl.coef[j]);
            if (j!=0)
              printf(" + ");
            printf("%sX[", s);
            freem(s);
            for (k = 0; k < obj->pl.ncols; k++) {
              if (k!=0)
                printf(",");
              printf("%d", obj->pl.elm[j][k]);
            }
            printf("]");
          }
          if (obj->m.nrows > 3)
            printf(" + ...");
          printf("\n");
          break;
        case GROUP:
          s = grp2str((group*) obj);
          printf("group  (%d):\t%s\n", obj->any.nref, s);
          freem(s);
          break;
        case TEKST:
          printf("tekst  (%d):\t%s\n", obj->any.nref, obj->t.string);
          break;
        case ERROR:
          printf("error\n");
          break;
        case ARGTYPE:
          printf("argtype\n");
          break;
        case VOID:
          printf("void\n");
          break;
        case SIMPGRP:
          printf("simpgrp(%d):\t%c%d\n", obj->any.nref, obj->s.lietype, obj->s.lierank);
          break;
        default:
          printf("unknown type: %d\n", type_of(obj));
          break;
        }
    }
  }
  /*printf("=========   %d   ====================================\n", chunks);*/
}
    
