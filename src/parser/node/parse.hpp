#pragma once

#include "../ast_node.hpp"
#include "../tokenizer.hpp"
#include "ast/program.hpp"
#include <memory>

namespace parser::node {

auto parse(tokenizer &lexer) -> ref<ast::program>;

auto new_program() -> ref<ast::program>;

} // namespace parser::node
