#pragma once

#include "../tokenizer.hpp"
#include <z/core/generator.hpp>

namespace parser::node {

auto lex(const zstring &text) -> tokenizer;

}
