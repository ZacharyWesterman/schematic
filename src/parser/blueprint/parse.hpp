#pragma once

#include "../ast_node.hpp"
#include "../tokenizer.hpp"
#include "ast/blueprint.hpp"
#include <memory>

namespace parser::blueprint {

auto parse(tokenizer &lexer) -> ref<ast::blueprint>;

} // namespace parser::blueprint
