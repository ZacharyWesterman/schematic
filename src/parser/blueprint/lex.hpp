#pragma once

#include "../tokenizer.hpp"
#include <z/core/generator.hpp>

namespace parser::blueprint {

auto lex(const zstring &text, const zstring &filename) -> tokenizer;

}
