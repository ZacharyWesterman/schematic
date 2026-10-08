#include "include.hpp"
#include "../../parse_error.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

auto include::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	(indent_text + "include [" + filename.text + "]").writeln(stream);
}

auto include::validate() const -> void {
	throw parse_error("Includes are not supported yet!", range);
}

} // namespace parser::node::ast
