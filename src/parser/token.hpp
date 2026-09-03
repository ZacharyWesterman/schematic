#pragma once

#include "span.hpp"
#include <z/core/string.hpp>

namespace parser {

struct token {
	int id;
	span range;
	zstring text;
	double value = 0.0;
};

} // namespace parser
