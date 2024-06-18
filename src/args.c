#include <args.h>

#define TYPENAME Args

void _default_help(Args*, ArgValue);
void _default_version(Args*, ArgValue);

ArgOption _DEFAULT_OPTIONS[] = {
  { "help"   , 'h', "Displays this message"       , ARG_TYPE_NONE, _default_help    },
  { "version", 'v', "Displays the program version", ARG_TYPE_NONE, _default_version }
};

/******************************************************************************/
int _sizeof_options() {
  ArgOption end  = END_OPTION;
  int       size = 0;

  do
  {
    ArgOption *option = &_OPTIONS[size];

    if (!strcmp(option->name, end.name) 
      && option->ident == end.ident 
      && !strcmp(option->desc, end.desc)
      && option->callback == end.callback)
    {
      break;
    }
  } while (++size);
  
  return size;
}

/******************************************************************************/
int _param_start() {
  ArgOption end   = END_OPTION;
  int       start = 0;

  do
  {
    ArgOption *option = &_OPTIONS[start];
    char       o      = option->ident;

    if (o == '+' || o == '-' || o == '*' || o == end.ident)
    {
      break;
    }
  } while (++start);
  
  return start;
}

/******************************************************************************/
void _default_help(Args *args, ArgValue value) {
  //Coordinate size = gettermsize();
  
  int opt_size = _sizeof_options();
  //int def_size = sizeof(_DEFAULT_OPTIONS) / sizeof(ArgOption);

  int max_name = 0;

  for (int i = 0; i < opt_size; i++) {
    int len = strlen(_OPTIONS[i].name) + 2;

    if (len > max_name)
    {
      max_name = len;
    }
  }

  printf("USAGE:\n");
  printf("\t%s [OPTIONS]", args->program_name->base);

  for (int i = _param_start(), start = i; i < opt_size; i++)
  {
    ArgOption *option = &_OPTIONS[i];

    if (!option->name[0])
    {
      printf(" param%d", i - start);
    }
    else if (option->ident == '+')
    {
      printf(" %s", option->name);
    }
    else if (option->ident == '-')
    {
      printf(" [%s]", option->name);
    }
    else if (option->ident == '*')
    {
      printf(" %s...", option->name);
    }
    else
    {
      fprintf(stderr, "Badly formed parameter: '%s'\nOnly '+', '-', '*' are allowed.\n", option->name);
      break;
    }
  }

  printf("\nOPTIONS:\n");

  printf("PARAMETERS:\n");

  exit(0);
}

/******************************************************************************/
void _default_version(Args *args, ArgValue value) {
  printf("Program: %s\n Version: %d.%d\n", args->program_name->base, args->program_major, args->program_minor);
  exit(0);
}

/******************************************************************************/
ArgValue _parse_option(ArgOption *option, const char *parameter)
{
  ArgValue value;

  switch (option->type)
  {
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

  if (parameter->ident != ' ' && parameter->ident != '*')
  {
    (*index)++;
  }

  Map_setkey(_this->parameters, NEW (String) (parameter->name), NEW (String) (value));
}

/******************************************************************************/
ArgOption* _(option)(char ident)
{
  ArgOption  last  = END_OPTION;
  ArgOption *found = NULL;

  int n_default = sizeof(_DEFAULT_OPTIONS) / sizeof(ArgOption);

  for (int i = 0; _OPTIONS[i].ident != last.ident; i++)
  {
    if (_OPTIONS[i].ident == ident)
    {
      found = &_OPTIONS[i];
      break;
    }
  }

  if (!found)
  {
    for (int i = 0; i < n_default; i++)
    {
      if (_DEFAULT_OPTIONS[i].ident == ident)
      {
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
  ArgOption  last  = END_OPTION;
  ArgOption *found = NULL;

  int n_default = sizeof(_DEFAULT_OPTIONS) / sizeof(ArgOption);

  for (int i = 0; _OPTIONS[i].ident != last.ident; i++)
  {
    if (!strcmp(_OPTIONS[i].name, name))
    {
      found = &_OPTIONS[i];
      break;
    }
  }

  if (!found)
  {
    for (int i = 0; i < n_default; i++)
    {
      if (!strcmp(_DEFAULT_OPTIONS[i].name, name))
      {
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
  if (param)
  {
    name = buffer;
    memcpy(buffer, option, ++param - option);
  }

  {
    ArgOption *opt = Args_loption(_this, name);

    if (opt)
    {
      Args_optcall(_this, opt, param);
    }
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

    _this->env           = env;
    _this->program_major = _VERSION_MAJOR;
    _this->program_minor = _VERSION_MINOR;
    _this->program_name  = NEW (String) (buffer);
    _this->parameters    = NEW (Map)    (sizeof(String), sizeof(String), (Comparer)String_equals);

    for (int i = 1; i < argc; i++)
    {
      char *arg = argv[i];

      // Ignore empty argument
      if (!arg[0]) continue;

      if (param_mode || arg[0] != '-')
      {
        // Add parameter
        param_mode = 1;
        
        Args_parameter(_this, &param_start, arg);
      }
      else if (arg[1] == '-')
      {
        if (!arg[2])
        {
        // Option end marker, everything after is a parameter
          param_mode = 1;
        }
        else
        {
          // Long option
          Args_loptcall(_this, &arg[2]);
        }
      } else {

        // Normal option
        for (int j = 1; arg[j]; j++)
        {
          ArgOption* option = Args_option(_this, arg[j]);

          if (option)
          {
            if (option->type == ARG_TYPE_NONE)
            {
              Args_optcall(_this, option, NULL);
            }
            else if (!arg[j + 1])
            {
              // Split param option
              Args_optcall(_this, option, argv[++i]);
              break;
            }
            else
            {
              // Contiguous param option
              Args_optcall(_this, option, &arg[++j]);
              break;
            }
          }
          else
          {
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


  for (int i = 0; i < params->size; i++)
  {
    Pair *pair = (Pair*)Array_at(params, i);

    if (!strcmp(option->name, *(char**)pair->first))
    {
      ArgValue value = _parse_option(option, *(char**)pair->second);

      Array_push(list, &value);
    }
  }

  return list;
}

