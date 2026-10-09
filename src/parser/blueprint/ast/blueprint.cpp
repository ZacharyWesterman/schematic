#include "blueprint.hpp"

namespace parser::blueprint::ast {

auto blueprint::print(std::ostream &stream, int indent) -> void {
	("  "_zs.repeat(indent) + "blueprint").writeln(stream);

	for (auto comment : comments) {
		comment->print(stream, indent + 1);
	}

	for (auto constructor : constructors) {
		constructor->print(stream, indent + 1);
	}
}

} // namespace parser::blueprint::ast
