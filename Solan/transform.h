#pragma once

#include <functional>

#include "ast.h"

bool discoverTypes(Ast* ast, Environment& env);
void set_UpdateVariable(AstSet* set, Environment& env);

bool transform(Ast* ast, Environment& env);