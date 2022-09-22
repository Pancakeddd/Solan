#include "ast.h"

std::string Ast::getIdentifier()
{
	return as<AstIdentifier>()->identifier;
}
