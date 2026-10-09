#pragma once

#include "span.hpp"
#include <exception>
#include <z/core/string.hpp>

namespace parser {

struct parse_error : public std::exception {
	const zstring message;
	const zstring filename;
	const span range;

	parse_error(const zstring message, const zstring filename, span range);

	const char *what() const noexcept override;
};

} // namespace parser
