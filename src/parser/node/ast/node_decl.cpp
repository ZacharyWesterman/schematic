#include "node_decl.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

auto text(token tok) -> zstring {
	return tok.text;
}

auto join(const zstring &a, const zstring &b) -> zstring {
	return a + ", " + b;
}

auto node_decl::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	(indent_text + "node_decl [" + name.text + "]").writeln(stream);
	indent_text += "  ";

	if (description.text) {
		(indent_text + "desc: " + description.text).writeln(stream);
	}
	(indent_text + "tags: " + tags.map<zstring>(text).reduce({}, join)).writeln(stream);

	if (help_text) {
		(indent_text + "help-text: " + help_text.value().text).writeln(stream);
	}
}

} // namespace parser::node::ast
