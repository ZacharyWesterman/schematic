#include "tag_decl.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

auto tag_decl::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	(indent_text + "tag_decl [" + name.text + "]").writeln(stream);
	indent_text += "  ";

	if (parent) {
		(indent_text + "parent: " + parent.value().text).writeln(stream);
	}
	if (description.text) {
		(indent_text + "desc: " + description.text).writeln(stream);
	}
}

} // namespace parser::node::ast
