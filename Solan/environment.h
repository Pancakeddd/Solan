#pragma once

#include <string>

#include "description.h"

struct Ast;
struct AstBlock;

struct Environment
{
	TypeDescription types;
	ScopeVariables vars;

	std::string strToFunction(std::string fun_name);

	Environment createSubScope();
	TypeDescription* makeUnaryType(std::string name);
	TypeDescription* makeEmptyType(std::string name);
	TypeDescription* makeFunctionType(std::string name, TypeDescription* return_type, AstBlock *args);
	Variable* makeVariable(std::string name, TypeDescription* type);

	TypeDescription* getTypeOfVariable(std::string name);
	bool getTypeStrict(TypeDescription** otype, std::string name);
	bool getVariableStrict(Variable** ovariable, std::string name);

	void throwError(int row, int column, std::string info);
	void throwError(Ast *ast, std::string info);
};