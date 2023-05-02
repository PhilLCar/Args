#ifndef ARGS_H
#define ARGS_H

#include <array.h>
#include <map.h>
#include <oop.h>
#include <diagnostic.h>
#include <str.h>
#include <file.h>
#include <terminal.h>

#define TYPENAME Args

#define VERSION(MAJOR, MINOR) \
int _VERSION_MAJOR = MAJOR;\
int _VERSION_MINOR = MINOR
#define OPTIONS(...) \
ArgOption _OPTIONS[] = { __VA_ARGS__ __VA_OPT__(,) { NULL, ' ', "", NULL } }

typedef enum arg_type {
  ARG_TYPE_CHARPTR,
  ARG_TYPE_INTEGER,
  ARG_TYPE_DECIMAL,
  ARG_TYPE_BOOLEAN
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
  const void (*callback)(Args*);
} ArgOption;

OBJECT (int, char*[], void*)
  void      *env;
  char      *program_name;
  Map       *parameters;
  Array     *positionals;
END_OBJECT;

extern ArgOption _OPTIONS[];
extern int       _VERSION_MAJOR;
extern int       _VERSION_MINOR;

long long   _(cint)(char c)        ALIAS (cint);
long long   _(sint)(const char *s) ALIAS (sint);
double      _(cdec)(char c)        ALIAS (cdec);
double      _(sdec)(const char *s) ALIAS (sdec);
const char *_(cstr)(char c)        ALIAS (cstr);
const char *_(sstr)(const char *s) ALIAS (sstr);


#undef TYPENAME
#endif