#include "include.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

auto include::print(std::ostream &stream, int indent) -> void {
	auto indent_text = " "_zs.repeat(indent * 2);
	(indent_text + "include [" + filename.text + "]").writeln(stream);
}

} // namespace parser::node::ast
