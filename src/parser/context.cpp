#include "context.hpp"
#include <iostream>

namespace parser {

context::context(const zstring &text, span range) {
	zstring prev = text.substr(0, range.start);
	int lines_before = prev.count("\n");

	zstring inside = text.substr(range.start, range.end);
	int lines_inside = inside.count("\n");

	line.start = lines_before + 1;
	int pos = prev.findLast("\n");
	col.start = (pos < 0 ? prev.length() + 1 : pos) + 1;

	std::cout << lines_before << std::endl;

	line.end = line.start + lines_inside;
	pos = inside.findLast("\n");
	col.end = pos < 0 ? col.start - 1 + inside.length() : inside.length() - pos;
}

} // namespace parser
