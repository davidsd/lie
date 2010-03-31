
#include <unistd.h>  /* needed for |isatty| */
#include <stdio.h> 

int no_terminal(FILE* f) { return !isatty(fileno(f)); }

void sysinit(void)  {}

