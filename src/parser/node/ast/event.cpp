#include "event.hpp"

namespace parser::node::ast {

static auto text(token tok) -> zstring {
	return tok.text;
}

static auto join(const zstring &a, const zstring &b) -> zstring {
	return a + ", " + b;
}

auto event::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	(indent_text + "event [" + triggers.map<zstring>(text).reduce(join) + "] = `...`").writeln(stream);
}

} // namespace parser::node::ast
