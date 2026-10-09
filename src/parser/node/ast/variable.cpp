#include "variable.hpp"
#include "../../parse_error.hpp"
#include <limits>
#include <z/core/string.hpp>

const z::core::array<zstring> valid_types = {
	"none", "boolean", "number", "string", "color", "sound", "image", "any",
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
		throw parse_error("Invalid data type `"_zs + type.text + "`.", filename, type.range);
	}

	// Only strings and numbers have array variants
	if (type.text != "string" && type.text != "number" && array_type) {
		throw parse_error("Arrays of `"_zs + type.text + "` type are not supported. Only string arrays and number arrays are allowed.", filename, type.range);
	}

	// Make sure all constraints are valid, and aren't specified multiple times.
	double min = std::numeric_limits<double>::min();
	double max = std::numeric_limits<double>::max();
	bool is_integer = false;
	bool has_min = false;
	bool has_max = false;

	for (int i = 0; i < constraints.length(); i++) {
		const auto &name = constraints[i]->name.text;

		for (int j = 0; j < i; j++) {
			if (name == constraints[j]->name.text) {
				throw parse_error("Redundant constraint `"_zs + name + "` already defined earlier.", filename, constraints[i]->name.range);
			}
		}

		constraints[i]->validate_type(type.text, array_type);

		if (name == "min") {
			min = constraints[i]->args[0].value;
			has_min = true;
		} else if (name == "max") {
			max = constraints[i]->args[0].value;
			has_max = true;
		} else if (name == "integer") {
			is_integer = true;
		}
	}

	// Make sure any `max` constraint is strictly greater than the `min` constraint!
	if (max <= min) {
		throw parse_error("Constraint `max` must be strictly greater than `min`.", filename, name.range);
	}

	if (is_integer) {
		if (has_max && std::trunc(max) != max) {
			throw parse_error("Constraint `integer` is true, so `max` must be an integer.", filename, name.range);
		}
		if (has_min && std::trunc(min) != min) {
			throw parse_error("Constraint `integer` is true, so `min` must be an integer.", filename, name.range);
		}
	}
}

} // namespace parser::node::ast
