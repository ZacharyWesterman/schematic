#include "constraint.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

static auto text(token tok) -> zstring {
	return tok.text;
}

static auto join(const zstring &a, const zstring &b) -> zstring {
	return a + ", " + b;
}

auto constraint::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	auto argstr = args.map<zstring>(text).reduce({}, join);

	(indent_text + "constraint [" + name.text + "] = " + argstr).writeln(stream);
	indent_text += "  ";
}

} // namespace parser::node::ast
