#include <args.h>

//gcc tst/test.c src/args.c -Iinc -I../Collection/inc -I../Diagnostic/inc -I../OOP/inc -I../IO/inc -I../String/inc -I../Stream/inc -Llib -L../String/lib -lstring0 -L../OOP/lib -loop0 -L../Stream/lib -lstream0 -L../Collection/lib -lcollection0 -Llib -largs0

//VERSION(0, 0);
OPTIONS(
  { "option", 'o', "Specify an option (between 1 and 10)", ARG_TYPE_INTEGER, NULL },
  { "name"  , 'n', "Does the program have a name?"       , ARG_TYPE_CHARPTR, NULL },
  { ""      , '+', "If name, specify here"               , ARG_TYPE_CHARPTR, NULL },
  { ""      , '-', "Optional"                            , ARG_TYPE_ANY,     NULL }
);

int main(int argc, char *argv[])
{
  Args *args = NEW (Args) (argc, argv, NULL);

  

  DELETE (args);
}