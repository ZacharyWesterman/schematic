#pragma once

#include "../ast.hpp"
#include "../tokenizer.hpp"
#include <memory>

namespace parser::node {

auto parse(tokenizer &lexer) -> std::unique_ptr<ast>;

}
