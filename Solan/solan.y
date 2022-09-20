%{
 #include "stdio.h"
 #include "stdlib.h"

 extern int yylex();
 extern int yyparse();

 
%}

%code provides
{
void yyerror(AstBlock **s, const char *str);
//FILE *yyin;
}

%code requires { 
#include "ast.h" 

  template <typename T>
  T* CreateAst(Location l)
  {
    auto x = new T();
    x->location = l;
    return x;
  }

  #define CreateAstL(T, F, L) CreateAst<T>({F, L})
}

%union
{
	SubAst asttype;
	AstBlock *program;
	char *text;
	double num;
	Operator op;
}

%parse-param {AstBlock **result}

%locations

%error-verbose

%token<op> SOLAN_EQL SOLAN_ADD SOLAN_SUB SOLAN_MUL SOLAN_DIV
%token SOLAN_LET SOLAN_COLON

%token <text> SOLAN_WORD
%token <num> SOLAN_NUMBER

%right SOLAN_EQL
%left SOLAN_ADD SOLAN_SUB
%left SOLAN_MUL SOLAN_DIV

%type<asttype> statement statement_let identifier number block value expr expr_bi_operator
%type<program> program
%type<op> operator

%destructor { free($$); } <text>

%%
	solan: program

	// Blocks and Program constructs

	program:
	  block { *result = $1->as<AstBlock>(); }
	  ;

	block:
	  statement block {
		$$ = $2;
		$$->as<AstBlock>()->contained.push_front($1);
	  }
	  | statement {
	    $$ = CreateAstL(AstBlock, @$.first_line, @$.first_column);
	    $$->as<AstBlock>()->contained.push_back($1);
	  }
	  ;

	// Statements 

	statement: 
	  statement_let
	  ;

	statement_let: 
	  SOLAN_LET identifier SOLAN_COLON identifier SOLAN_EQL value
	    {
		    auto ast = CreateAstL(AstSet, @$.first_line, @$.first_column);
			ast->l = $2;
			ast->r = $6;
			ast->specified_type = $4;
			$$ = ast;
	    }
	  | SOLAN_LET identifier SOLAN_EQL value
		{
			auto ast = CreateAstL(AstSet, @$.first_line, @$.first_column);
			ast->l = $2;
			ast->r = $4;
			ast->specified_type = std::nullopt;
			$$ = ast;
		}
	  ;

	// Values/Expressions

	operator:
	  SOLAN_ADD {$$ = Operator::ADD;}
	  | SOLAN_SUB {$$ = Operator::SUB;}
	  | SOLAN_MUL {$$ = Operator::MUL;} 
	  | SOLAN_DIV {$$ = Operator::DIV;}
	  ;

	expr:
	  expr_bi_operator
	  ;

	expr_bi_operator:
	  value operator value 
	  {
		auto ast = CreateAstL(AstOperator, @$.first_line, @$.first_column);
		ast->l = $1;
		ast->optype = $2;
		ast->r = $3;
		$$ = ast;
	  }
	  ;

	value: 
	  identifier
	  | number
	  | expr
	  | '(' value ')' {$$ = $2;}
	  ;
	
	identifier:
	  SOLAN_WORD
	  {
		auto ast = CreateAstL(AstIdentifier, @$.first_line, @$.first_column);
		ast->identifier = std::string($1);
		$$ = ast;
	  }
	  ;

	number:
		SOLAN_NUMBER
		{
			auto ast = CreateAstL(AstNumber, @$.first_line, @$.first_column);
			ast->num = $1;
			$$ = ast;
		}
		;

	// End
%%

void yyerror(AstBlock **s, const char *str)
{
	printf("Syntax error: %s\n", str);
	return;
}