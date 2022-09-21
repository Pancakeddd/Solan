#include "transform.h"
#include "types.h"

#include <format>

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

		case AstType::ASTNULL:
		{
			ast->type = env.types.get("Null");
			break;
		}

		case AstType::ASTSET:
		{
			auto set = ast->as<AstSet>();
			if (set->isSpecified())
			{
				auto type_name = set->specified_type.value()
					->as<AstIdentifier>()
					->identifier;

				if (!env.getTypeStrict(&set->type, type_name)) // If the type doesn't exist.
				{
					env.throwError(set, std::format("unknown type '{}'.", type_name));
				}

				if (!typecheckSet(set)) // Failed check;
				{
					env.throwError(set, std::format("typecheck mismatch between '{}' and right hand side.", type_name));
				}
			}
			break;
		}

		case AstType::ASTOPERATION:
		{
			auto op = ast->as<AstOperator>();

			if (!typecheckOp(op))
			{
				env.throwError(op, "type mismatch between Lhs and Rhs.");
			}

			op->type = op->l->type;
		}
		break;
	}
	

	return true;
}

void set_UpdateVariable(AstSet* set, Environment& env)
{
	auto variable_name = set->l->as<AstIdentifier>()->identifier;
	env.makeVariable(variable_name, set->type);
}

bool transform(Ast* ast, Environment& env)
{
	discoverTypes(ast, env);

	if (ast->getAstType() == AstType::ASTSET)
	{
		set_UpdateVariable(ast->as<AstSet>(), env);
	}

	return true;
}
