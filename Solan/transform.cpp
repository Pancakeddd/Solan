#include "transform.h"

bool discoverTypes(Ast* ast, Environment& env)
{
	// Find and assign types to AST members.
	switch (ast->getAstType())
	{
		case AstType::NUMBER:
		{
			ast->type = env.types.get("Number");
			break;
		}

		case AstType::IDENTIFIER:
		{
			auto identifier_string = ast->as<AstIdentifier>()->identifier;
			ast->type = env.getTypeOfVariable(identifier_string);
			break;
		}

		case AstType::ASTSET:
		{
			auto right_hand_type = ast->as<AstSet>()->r->type;
			ast->type = right_hand_type;

			set_UpdateVariable(ast->as<AstSet>(), env);
		}
	}
	

	return true;
}

void set_UpdateVariable(AstSet* set, Environment& env)
{
	auto variable_name = set->l->as<AstIdentifier>()->identifier;
	env.makeVariable(variable_name, set->type);
}
