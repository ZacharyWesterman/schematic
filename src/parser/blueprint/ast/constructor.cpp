#include "constructor.hpp"

namespace parser::blueprint::ast {

auto constructor::print(std::ostream &stream, int indent) -> void {
	("  "_zs.repeat(indent) + "constructor").writeln(stream);
	position->print(stream, indent + 1);
}

} // namespace parser::blueprint::ast
