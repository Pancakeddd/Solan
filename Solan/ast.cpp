#include "ast.h"

#include <format>

std::string Ast::getIdentifier()
{
	return as<AstIdentifier>()->identifier;
}

std::deque<SubAst> &Ast::getContained()
{
	return as<AstBlock>()->contained;
}

void AstFunction::addArgumentsToScope(Environment& env)
{
	for (auto arg : arguments->getContained())
	{
		auto func_arg = arg->as<AstFunctionArgument>();
		TypeDescription* ty;

		if (env.getTypeStrict(&ty, func_arg->tyname->getIdentifier()))
		{
			env.makeVariable(func_arg->name->getIdentifier(), ty);
		}
		else {
			env.throwError(func_arg, std::format("unknown type '{}'", func_arg->tyname->getIdentifier()));
		}
	}
}