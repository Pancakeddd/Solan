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
%token SOLAN_LET SOLAN_COLON SOLAN_NULL SOLAN_FUNCTION SOLAN_COMMA SOLAN_END SOLAN_RETURN SOLAN_TYPE

%token <text> SOLAN_WORD
%token <num> SOLAN_NUMBER

%right SOLAN_EQL
%left SOLAN_ADD SOLAN_SUB
%left SOLAN_MUL SOLAN_DIV

%type<asttype> statement statement_let statement_function_def statement_return
	identifier number null 
	block function_arg_list arguments line
	definition type_definition type_block subtype_definition
	value expr expr_bi_operator expr_type_construct expr_function_call
%type<program> program
%type<op> operator

%destructor { free($$); } <text>

%%
	solan: program

	// Blocks, List and Program constructs

	program:
	  block { *result = $1->as<AstBlock>(); }
	  ;

	block:
	  line block {
		$$ = $2;
		$$->getContained().push_front($1);
	  }
	  | line {
	    $$ = CreateAstL(AstBlock, @1.first_line, @1.first_column);
	    $$->getContained().push_back($1);
	  }
	  | {
	    $$ = CreateAstL(AstBlock, @0.first_line, @0.first_column);
	    
	  }
	  ;

	line: 
	  value
	  | statement
	  | definition
	  ;

	function_arg_list:
	  identifier SOLAN_COLON identifier function_arg_list
	    {
		$$ = $4;
		auto ast = CreateAstL(AstFunctionArgument, @1.first_line, @1.first_column);
		ast->name = $1;
		ast->tyname = $3;
		$$->getContained().push_front(ast);
	    }
	  | identifier SOLAN_COLON identifier
	    {
	    $$ = CreateAstL(AstBlock, @1.first_line, @1.first_column);
		auto ast = CreateAstL(AstFunctionArgument, @1.first_line, @1.first_column);
		ast->name = $1;
		ast->tyname = $3;
	    $$->getContained().push_back(ast);
	    }


	arguments:
	  value SOLAN_COMMA arguments
	    {
		 $$ = $3;
		 $$->getContained().push_front($1);
	    }
	  | value 
	   {
	    $$ = CreateAstL(AstBlock, @1.first_line, @1.first_column);
	    $$->getContained().push_back($1);
	   }
	   ;

	// Definitions

	subtype_definition:
	  identifier SOLAN_COLON identifier
	    {
		auto ast = CreateAstL(AstSubTypeDefinition, @1.first_line, @1.first_column);
		ast->name = $1;
		ast->tyname = $3;
		$$ = ast;
	    }
	  ;

	type_block:
	  subtype_definition type_block
	    {
		$$ = $2;
		$$->getContained().push_front($1);
	    }
	  | subtype_definition
	    {
	    $$ = CreateAstL(AstBlock, @1.first_line, @1.first_column);
	    $$->getContained().push_back($1);
	    }

	definition:
	  type_definition
	  ;

	type_definition:
	  SOLAN_TYPE identifier type_block SOLAN_END
	    {
		auto ast = CreateAstL(AstTypeDefinition, @1.first_line, @1.first_column);
		ast->name = $2;
		ast->subtypes = $3;
		$$ = ast;
	    }
	  ;

	// Statements 

	statement: 
	  statement_let
	  | statement_function_def
	  | statement_return
	  ;

	statement_let: 
	  SOLAN_LET identifier SOLAN_COLON identifier SOLAN_EQL value
	    {
		    auto ast = CreateAstL(AstSet, @1.first_line, @1.first_column);
			ast->l = $2;
			ast->r = $6;
			ast->specified_type = $4;
			$$ = ast;
	    }
	  | SOLAN_LET identifier SOLAN_EQL value
		{
			auto ast = CreateAstL(AstSet, @1.first_line, @1.first_column);
			ast->l = $2;
			ast->r = $4;
			ast->specified_type = std::nullopt;
			$$ = ast;
		}
	  ;

	statement_function_def:
	  SOLAN_LET identifier function_arg_list SOLAN_EQL block SOLAN_END
	    {
			auto ast = CreateAstL(AstFunction, @1.first_line, @1.first_column);
			ast->name = $2;
			ast->arguments = $3;
			ast->code = $5;
			$$ = ast;
		}
	  ;

	statement_return:
	  SOLAN_RETURN value
	    {
			auto ast = CreateAstL(AstReturn, @1.first_line, @1.first_column);
			ast->returnvalue = $2;
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
	  | expr_type_construct
	  | expr_function_call
	  ;

	expr_bi_operator:
	  value operator value 
	  {
		auto ast = CreateAstL(AstOperator, @1.first_line, @1.first_column);
		ast->l = $1;
		ast->optype = $2;
		ast->r = $3;
		$$ = ast;
	  }
	  ;

	expr_type_construct:
	  identifier '{' arguments '}'
	  {
	    auto ast = CreateAstL(AstConstruct, @1.first_line, @1.first_column);
		ast->name = $1;
		ast->args = $3;
		$$ = ast;
	  }
	  ;

	expr_function_call:
	  identifier '(' arguments ')'
	  {
	    auto ast = CreateAstL(AstFunctionCall, @1.first_line, @1.first_column);
		ast->name = $1;
		ast->args = $3;
		$$ = ast;
	  }
	  ;

	value: 
	  identifier
	  | number
	  | expr
	  | null
	  | '(' value ')' {$$ = $2;}
	  ;

	null:
	  SOLAN_NULL
	  {
	    $$ = CreateAstL(AstNull, @1.first_line, @1.first_column);
	  }
	
	identifier:
	  SOLAN_WORD
	  {
		auto ast = CreateAstL(AstIdentifier, @1.first_line, @1.first_column);
		ast->identifier = std::string($1);
		$$ = ast;
	  }
	  ;

	number:
		SOLAN_NUMBER
		{
			auto ast = CreateAstL(AstNumber, @1.first_line, @1.first_column);
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