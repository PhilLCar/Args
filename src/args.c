#include <args.h>

#define TYPENAME Args

void _(help)(ArgValue);
void _(version)(ArgValue);

ArgOption _DEFAULT_OPTIONS[] = {
  { "help"   , 'h', "Displays this message"       , ARG_TYPE_NONE, Args_help    },
  { "version", 'v', "Displays the program version", ARG_TYPE_NONE, Args_version }
};

/******************************************************************************/
int STATIC (sizeof_options)()
{
  int size = 0;

  do {
    ArgOption *option = &_OPTIONS[size];

    if (!option->name && !option->ident) break;
  } while (++size);
  
  return size;
}

/******************************************************************************/
int STATIC (param_start)()
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
ArgOption *STATIC (option)(const char ident)
{
  ArgOption *found = NULL;

  int n_options = Args_param_start();
  int n_default = sizeof(_DEFAULT_OPTIONS) / sizeof(ArgOption);

  for (int i = 0; i < n_options; i++) {
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
ArgOption *STATIC (loption)(const char *name)
{
  ArgOption *found = NULL;

  int n_options = Args_param_start();
  int n_default = sizeof(_DEFAULT_OPTIONS) / sizeof(ArgOption);

  for (int i = 0; i < n_options; i++) {
    if (_OPTIONS[i].name && !strcmp(_OPTIONS[i].name, name)) {
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
ArgOption *STATIC (parameter)(const char *name)
{
  ArgOption *found = NULL;

  for (int i = Args_param_start(); _OPTIONS[i].ident; i++) {
    if (!strcmp(_OPTIONS[i].name, name)) {
        found = &_OPTIONS[i];
        break;
      }
  }

  return found;
}

/******************************************************************************/
ArgValue STATIC (parse_option)(ArgOption *option, const char *parameter)
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
      value.as_integer = !parameter
        || (strcmp("false", parameter) 
         && strcmp("False", parameter)
         && strcmp("FALSE", parameter)
         && strcmp("0",     parameter));
      break;
    case ARG_TYPE_NONE:
    default:
      value.as_charptr = NULL;
      break;
  }

  return value;
}

/******************************************************************************/
char *STATIC (expects)(ArgOption *option)
{
  char *expects;

  switch (option->type) {
    case ARG_TYPE_CHARPTR: expects = "string "; break;
    case ARG_TYPE_INTEGER: expects = "integer"; break;
    case ARG_TYPE_DECIMAL: expects = "decimal"; break;
    case ARG_TYPE_BOOLEAN: expects = "boolean"; break;
    case ARG_TYPE_ANY:     expects = "any    "; break;
    default:
    case ARG_TYPE_NONE:    expects = "       "; break;
  }

  return expects;
}

/******************************************************************************/
char *STATIC (ptype)(ArgOption *option)
{
  char *ptype;

  switch (option->ident) {
    case '-': ptype = "optional  "; break;
    case '*': ptype = "multiple  "; break;
    default:
    case '+': ptype = "mandatory "; break;
  }

  return ptype;
}

/******************************************************************************/
void STATIC (print_option)(ArgOption *option, int padding)
{
  char  buffer[256];
  char  name[256];
  char  ident[256];
  char *expects = Args_expects(option);

  sprintf(buffer, "--%s", option->name);

  if (option->name[0]) ljust(buffer,     name, padding);
  else                 ljust(buffer + 2, name, padding);

  if (option->ident != ' ') sprintf(ident, "-%c   ", option->ident);
  else                      sprintf(ident, "%s", "     ");

  printf("\t%s %s %s %s\n", name, ident, expects, option->desc);
}

/******************************************************************************/
void STATIC (print_parameter)(ArgOption *option, int padding)
{
  char  name[256];
  char *type    = Args_ptype(option);
  char *expects = Args_expects(option);

  ljust(option->name, name, padding);

  printf("\t%s %s %s %s\n", name, type, expects, option->desc);
}

/******************************************************************************/
void _(help)(ArgValue value)
{
  int opt_size = Args_sizeof_options();
  int def_size = sizeof(_DEFAULT_OPTIONS) / sizeof(ArgOption);
  int p_start  = Args_param_start();

  int max_name = 11;

  ArgOption *params = malloc((opt_size - p_start) * sizeof(ArgOption));
  char       pnames[1024];

  for (int i = 0; i < opt_size; i++) {
    int len = strlen(_OPTIONS[i].name) + 2;

    if (len > max_name) max_name = len;
  }

  for (int i = 0; i < def_size; i++) {
    int len = strlen(_DEFAULT_OPTIONS[i].name) + 2;

    if (len > max_name) max_name = len;
  }

  printf("USAGE:\n");
  printf("\t%s [OPTIONS]", this->program_name->base);

  for (int i = p_start, j = 0; i < opt_size; i++, j++)
  {
    ArgOption *opt;

    params[j] = _OPTIONS[i];

    opt = &params[j];

    if (!opt->name[0])
    {
      opt->name = &pnames[j << 5];
      sprintf((char*)opt->name, "param%d", j);
    }

    switch (opt->ident)
    {
      case '+': printf(" %s",    opt->name); break;
      case '-': printf(" [%s]",  opt->name); break;
      case '*': printf(" %s...", opt->name); break;
      default:
        fprintf(stderr, "Badly formed parameter: '%s'\nOnly '+', '-', '*' are allowed.\n", opt->name);
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

  for (int i = 0; i < p_start; i++) Args_print_option(&_OPTIONS[i], max_name);

  for (int i = 0; i < def_size; i++)
  {
    ArgOption defopt = _DEFAULT_OPTIONS[i];

    for (int j = 0; j < p_start; j++)
    {
      ArgOption *option = &_OPTIONS[j];

      if (option->ident == defopt.ident)      defopt.ident = ' ';
      if (!strcmp(option->name, defopt.name)) defopt.name  = "";
    }

    if (defopt.ident != ' ' || defopt.name[0] != '\0') Args_print_option(&defopt, max_name);
  }

  printf("PARAMETERS:\n");

  {
    char buffer[256];

    ljust("Position ", buffer, max_name - 2);
    printf("\t%s %-10s %s %s\n", buffer, "Type", "Expects", "Description");
    ljust("======== ", buffer, max_name - 2);
    printf("\t%s %-10s %s %s\n", buffer, "====", "=======", "===========");
  }

  for (int i = 0; i < opt_size - p_start; i++)
  {
    Args_print_parameter(&params[i], max_name - 2);
  }

  free(params);
  exit(0);
}

/******************************************************************************/
void _(version)(ArgValue value)
{
  printf("Program: %s\nVersion: %d.%d\n", this->program_name->base, this->program_major, this->program_minor);
  exit(0);
}

/******************************************************************************/
void _(optcall)(ArgOption *option, const char *param)
{
  if (option->callback) option->callback(this, Args_parse_option(option, param));
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
    memcpy(buffer, option, param++ - option);
  }

  {
    ArgOption *opt = Args_loption(name);

    if (opt) {
      Args_optcall(this, opt, param);
    } else {
      fprintf(stderr, "Unreckognized option '%s'.\n", option);
      exit(0);
    }
  }
}

/******************************************************************************/
void _(pcall)(int* index, const char *value)
{
  ArgOption *parameter = &_OPTIONS[*index];

  if (parameter->ident != ' ' && parameter->ident != '*') (*index)++;

  Map_Set(this->parameters, NEW (String) (parameter->name), NEW (String) (value));
  Args_optcall(this, parameter, value);
}

////////////////////////////////////////////////////////////////////////////////
TYPENAME *_(Construct)(int argc, char *argv[], void *env)
{
  if (this) {
    int  param_start = Args_param_start();
    int  param_mode  = 0;
    char buffer[256];

    filenamewopath(buffer, argv[0]);
    filenamewoext (buffer, buffer);

    this->base           = env;
    this->program_major = _VERSION_MAJOR;
    this->program_minor = _VERSION_MINOR;
    this->program_name  = NEW (String) (buffer);
    this->parameters    = NEW (Map)    (OBJECT_TYPE(String), OBJECT_TYPE(String), (Comparer)String_Cmp);

    for (int i = 1; i < argc; i++) {
      char *arg = argv[i];

      // Ignore empty argument
      if (!arg[0]) continue;

      // TODO: Remove param mode
      if (param_mode || arg[0] != '-') {        
        Args_pcall(this, &param_start, arg);
      }
      else if (arg[1] == '-') {
        if (!arg[2]) {
          // Option end marker, everything after is a parameter
          param_mode = 1;
        } else {
          // Long option
          Args_loptcall(this, &arg[2]);
        }
      } else {
        // Normal option
        for (int j = 1; arg[j]; j++) {
          ArgOption* option = Args_option(arg[j]);

          if (option) {
            if (option->type == ARG_TYPE_NONE) {
              Args_optcall(this, option, NULL);
            } else if (!arg[j + 1]) {
              // Split param option
              if (++i < argc) Args_optcall(this, option, argv[i]);
              else {
                char buffer[2048];

                sprintf(buffer, "Parameter is missing for option '%c'.\n", arg[j]);
                // TODO: Create "ArgException"
                THROW(NEW (Exception)(buffer));
              }
              break;
            } else {
              // Contiguous param option
              Args_optcall(this, option, &arg[++j]);
              break;
            }
          } else {
            char buffer[2048];

            sprintf(buffer, "Unreckognized option '%c'.\n", arg[j]);
            // TODO: Create "ArgException"
            THROW(NEW (Exception)(buffer));
          }
        }
      }
    }
  } else {
    THROW(NEW (MemoryAllocationException)());
  }


  for (int i = 0; _OPTIONS[i].ident; i++) {
    if (_OPTIONS[i].ident == '+' && !Map_At(this->parameters, _OPTIONS[i].name)) {
      // TODO: Make exception constructor able to parse format strings to avoid this
      char buffer[2048];

      sprintf(buffer, "The parameter '%s' is mandatory and wasn't specified", _OPTIONS[i].name);

      THROW(NEW (Exception)(buffer));
    }
  }

  return this;
}

////////////////////////////////////////////////////////////////////////////////
void _(Destruct)()
{
  if (this)
  {
    DELETE (this->program_name);
    DELETE (this->parameters);
  }
}

////////////////////////////////////////////////////////////////////////////////
ArgValue _(Index)(int index)
{
  ArgValue  value = { .as_charptr = NULL };
  Pair     *pair  = Array_At((Array*)this->parameters, index);

  if (pair) {
    ArgOption *param = Args_parameter(Pair_DerefF(pair));

    if (param) value = Args_parse_option(param, Pair_DerefS(pair));
  }

  return value;
}

////////////////////////////////////////////////////////////////////////////////
ArgValue _(Name)(const char* name)
{
  ArgValue  value = { .as_charptr = NULL };
  Pair     *pair  = Map_At(this->parameters, name);

  if (pair) {
    ArgOption *param = Args_parameter(Pair_DerefF(pair));

    if (param) value = Args_parse_option(param, Pair_DerefS(pair));
  }

  return value;
}

////////////////////////////////////////////////////////////////////////////////
Array* _(List)()
{
  Array *list      = NULL;
  int    n_options = Args_sizeof_options();

  if (n_options > 0) {
    ArgOption *option = &_OPTIONS[n_options - 1];

    if (option->ident == '*') {
      Array *params = (Array*)this->parameters;
      
      list = NEW (Array) (sizeof(ArgValue));

      for (int i = 0; i < params->size; i++) {
        Pair *pair = (Pair*)Array_At(params, i);

        if (!strcmp(option->name, Pair_DerefF(pair))) {
          ArgValue value = Args_parse_option(option, Pair_DerefS(pair));

          Array_Push(list, &value);
        }
      }
    }
  }

  return list;
}

