#include "types.h"

bool typecheckSet(AstSet* set)
{
	if (set->type == nullptr)
	{
		return true;
	}

	return set->type == set->r->type;
}

bool typecheckOp(AstOperator* op)
{
	return op->l->type == op->r->type;
}
