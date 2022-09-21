#include "environment.h"

#include <stdio.h>

#include "ast.h"

Environment Environment::createSubScope()
{
	Environment e;
	e.types.parent_description = &types;
	e.vars.parent_description = &vars;
	return e;
}

TypeDescription* Environment::getTypeOfVariable(std::string name)
{
	Variable* v = vars.get(name);

	if (v != nullptr)
	{
		return v->type;
	}

	return nullptr;
}

bool Environment::getTypeStrict(TypeDescription** otype, std::string name)
{
	auto ty = types.get(name);
	
	if (ty != nullptr)
	{
		*otype = ty;
		return true;
	}

	return false;
}

void Environment::throwError(int row, int column, std::string info)
{
	printf("%d, %d: Error: %s", row, column, info.c_str());
}

void Environment::throwError(Ast *ast, std::string info)
{
	throwError(ast->location.line_i, ast->location.char_i, info);
}

TypeDescription* Environment::makeUnaryType(std::string name)
{
	return (types.sub_descriptions[name] = std::make_unique<TypeDescription>()).get(); // Creates a unique PTR type description then returns raw PTR.
}

Variable* Environment::makeVariable(std::string name, TypeDescription* type)
{
	// Creates variable with TypeDescription "type" and returns raw PTR.

	auto variable = (vars.sub_descriptions[name] = std::make_unique<Variable>()).get();
	variable->type = type;
	return variable;
}
