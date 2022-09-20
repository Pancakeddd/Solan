#pragma once

#include <string>


#include "description.h"

struct Environment
{
	TypeDescription types;
	ScopeVariables vars;

	Environment createSubScope();
	TypeDescription* makeUnaryType(std::string name);
	Variable* makeVariable(std::string name, TypeDescription* type);

	TypeDescription* getTypeOfVariable(std::string name);

	void throwError(int row, int column, std::string info);
};