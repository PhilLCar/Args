#include <diagnostic.h>
#include <args.h>

typedef struct _env {
  int         num;
  const char *name;
} Env;

void setnum(Args *args, ArgValue value)
{
  Env *env = args->env;

  env->num = value.as_integer;
}

void setname(Args *args, ArgValue value)
{
  Env *env = args->env;

  env->name = value.as_charptr;
}

OPTIONS(
  { "option", 'o', "Specify an option (between 1 and 10)", ARG_TYPE_INTEGER, setnum  },
  { "name"  , 'n', "Does the program have a name?"       , ARG_TYPE_CHARPTR, setname },
  { "test"  , '+', "If name, specify here"               , ARG_TYPE_CHARPTR, NULL    },
  { "rest"  , '*', "Optional"                            , ARG_TYPE_INTEGER, NULL    }
);

int main(int argc, char *argv[])
{
  Env   env = { 0, "" };
  Args *args = NEW (Args) (argc, argv, &env);

  CHECK_MEMORY

  if (env.num)
  {
    printf("Num: %d\n", env.num);
  }

  if (env.name[0])
  {
    printf("Name: %s\n", env.name);
  }

  Array *list = Args_List(args);

  for (int i = 0; i < list->size; i++)
  {
    int e = *(int*)Array_At(list, i);

    printf("Index %d: %d\n", i, e);
  }

  DELETE (list);

  DELETE (args);

  CHECK_MEMORY

  STOP_WATCHING
}