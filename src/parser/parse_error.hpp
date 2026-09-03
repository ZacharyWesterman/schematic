#pragma once

#include "span.hpp"
#include <exception>
#include <z/core/string.hpp>

namespace parser {

struct parse_error : public std::exception {
	const zstring message;
	const span context;

	parse_error(const zstring message, span context);

	const char *what() const noexcept override;
};

} // namespace parser
