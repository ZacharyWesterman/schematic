#include "field.hpp"

namespace parser::blueprint::ast {

auto field::print(std::ostream &stream, int indent) -> void {
	("  "_zs.repeat(indent) + "field: " + name.text).writeln(stream);
	args->print(stream, indent + 1);
}

} // namespace parser::blueprint::ast
