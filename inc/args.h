#ifndef ARGS_H
#define ARGS_H

// CUT
#include <array.h>
#include <map.h>
#include <oop.h>
#include <diagnostic.h>
#include <str.h>
#include <file.h>
#include <terminal.h>
#include <exception.h>

/* GNU/POSIX compliant argument parsing utility for CUT
 * https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html
 */

#define TYPENAME Args

#define END_OPTION { NULL, '\0', NULL, ARG_TYPE_NONE, NULL }

// '+' Required parameter
// '-' Optional paramters (must be last)
// '*' Array parameter (must be last)
#define OPTIONS(...) \
ArgOption _OPTIONS[] = { __VA_ARGS__ __VA_OPT__(,) END_OPTION }

#ifndef VERSION_MAJOR
#define VERSION_MAJOR 0
#endif
#ifndef VERSION_MINOR
#define VERSION_MINOR 0
#endif

__attribute__((unused)) static int _VERSION_MAJOR = VERSION_MAJOR;
__attribute__((unused)) static int _VERSION_MINOR = VERSION_MINOR;

OBJECT (int argc, char *argv[], void *envptr) BASED (void*)
  int        param_mode;
  int        program_major;
  int        program_minor;
  String    *program_name;
  Map       *parameters;
END_OBJECT(0, NULL, NULL);

typedef enum arg_type {
  ARG_TYPE_CHARPTR,
  ARG_TYPE_INTEGER,
  ARG_TYPE_DECIMAL,
  ARG_TYPE_BOOLEAN,
  ARG_TYPE_ANY,
  ARG_TYPE_NONE
} ArgType;

typedef union arg_value {
  const char *as_charptr;
  long long   as_integer;
  double      as_decimal;
} ArgValue;

typedef struct arg_option {
  const char  *name;
  char         ident;
  const char  *desc;
  ArgType      type;
  const void (*callback)(Args*, ArgValue);
} ArgOption;

extern ArgOption _OPTIONS[];

ArgValue  _(Index)(int index);
ArgValue  _(Name)(const char *name);
Array    *_(List)();

#undef TYPENAME
#endif