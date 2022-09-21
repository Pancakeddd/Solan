#define YYDEBUG 1

#include "solan.tab.h"

#include "transform.h"



extern FILE *yyin;

int main()
{
  //yydebug = 1;
	AstBlock *program = nullptr;
  FILE* pt = nullptr;

  fopen_s(&pt, "test.txt", "r");
  if (!pt)
  {
    printf("Failed to open file\n");

    return -1;
  }

  yyin = pt;
  

	yyparse(&program);

  fclose(pt);

  if (program == nullptr)
  {
    return 1;
  }

  Environment e;
  e.makeUnaryType("Null");
  e.makeUnaryType("Number");
  e.makeVariable("test_var", e.types.get("Null"));

  program->touch(transform, e);
	
	return 0;
}