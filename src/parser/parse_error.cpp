#include "parse_error.hpp"

namespace parser {

parse_error::parse_error(const zstring message, span range) : message(message), range(range) {}

const char *parse_error::what() const noexcept {
	return message.cstring();
}

} // namespace parser
