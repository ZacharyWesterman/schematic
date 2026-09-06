#pragma once

#include "span.hpp"
#include <istream>
#include <z/core/string.hpp>

namespace parser {

struct context {
	span line;
	span col;
	zstring text;

	context(std::istream &stream, span range);
};

} // namespace parser
