#pragma once

#include <functional>

#include "ast.h"

bool discoverTypes(Ast* ast, Environment& env);
void set_UpdateVariable(AstSet* set, Environment& env);
void function_CreateFunction(AstFunction* func, Environment& env);

bool transform(Ast* ast, Environment& env);