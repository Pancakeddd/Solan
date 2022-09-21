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

			Variable* v;
			if (env.getVariableStrict(&v, identifier_string))
			{
				ast->type = v->type;
			}
			else {
				env.throwError(ast, std::format("unknown variable '{}'", identifier_string));
			}
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
			else {
				set->type = set->r->type;
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
			break;
		}

		case AstType::ASTRETURN:
		{
			ast->type = ast->as<AstReturn>()->returnvalue->type;
			break;
		}

		case AstType::ASTFUNCTIONDEF:
		{
			auto func = ast->as<AstFunction>();
			
			// Find return, if there is no return then the func's return type will be "Null"

			func->return_type = env.types.get("Null");

			for (auto sub_a : func->code->as<AstBlock>()->contained)
			{
				if (sub_a->getAstType() == ASTRETURN)
				{
					func->return_type = sub_a->as<AstReturn>()->type;
					break;
				}
			}

			break;
		}
	}
	

	return true;
}

void set_UpdateVariable(AstSet* set, Environment& env)
{
	auto variable_name = set->l->as<AstIdentifier>()->identifier;
	env.makeVariable(variable_name, set->type);
}

void function_CreateFunction(AstFunction* func, Environment& env)
{
	auto func_name = func->name->as<AstIdentifier>()->identifier;
	auto ty = env.makeFunctionType(func_name, func->return_type);
	env.makeVariable(func_name, ty);
}

bool transform(Ast* ast, Environment& env)
{
	discoverTypes(ast, env);

	if (ast->getAstType() == AstType::ASTSET)
	{
		set_UpdateVariable(ast->as<AstSet>(), env);
	}

	if (ast->getAstType() == AstType::ASTFUNCTIONDEF)
	{
		function_CreateFunction(ast->as<AstFunction>(), env);
	}

	return true;
}
