#include "print_error.hpp"
#include "context.hpp"
#include <iostream>

namespace parser {

auto print_error(std::istream &file, const parse_error &error) -> void {
	context ctx(file, error.range);
	int caretlen = ctx.col.end - ctx.col.begin;

	std::cerr << "[ERROR] " << ctx.line.begin << ", " << ctx.col.begin << ": " << error.message << '\n';
	std::cerr << "[CONTEXT] " << ctx.text.replace('\t', ' ').replace('\n', "\n[CONTEXT] ") << '\n';
	std::cerr << "[CONTEXT] " << ' '_zs.repeat(ctx.col.begin - 1) << '^'_zs.repeat(caretlen);
	std::cerr << std::endl;
}

} // namespace parser
