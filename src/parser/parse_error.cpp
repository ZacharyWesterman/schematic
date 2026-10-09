#include "parse_error.hpp"

namespace parser {

parse_error::parse_error(const zstring message, const zstring filename, span range) : message(message), filename(filename), range(range) {}

const char *parse_error::what() const noexcept {
	return message.cstring();
}

} // namespace parser
