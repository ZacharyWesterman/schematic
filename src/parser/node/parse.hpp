#pragma once

#include "../ast_node.hpp"
#include "../tokenizer.hpp"
#include <memory>

namespace parser::node {

auto parse(tokenizer &lexer) -> ast_ref;

}
