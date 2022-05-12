#pragma once

// #include "error.hpp"
#include "boehmgc.hpp"

#include <cstring>
#include <iostream>
#include <memory>
// #include "scope_stack.hpp"

class ScopeStack;

extern ScopeStack *scopes;
/* Output Indentation */
void newLine(std::ostream &);
void incIndent(std::ostream &);
void decIndent(std::ostream &);