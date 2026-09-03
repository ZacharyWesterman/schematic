#pragma once

#include "span.hpp"
#include <exception>

namespace parser {

struct parse_error : public std::exception {
	const char *message;
	const span context;

	parse_error(const char *message, span context);

	const char *what() const noexcept override;
};

} // namespace parser
