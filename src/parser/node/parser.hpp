#pragma once

#include "../ast.hpp"
#include "../tokenizer.hpp"
#include <memory>

namespace parser {

auto node_parser(tokenizer &lexer) -> std::unique_ptr<ast>;

}
