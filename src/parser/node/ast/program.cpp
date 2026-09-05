#include "program.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

auto program::print(std::ostream &stream, int indent) -> void {
	(" "_zs.repeat(indent * 2) + "program").writeln(stream);
	for (auto child : children) {
		child->print(stream, indent + 1);
	}
}

} // namespace parser::node::ast
