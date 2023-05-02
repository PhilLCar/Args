#include <args.h>

#define TYPENAME Args

ArgOption _DEFAULT_OPTIONS[] = {
  { "help"   , 'h', "Displays this message"       , _default_help    },
  { "version", 'v', "Displays the program version", _default_version }
};

/******************************************************************************/
void _default_help(Args *args) {
  Coordinate size = gettermsize();
  
  int opt_size = ({ int size; for (size = 0; _OPTIONS[size].name; size++); size; });
  int def_size = sizeof(_DEFAULT_OPTIONS) / sizeof(ArgOption);

  int max_name = 0;
  int max_desc = 0;

  for (int i = 0; i < opt_size; i++) {
    
  }

  exit(0);
}

/******************************************************************************/
void _default_version(Args *args) {

  exit(0);
}

////////////////////////////////////////////////////////////////////////////////
Args *_(cons)(int argc, char *argv[], void *env)
{
  if (_this) {
    _this->env = env;
  }
}

////////////////////////////////////////////////////////////////////////////////
void _(free)()
{
  DELETE (_this->parameters);
  DELETE (_this->positionals);
}