

typedef unsigned short objtype;
typedef unsigned short reftype;

enum
{ UNKNOWN = 0x00
, TEKST   = 0x11 /* character based type */
  , INTEGER = 0x02, VECTOR  = 0x12, MATRIX  = 0x32 /* integer based types */
  , BIGINT  = 0x03
  , POLY    = 0x14
, SIMPGRP = 0x06, GROUP   = 0x16 /* group based types */
  , ERROR   = 0x08
  , ARGTYPE = 0x09
  , VOID    = 0x0A
};

typedef struct
{
  
  objtype type; reftype nref;

  entry intval;
} intcel;

typedef struct
{
  
  objtype type; reftype nref;

  short allocsize, size;
  digit *data;
} bigint;

typedef struct
{
  
  objtype type; reftype nref;

  index ncomp;
  index size;
  entry *compon;
} vector;

typedef struct
{
  
  objtype type; reftype nref;

  index nrows, ncols;
  index rowsize;
  entry **elm;
  bigint **null;
} matrix;

typedef struct
{
  
  objtype type; reftype nref;

  index nrows, ncols;
  index rowsize;
  entry **elm;
  bigint **coef;
} poly;

typedef struct simpgrp_struct simpgrp;
struct simpgrp_struct
{
  
  objtype type; reftype nref;

  char lietype;
  entry lierank;
  matrix* cartan,* icartan,* roots;
  vector* exponents,* level,* root_norm;
  simpgrp* nextgrp;
};

typedef struct
{
  
  objtype type; reftype nref;

  index ncomp, toraldim;
  simpgrp **liecomp;
} group;

typedef struct
{
  
  objtype type; reftype nref;

  index len;
  char *string;
} tekst;

typedef union {
  struct { 
           objtype type; reftype nref;
  } any;
  intcel i;
  bigint b;
  vector v;
  matrix m;
  poly	 pl;
  simpgrp s;
  group g;
  tekst	 t;
} objcel, *object;


