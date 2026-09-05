#include "tag_decl.hpp"
#include <z/core/string.hpp>

namespace parser::node::ast {

auto tag_decl::print(std::ostream &stream, int indent) -> void {
	(" "_zs.repeat(indent * 2) + "tag_decl [" + name.text + "]").writeln(stream);
	if (description.text) {
		(" "_zs.repeat((indent + 1) * 2) + "desc: " + description.text).writeln(stream);
	}
}

} // namespace parser::node::ast
