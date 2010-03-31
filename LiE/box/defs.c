/* this file allows the the user/programmer to add some builtin functions to
   LiE and to "dynamically" include them, without having to touch or recompile
   the library interface in the static[1-7].c files. Each file that contains
   new builtin functions should define a parameterless void initialisation
   function that installs the necessary functions by calling on of the macros
   A0-A5 defined in lie.h (their syntax is identical to C0-C5 used in the
   static[1-7].c files, but they expand to statements). To arrange that the
   initialisation function be declared and called, add its name followed by
   "(VID)," (without the quotes) to defs.h; the function |add_user_defined|
   below then ensures that all initialisation functions are called in order */

#include "lie.h"
#ifdef __STDC__
#define VID void
#else
#define VID 
#endif
void
#include "defs.h"
add_user_defined (VID); /* just to have something after the final comma */
#undef VID
#define VID
void add_user_defined() {
#include "defs.h"
(void)0; /* idem */
#undef VID
}
