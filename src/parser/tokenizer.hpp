#pragma once

#include "token.hpp"
#include <z/core/generator.hpp>
#include <z/core/string.hpp>

namespace parser {

struct programText {
	zstring text;
	int index;
};

typedef z::core::generator<token, programText> tokenizer;

} // namespace parser
