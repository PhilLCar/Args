#include <args.h>

VERSION(0, 0);
OPTIONS(
  { "option", 'o', "Specify an option (between 1 and 10)", NULL },
  { "name"  , 'n', "Does the program have a name?"       , NULL },
  { ""      , '+', "If name, specify here"               , NULL },
  { ""      , '-', "Optional"                            , NULL }
);

int main(int argc, char *argv[])
{
  Args *args = NEW (Args) (argc, argv, NULL);

  Args_cint(args, 'c');

  DELETE (args);
}