#include "environment.h"

#include <stdio.h>
#include <format>

#include "ast.h"

std::string Environment::strToFunction(std::string fun_name)
{
	return std::format("Function_{}", fun_name);
}

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

bool Environment::getVariableStrict(Variable** ovariable, std::string name)
{
	auto ty = vars.get(name);

	if (ty != nullptr)
	{
		*ovariable = ty;
		return true;
	}

	return false;
}

void Environment::throwError(int row, int column, std::string info)
{
	printf("%d, %d: Error: %s\n", row, column, info.c_str());
}

void Environment::throwError(Ast *ast, std::string info)
{
	throwError(ast->location.line_i, ast->location.char_i, info);
}

TypeDescription* Environment::makeUnaryType(std::string name)
{
	auto ty = types.make(name);
	ty->addIndex("value", ty);
	return ty; // Creates a unique PTR type description then returns raw PTR.
}

TypeDescription* Environment::makeEmptyType(std::string name)
{
	return types.make(name);
}

TypeDescription* Environment::makeFunctionType(std::string name, TypeDescription* return_type, AstBlock* args)
{
	auto type = types.make(strToFunction(name));
	type->indexes["return_type"] = return_type;

	for (size_t i = 0; i < args->contained.size(); ++i)
	{
		auto arg = args->contained[i];

		type->addIndex(std::to_string(i), arg->type);
	}

	return type;
}

Variable* Environment::makeVariable(std::string name, TypeDescription* type)
{
	// Creates variable with TypeDescription "type" and returns raw PTR.
	auto variable = vars.make(name);
	variable->type = type;
	return variable;
}
