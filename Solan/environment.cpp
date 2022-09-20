#include "environment.h"

#include <stdio.h>

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

void Environment::throwError(int row, int column, std::string info)
{
	printf("%d, %d: Error: %s", row, column, info.c_str());
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
