
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <limits.h>
#include <assert.h>

/* non-ANSI */
#include <unistd.h>  

typedef long entry; /* see also |MaxEntry| and |MinEntry| */
typedef long index;
typedef short Short;
typedef unsigned short digit; /* see also |MaxDigit| */
typedef unsigned long twodigits;

#define  no_terminal(f)	(!isatty(fileno(f)))
#define  sysinit()

#define  DEFAULT_EDITOR	"emacs"
#define  DEFAULT_PAGER	"less"
#define  MaxDigit		((1<<15)-1) /* bigints are base $2^{15}$ */
#define  MaxEntry		LONG_MAX
#define  MinEntry		LONG_MIN
#define  max_obj_size	UINT_MAX /* limit for |sizeof(x)| */

#define  MAXPTRS_DFLT		999999
#define  GCCRIT		1000
  /* number of spare locations when calling the garbage collector */
#define  MAXNODES_DFLT	6000

#define  readmode "r"
#define  writemode "w"

