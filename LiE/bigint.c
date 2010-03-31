/* big integer arithmetic is done in base |2^15|, so that two digits can be
   safely added to give another |digit| */

#define SHIFT        15
#define MASK      (digit)MaxDigit

/*
 *	bigint	*entry2bigint(entry n)
 *	entry	 bigint2entry(bigint *num)
 *	int	 cmp1(bigint* a; digit n)
 *	int	 cmp(bigint* a,bigint* b)
 *	bigint	*mul1add(bigint* b; digit n, m)
 *	digit	 div1(bigint* b; digit n)
 *	bigint	*mult(bigint* a,bigint* b)
 *	bigint	*divq(bigint* a,bigint* b)
 *	bigint	*norm(bigint* z)
 *	bigint	*add(bigint* a,bigint* b)
 *	bigint	*sub(bigint* a,bigint* b)
 *	bigint	*power(object a,bigint* b, object unit, f2object f)
 */
#include "lie.h"

#ifdef __STDC__
static bigint* x_add(bigint* a,bigint* b);
static bigint* x_sub(bigint* a,bigint* b);
static void errorc(void);
#endif

/* free argument pair a,b, but retain either if equal to z */
static void freepair(bigint* a,bigint* b,bigint* z)
{ 
#ifndef argumentsave
  if (a==b) { if (a!=z) freemem(a); } /* avoid freeing twice if a==b */
  else { if (a!=z) freemem(a);	if (b!=z) freemem(b); }
#endif
}

bigint *normalize(bigint *a)
{
  int size = abs(a->size); digit *d = a->data + size - 1;

  while (size > 0 && *d-- == 0) size--;
  a->size = (a->size < 0) ? -size : size;
  return a;
}

bigint *entry2bigint(entry n)
{
  twodigits m; bigint *result;

  if (n == 0) return mkbigint(0);
  result = mkbigint(3);
  if (n < 0) { result->size = -3; m = -n; } else m = n;
  result->data[0] = m & MASK; m >>= SHIFT;
  result->data[1] = m & MASK;
  result->data[2] = m >> SHIFT;
  return normalize(result);
}

entry bigint2entry(bigint *num)
{
  int size = abs(num->size); twodigits result = 0;
  digit *d = num->data + size - 1;

  if (size > 3 || size == 3 && *d > 1)
    error("Integer too big\n");
  for (; size > 0; size--)
    result = (result << SHIFT) + *d--;
  return (num->size < 0) ? -(entry)result : (entry)result;
}

int cmp1(bigint* a,digit n)
{
  int m = a->size;

  if (m < 0) return -1;
  if (m > 1) return 1;
  if (m > 0) m = *a->data;
  return (m < n) ? -1 : m != n;
}

int cmp(bigint* a, bigint* b)
{
  int a_size = a->size, size; digit *d, *e;

  if (a_size < b->size) return -1;
  if (a_size > b->size) return 1;
  size = abs(a_size); d = &a->data[size]; e = &b->data[size];
  for (; size > 0; --size)
    if (*--d != *--e) break;
  if (size == 0) return 0;
  return (a_size > 0) == (*d < *e) ? -1 : 1;
}

bigint *mul1add(bigint* b,digit n,digit m)
{
/* Side effect b */
  int i, size = abs(b->size);
  digit *d; twodigits n2 = n, t = m;

  d = b->data;
  for (i = size; i > 0; i--) {
    t += n2 * *d; *d++ = t & MASK; t >>= SHIFT;
  }
  if (t) {
    if (b->allocsize == size) {
      b = extendbigint(b); d = b->data + size;
    }
    *d = t;
    b->size += (b->size < 0) ? -1 : 1;
  }
  return b;
}

digit div1(bigint* b,digit n)/* Side effect in b */
{ digit* d; twodigits t = 0;
  int size = abs(b->size);

  if (size == 0) return (digit)0;
  d = b->data + size - 1;
  for (; size > 0; size--) {
    t = (t << SHIFT) + *d;
    *d-- = t / n; t %= n;
  }
  (void)normalize(b);
  return (digit)t;
}

bigint *mult(bigint *a, bigint* b) /* No side effect in a */
{
  int i, size_a = abs(a->size), size_b = abs(b->size);
  bigint *prod;

  if (size_a == 0 || size_b == 0) { freepair(a,b,NULL); return null; }
  prod = mkbigint(size_a + size_b);

  for (i = 0; i < prod->size; i++) prod->data[i] = 0;
  for (i = 0; i < size_a; i++) {
    twodigits carry = 0, f = a->data[i];
    int j; digit *d = prod->data + i;

    for (j = 0; j < size_b; j++) {
      carry += *d + b->data[j] * f;
      *d++ = carry & MASK; carry >>= SHIFT;
    }
    while (carry) {
      carry += *d;
      *d++ = carry & MASK; carry >>= SHIFT;
    }
  }
  if ((a->size < 0) != (b->size < 0)) prod->size = -prod->size;
  freepair(a,b,NULL); return normalize(prod);
}

bigint *divq(bigint *a,bigint *b) /* Side effect in a */

/*
    result = a/b and a = a % b
*/

{
  int size_a = abs(a->size), size_b = abs(b->size);
  bigint  *a_org = a, *quo; digit bl, d;
  int j, k;

  if (size_b == 0) error("Division by zero\n");
  if (size_a < size_b ||
	size_a == size_b && a->data[size_a-1] < b->data[size_b-1])
    return null;
  if (size_b == 1) {
    entry rem = (entry)div1(a, *b->data);
    if (a->size < 0) rem = -rem;
    quo = copybigint(a, (bigint *)NULL);
    a = entry2bigint(rem); (void)copybigint(a, a_org); freemem( a);
    if (b->size < 0)
      quo->size = -quo->size;
    return quo;
  }
  d = (twodigits)(MASK + 1) / (b->data[size_b-1] + 1);
  a = copybigint(a, mkbigint(size_a+1));
  (void)mul1(a, d); (void)mul1(b, d); /* size effect on b will be undone */
  size_a = abs(a->size);
  bl = b->data[size_b-1];
  quo = mkbigint(size_a - size_b + 1);
  for (j = size_a, k = size_a - size_b; k >= 0; j--, k--) {
    digit aj = (j >= size_a) ? 0 : a->data[j];
    twodigits aj2 = ((twodigits)aj << SHIFT) + a->data[j-1], q;
    entry carry = 0L; int i;
 
    q = (aj == bl) ? MASK : aj2/bl;
    while (b->data[size_b-2] * q > ((aj2 - q * bl) << SHIFT) + a->data[j-2])
      q--;
    for (i = 0; i < size_b && i+k < size_a; i++) {
      twodigits z = b->data[i] * q;
      carry += a->data[i+k] - (z & MASK);
      a->data[i+k] = carry & MASK;
      carry = (carry >> SHIFT) - (z >> SHIFT);
    }
    if (i+k < size_a) {
      carry += a->data[i+k]; a->data[i+k] = 0;
    }
    if (carry == 0)
      quo->data[k] = q;
    else {
      quo->data[k] = q-1;
      for (carry = i = 0; i < size_b && i+k < size_a; i++) {
	carry += a->data[i+k] + b->data[i];
	a->data[i+k] = carry & MASK;
	carry >>= SHIFT;
      }
    }
  }
  if ((a->size < 0) != (b->size < 0))
    quo->size = -quo->size;
  (void)normalize(a);
  (void)div1(a, d); copybigint(a,a_org); (void)div1(b, d); /* restore */
  return normalize(quo);
}

bigint *quotient(a,b) bigint *a, *b;
{ bigint *result = divq( isshared(a)?(a=copybigint(a,NULL)):a , b);
  freepair(a,b,result); return result;
}

bigint *mod(a, b)
bigint *a, *b;
{
  int size_b = b->size;
  bigint *result = (isshared(a)?copybigint(a,NULL):a);
  bigint *garbage;
  if (size_b <= 0) error("Remainder should be modulo positive number.\n");
  garbage = divq(result,b);
#ifndef argumentsave
  freemem(garbage);
#endif
  if (result->size < 0)
  { /* Result mod always has to be positive */
      setshared(b);
      result = add(result,b);
      clrshared(b);
  }
#ifndef argumentsave
    freemem(b); /* a is either shared or ==result */
#endif
  return(result);
}

/* convert string of digits to bigint; used in scanner */
bigint *str2bigint(string s)
{
  bigint* result = mkbigint(2 + strlen(s)/4); int i;
  for (i=0, result->size=0; s[i]!='\0'; ++i)
    result = mul1add(result, 10, s[i]-'0');
  return result;
}

string bigint2str(bigint* num) /* free result with |freem| */
{
  int i,n;
  unsigned long strsize = 4 + 5 * (unsigned long)abs(num->size);
  bigint *copynum = copybigint(num,(bigint *)NULL);
  string result = allocmem(strsize), s=result+strsize, t=result;

  *--s = '\0';
  while (copynum->size)
  { n=div1(copynum,10000); for (i=4; i>0; i--,n/=10) *--s = n%10+'0'; }
  while (*s=='0') s++; /* skip leading 0's */
  if (*s=='\0') *--s = '0'; /* but restore 0 if last remaining digit */
  if (num->size<0) *--s = '-';
  while ((*t++ = *s++)!=0) {} /* adjust to the left */
  freemem(copynum); return result;
}

int printbigint(num,len)
bigint *num;int len;
{
  Short i; int nc = 0;
  string s = bigint2str(num);
  len = len-strlen(s);
  for (i=0;i<len;i++) nc += Printf("%c",' ');
  nc += Printf("%s",s);
  freem(s);
  return nc;
}

bigint *
norm(z)
	bigint *z;
{
	int j= abs(z->size);
	int i= j;
	
	while (i > 0 && z->data[i-1] == 0)
		--i;
	if (i != j)
		z->size= (z->size < 0) ? -i : i;
	return z;
}

static	bigint* x_add(a,b) bigint* a,* b;
{ /* No side effect, result address equal to first argument */
  int size_a= abs(a->size), size_b= abs(b->size);
  bigint *z;
  int i;
  digit carry= 0;
  
  /* Ensure a is the larger of the two: */
  if (size_a < size_b) {
    { bigint *temp= a; a= b; b= temp; }
    { int size_temp= size_a; size_a= size_b; size_b= size_temp; }
  }
  if (isshared(a)  || (a->allocsize <= size_a)) 
    z= mkbigint(size_a+1);
  
/*************************************************************
* Side effect on argument a				     *
*************************************************************/
  
  else {
  
#ifdef development
if (a==one) abort();
if (a==null) abort();
#endif
  
    z = a;
    z->size = size_a + 1;
  }
  for (i= 0; i < size_b; ++i) {
    carry += a->data[i] + b->data[i];
    z->data[i]= carry & MASK;
    /* The following assumes unsigned shifts don't
       propagate the sign bit. */
    carry >>= SHIFT;
  }
  for (; i < size_a; ++i) {
    carry += a->data[i];
    z->data[i]= carry & MASK;
    carry >>= SHIFT;
  }
  z->data[i]= carry;
  return norm(z);
}

/* Subtract the absolute values of two integers. */

static bigint* x_sub(a, b) bigint *a, *b;
{
  int size_a= abs(a->size), size_b= abs(b->size);
  bigint *z;
  int i;
  int sign= 1;
  digit borrow= 0;
  
  /* Ensure a is the larger of the two: */
  if (size_a < size_b) {
    sign= -1;
    { bigint *temp= a; a= b; b= temp; }
    { int size_temp= size_a; size_a= size_b; size_b= size_temp; }
  }
  else if (size_a == size_b) {
/* Find highest digit where a and b differ: */
    i= size_a-1;
    while (i >= 0 && a->data[i] == b->data[i]) --i;
    if (i < 0) return mkbigint(0);
    if (a->data[i] < b->data[i]) {
      sign= -1; { bigint *temp= a; a= b; b= temp; }
    }
    size_a= size_b= i+1;
  }
  z= mkbigint(size_a);
  for (i= 0; i < size_b; ++i) {
/* The following assumes unsigned arithmetic
   works modulo 2^N for some N>SHIFT. */
    borrow= a->data[i] - b->data[i] - borrow; 
    z->data[i]= borrow & MASK;
    borrow >>= SHIFT;
    borrow &= 1; /* Keep only one sign bit */
  }
  for (; i < size_a; ++i) {
    borrow= a->data[i] - borrow;
    z->data[i]= borrow & MASK;
    borrow >>= SHIFT;
  }
  z->size *= sign;
  return norm(z);
}

/* Add two long integers, taking their signs into account. */

bigint* add(a, b) bigint* a,* b;
{ /* Result address equal to adress first argument */
  bigint *z;
  
  if (a->size < 0) {
    if (b->size < 0) { z= x_add(a, b); z->size= -z->size; }
    else z= x_sub(b, a);
  }
  else if (b->size < 0) z= x_sub(a, b); else  z= x_add(a, b);
  freepair(a,b,z); return z;
}

/* Subtract two long integers, taking their signs into account. */

bigint* sub(a, b) bigint* a,* b;
{
  bigint *z;
  
  if (a->size < 0) {
    if (b->size < 0) z= x_sub(a, b); else z= x_add(a, b);
    z->size= -z->size;
  }
  else if (b->size < 0) z= x_add(a, b); else z= x_sub(a, b);
  freepair(a,b,z); return z;
}

/* Compute $f(\cdot,a)^{|b|}(unit)$ by a form of Chinese powering,
   assuming $f$ is an associative operation */

object power(object a, bigint* b, object unit, f2object f)
{ int size_b= abs(b->size);
  object z = unit;
  int i;
  for (i= 0; i < size_b; ++i) /* for digits from least to most significant */
  { digit bi= b->data[i]; int j;
    for (j= 0; j < SHIFT; ++j) /* and for all bits of digit in same order */
    { setshared(a);
      if ((bi&1)!=0) z=f(z,a); /* include $a^{2^{SHIFT*i+j}}$ into |z| */
      bi >>= 1;
      if (bi == 0 && i+1 == size_b) break; /* avoid excess powering of |a| */
      { object temp= f(a,a); clrshared(a); freemem(a); a= temp; }/* $a=a^2$ */
    }
  }
  return z;
}

/***************************************************************
*  Operations with type digit constants on bigints	       *
***************************************************************/

/* subc is not defined */

/* addc, mulc, divc, modc */

static void errorc() { error("System: Pointer to bigint is NULL.\n"); }

#ifdef __STDC__
void addc(bigint** b,digit n)
#else
void addc(b,n) bigint **b; digit n;
#endif
{
    if (!(*b)) errorc();
    if (isshared(*b)) *b = copybigint(*b,NULL);
    *b = mul1add(*b,1,n);
}

#ifdef __STDC__
void mulc(bigint** b,digit m)
#else
void mulc(b,m) bigint **b; digit m;
#endif
{
    if (!(*b)) errorc();
    if (isshared(*b)) *b = copybigint(*b,(bigint*) NULL);
    *b = mul1add(*b,m,0);
}

#ifdef __STDC__
void divc(bigint** b,digit n)
#else
void divc(b,n) bigint **b; digit n;
#endif
{
    if (!(*b)) errorc();
    if (isshared(*b)) *b = copybigint(*b,(bigint*) NULL);
    div1(*b,n);
}
