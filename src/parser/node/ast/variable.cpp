#include "variable.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

auto variable::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	(indent_text + "var [" + name.text + ", " + type.text + "] = " + description.text).writeln(stream);
	for (auto i : constraints) {
		i->print(stream, indent + 1);
	}
}

} // namespace parser::node::ast
