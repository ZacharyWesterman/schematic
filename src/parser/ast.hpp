#pragma once

#include "span.hpp"
#include <memory>
#include <z/core/array.hpp>
#include <z/core/string.hpp>

namespace parser {

struct ast {
	int id;
	zstring text;
	span range;
	z::core::array<std::unique_ptr<ast>> children;
};

} // namespace parser
