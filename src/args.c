#include <args.h>

#define TYPENAME Args

void _default_help(Args*, ArgValue);
void _default_version(Args*, ArgValue);

ArgOption _DEFAULT_OPTIONS[] = {
  { "help"   , 'h', "Displays this message"       , ARG_TYPE_NONE, _default_help    },
  { "version", 'v', "Displays the program version", ARG_TYPE_NONE, _default_version }
};

/******************************************************************************/
int _sizeof_options()
{
  int size = 0;

  do {
    ArgOption *option = &_OPTIONS[size];

    if (!option->name && !option->ident) break;
  } while (++size);
  
  return size;
}

/******************************************************************************/
int _param_start()
{
  int start = 0;

  do {
    ArgOption *option = &_OPTIONS[start];
    char       o      = option->ident;

    if (o == '+' || o == '-' || o == '*' || !o) break;
  } while (++start);
  
  return start;
}

/******************************************************************************/
void _print_option(ArgOption *option, int padding)
{
  char buffer[256];
  char name[256];
  char ident[256];
  char *expects;

  switch (option->type) {
    case ARG_TYPE_CHARPTR: expects = "string "; break;
    case ARG_TYPE_INTEGER: expects = "integer"; break;
    case ARG_TYPE_DECIMAL: expects = "decimal"; break;
    case ARG_TYPE_BOOLEAN: expects = "boolean"; break;
    case ARG_TYPE_ANY:     expects = "any    "; break;
    case ARG_TYPE_NONE:    expects = "       "; break;
    default: return;
  }

  sprintf(buffer, "--%s", option->name);

  if (option->name[0]) ljust(buffer,     name, padding);
  else                 ljust(buffer + 2, name, padding);

  if (option->ident != ' ') sprintf(ident, "-%c   ", option->ident);
  else                      sprintf(ident, "%s", "     ");

  printf("\t%s %s %s %s\n", name, ident, expects, option->desc);
}

/******************************************************************************/
void _default_help(Args *args, ArgValue value)
{
  int opt_size = _sizeof_options();
  int def_size = sizeof(_DEFAULT_OPTIONS) / sizeof(ArgOption);
  int p_start  = _param_start();

  int max_name = 11;

  for (int i = 0; i < opt_size; i++) {
    int len = strlen(_OPTIONS[i].name) + 2;

    if (len > max_name) max_name = len;
  }

  for (int i = 0; i < def_size; i++) {
    int len = strlen(_DEFAULT_OPTIONS[i].name) + 2;

    if (len > max_name) max_name = len;
  }

  printf("USAGE:\n");
  printf("\t%s [OPTIONS]", args->program_name->base);

  for (int i = p_start; i < opt_size; i++)
  {
    ArgOption  *option = &_OPTIONS[i];
    const char *name   = option->name;
    char        buffer[256];

    if (!option->name[0])
    {
      name = buffer;
      sprintf(buffer, "param%d", i - p_start);
    }

    switch (option->ident)
    {
      case '+': printf(" %s",    name); break;
      case '-': printf(" [%s]",  name); break;
      case '*': printf(" %s...", name); break;
      default:
        fprintf(stderr, "Badly formed parameter: '%s'\nOnly '+', '-', '*' are allowed.\n", name);
        exit(1);
    }
  }

  printf("\nOPTIONS:\n");

  {
    char buffer[256];

    ljust("Long Option", buffer, max_name);
    printf("\t%s %s %s %s\n", buffer, "Short", "Expects", "Description");
    ljust("===========", buffer, max_name);
    printf("\t%s %s %s %s\n", buffer, "=====", "=======", "===========");
  }

  for (int i = 0; i < p_start; i++) _print_option(&_OPTIONS[i], max_name);

  for (int i = 0; i < def_size; i++)
  {
    ArgOption defopt = _DEFAULT_OPTIONS[i];

    for (int j = 0; j < p_start; j++)
    {
      ArgOption *option = &_OPTIONS[j];

      if (option->ident == defopt.ident)      defopt.ident = ' ';
      if (!strcmp(option->name, defopt.name)) defopt.name  = "";
    }

    if (defopt.ident != ' ' || defopt.name[0] != '\0') _print_option(&defopt, max_name);
  }

  printf("PARAMETERS:\n");

  exit(0);
}

/******************************************************************************/
void _default_version(Args *args, ArgValue value)
{
  printf("Program: %s\nVersion: %d.%d\n", args->program_name->base, args->program_major, args->program_minor);
  exit(0);
}

/******************************************************************************/
ArgValue _parse_option(ArgOption *option, const char *parameter)
{
  ArgValue value;

  switch (option->type) {
    case ARG_TYPE_ANY:
    // Casting is performed by used, so treat as charptr
    case ARG_TYPE_CHARPTR:
      value.as_charptr = parameter;
      break;
    case ARG_TYPE_INTEGER:
      value.as_integer = atoi(parameter);
      break;
    case ARG_TYPE_DECIMAL:
      value.as_decimal = atof(parameter);
      break;
    case ARG_TYPE_BOOLEAN:
      value.as_integer = !strcmp("true", parameter);
      break;
    case ARG_TYPE_NONE:
    default:
      value.as_charptr = NULL;
      break;
  }

  return value;
}

/******************************************************************************/
void _(parameter)(int* index, const char *value)
{
  ArgOption *parameter = &_OPTIONS[*index];

  if (parameter->ident != ' ' && parameter->ident != '*') (*index)++;

  Map_setkey(_this->parameters, NEW (String) (parameter->name), NEW (String) (value));
}

/******************************************************************************/
ArgOption* _(option)(char ident)
{
  ArgOption *found = NULL;

  int n_default = sizeof(_DEFAULT_OPTIONS) / sizeof(ArgOption);

  for (int i = 0; _OPTIONS[i].ident; i++) {
    if (_OPTIONS[i].ident == ident) {
      found = &_OPTIONS[i];
      break;
    }
  }

  if (!found) {
    for (int i = 0; i < n_default; i++) {
      if (_DEFAULT_OPTIONS[i].ident == ident) {
        found = &_DEFAULT_OPTIONS[i];
        break;
      }
    }
  }

  return found;
}

/******************************************************************************/
void _(optcall)(ArgOption *option, const char *param)
{
  option->callback(_this, _parse_option(option, param));
}

/******************************************************************************/
ArgOption* _(loption)(const char *name)
{
  ArgOption *found = NULL;

  int n_default = sizeof(_DEFAULT_OPTIONS) / sizeof(ArgOption);

  for (int i = 0; _OPTIONS[i].ident; i++) {
    if (!strcmp(_OPTIONS[i].name, name)) {
      found = &_OPTIONS[i];
      break;
    }
  }

  if (!found) {
    for (int i = 0; i < n_default; i++) {
      if (!strcmp(_DEFAULT_OPTIONS[i].name, name)) {
        found = &_DEFAULT_OPTIONS[i];
        break;
      }
    }
  }

  return found;
}

/******************************************************************************/
void _(loptcall)(const char *option)
{
  const char *param = strstr(option, "=");
  const char *name  = option;

  char  buffer[256];

  // Separate the name and the value around the "=" sign
  if (param) {
    name = buffer;
    memcpy(buffer, option, ++param - option);
  }

  {
    ArgOption *opt = Args_loption(_this, name);

    if (opt) Args_optcall(_this, opt, param);
  }
}

////////////////////////////////////////////////////////////////////////////////
TYPENAME *_(cons)(int argc, char *argv[], void *env)
{
  if (_this) {
    int  param_start = _param_start();
    int  param_mode  = 0;
    char buffer[256];

    filenamewopath(buffer, argv[0]);
    filenamewoext (buffer, buffer);

    _this->env           = env;
    _this->program_major = _VERSION_MAJOR;
    _this->program_minor = _VERSION_MINOR;
    _this->program_name  = NEW (String) (buffer);
    _this->parameters    = NEW (Map)    (sizeof(String), sizeof(String), (Comparer)String_equals);

    for (int i = 1; i < argc; i++) {
      char *arg = argv[i];

      // Ignore empty argument
      if (!arg[0]) continue;

      // TODO: Remove param mode
      if (param_mode || arg[0] != '-') {        
        Args_parameter(_this, &param_start, arg);
      }
      else if (arg[1] == '-') {
        if (!arg[2]) {
          // Option end marker, everything after is a parameter
          param_mode = 1;
        } else {
          // Long option
          Args_loptcall(_this, &arg[2]);
        }
      } else {
        // Normal option
        for (int j = 1; arg[j]; j++) {
          ArgOption* option = Args_option(_this, arg[j]);

          if (option) {
            if (option->type == ARG_TYPE_NONE) {
              Args_optcall(_this, option, NULL);
            } else if (!arg[j + 1]) {
              // Split param option
              Args_optcall(_this, option, argv[++i]);
              break;
            } else {
              // Contiguous param option
              Args_optcall(_this, option, &arg[++j]);
              break;
            }
          } else {
            fprintf(stderr, "Unreckognized option '%c'.\n", arg[j]);
            
            exit(0);
          }
        }
      }
    }
  }

  return _this;
}

////////////////////////////////////////////////////////////////////////////////
void _(free)()
{
  DELETE (_this->program_name);
  DELETE (_this->parameters);
}

////////////////////////////////////////////////////////////////////////////////
ArgValue _(index)(int index)
{
  Pair      *pair   = Array_at((Array*)_this->parameters, index);
  ArgOption *option = Args_loption(_this, *(char**)pair->first);

  return _parse_option(option, *(char**)pair->second);
}

////////////////////////////////////////////////////////////////////////////////
ArgValue _(name)(const char* name)
{
  Pair      *pair   = Map_atkey(_this->parameters, (void*)name);
  ArgOption *option = Args_loption(_this, *(char**)pair->first);

  return _parse_option(option, *(char**)pair->second);
}

////////////////////////////////////////////////////////////////////////////////
Array* _(list)()
{
  ArgOption *option = Args_option(_this, '*');
  Array     *params = (Array*)_this->parameters;
  Array     *list   = NEW (Array) (sizeof(ArgValue));


  for (int i = 0; i < params->size; i++) {
    Pair *pair = (Pair*)Array_at(params, i);

    if (!strcmp(option->name, *(char**)pair->first)) {
      ArgValue value = _parse_option(option, *(char**)pair->second);

      Array_push(list, &value);
    }
  }

  return list;
}

