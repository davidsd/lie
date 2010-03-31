#include "lie-py.h"
#include <signal.h>

/* this function replaces LiE's error function in output.c */
void error(char *format, ...) {
  va_list ap;
  char str[128];

  va_start(ap, format);
  vsprintf(str, format, ap);
  va_end(ap);
  
  printf(str);
  PyErr_SetString(PyExc_RuntimeError, str);
  /*PyErr_Format(PyExc_RuntimeError, format, ap);*/
  raise (SIGFPE);
}
