#include "variable.hpp"
#include "../../parse_error.hpp"
#include <z/core/string.hpp>

const z::core::array<zstring> valid_types = {
	"none", "boolean", "number", "text", "color", "sound", "image", "any",
};

namespace parser::node::ast {

auto variable::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	(indent_text + "var [" + name.text + ", " + type.text + (array_type ? " array" : "") + "] = " + description.text).writeln(stream);
	for (auto i : constraints) {
		i->print(stream, indent + 1);
	}
}

auto variable::validate() const -> void {
	// Make sure variable has a valid type
	if (!valid_types.contains(type.text)) {
		throw parse_error("Invalid data type `"_zs + type.text + "`.", type.range);
	}

	// Only strings and numbers have array variants
	if (type.text != "text" && type.text != "number" && array_type) {
		throw parse_error("Arrays of `"_zs + type.text + "` type are not supported. Only text arrays and number arrays are allowed.", type.range);
	}

	// Make sure all constraints are allowed for the given type.
	for (auto constraint : constraints) {
		constraint->validate();
	}
}

} // namespace parser::node::ast
