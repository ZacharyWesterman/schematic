#include "constructor.hpp"

namespace parser::blueprint::ast {

auto constructor::print(std::ostream &stream, int indent) -> void {
	("  "_zs.repeat(indent) + "constructor: " + name.text + " = " + node.text).writeln(stream);
	if (title) {
		("  "_zs.repeat(indent + 1) + "title: " + title.value().text).writeln(stream);
	}
	position->print(stream, indent + 1);

	for (auto i : inputs) {
		i->print(stream, indent + 1);
	}
	for (auto i : outputs) {
		i->print(stream, indent + 1);
	}
}

} // namespace parser::blueprint::ast
