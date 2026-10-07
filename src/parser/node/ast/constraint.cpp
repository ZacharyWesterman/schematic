#include "constraint.hpp"
#include "../../parse_error.hpp"
#include "../tokens.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

static auto text(token tok) -> zstring {
	return tok.text;
}

static auto join(const zstring &a, const zstring &b) -> zstring {
	return a + ", " + b;
}

auto constraint::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	auto argstr = args.map<zstring>(text).reduce(join);

	(indent_text + "constraint [" + name.text + "] = " + argstr).writeln(stream);
	indent_text += "  ";
}

auto constraint::validate() const -> void {
	if (name.text == "integer") {
		if (args.length() != 1 || (args[0].text != "true" && args[0].text != "false")) {
			throw parse_error("Constraint `"_zs + name.text + "` takes a single argument of `true` or `false`.", name.range);
		}
	} else if (name.text == "min" or name.text == "max") {
		if (args.length() != 1 || args[0].id != tokens::NUMBER) {
			throw parse_error("Constraint `"_zs + name.text + "` takes a single, numeric argument.", name.range);
		}
	} else if (name.text == "format") {
		if (args.length() == 1 && args[0].text == "time") {
			return;
		}

		if (args.length() <= 2 && args[0].id == tokens::NUMBER && !(args.length() == 2 && args[1].text != "digit" && args[1].text != "digits")) {
			return;
		}

		throw parse_error("Constraint `"_zs + name.text + "` must have either the single argument `time`, or a numeric argument followed by an optional `digit` or `digits`.", name.range);
	} else {
		throw parse_error("Unknown constraint `"_zs + name.text + "`.", name.range);
	}
}

} // namespace parser::node::ast
