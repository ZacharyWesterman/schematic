#pragma once

#include "../tokenizer.hpp"
#include <z/core/generator.hpp>

namespace parser {

auto node_parser(const zstring &text) -> tokenizer;

}
