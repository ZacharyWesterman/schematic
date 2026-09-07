#include "tag_decl.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

auto tag_decl::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	(indent_text + "tag_decl [" + name.text + "] = " + description.text).writeln(stream);
	indent_text += "  ";

	if (parent) {
		(indent_text + "parent: " + parent.value().text).writeln(stream);
	}
	if (help_text) {
		(indent_text + "help-text: " + help_text.value().text).writeln(stream);
	}
}

} // namespace parser::node::ast
