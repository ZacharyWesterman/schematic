#pragma once

#include "span.hpp"
#include <z/core/string.hpp>

namespace parser {

struct context {
	span line;
	span col;

	context(const zstring &text, span range);
};

} // namespace parser
