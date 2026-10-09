#include "value.hpp"

namespace parser::blueprint::ast {

auto value::print(std::ostream &stream, int indent) -> void {
	("  "_zs.repeat(indent) + "type: " + (type ? type.value().text : "<deduced>"_zs) + (array_type ? "[]" : "") + " (").writeln(stream);
	for (auto value : values) {
		("  "_zs.repeat(indent + 1) + value.text).writeln(stream);
	}
	("  "_zs.repeat(indent) + ")").writeln(stream);
}

} // namespace parser::blueprint::ast
