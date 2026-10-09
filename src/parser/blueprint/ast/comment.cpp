#include "comment.hpp"

namespace parser::blueprint::ast {

auto comment::print(std::ostream &stream, int indent) -> void {
	("  "_zs.repeat(indent) + "comment: " + title).writeln(stream);
	position->print(stream, indent + 1);
	("  "_zs.repeat(indent + 1) + "body: " + body).writeln(stream);
}

} // namespace parser::blueprint::ast
