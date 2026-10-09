#include "coords.hpp"

namespace parser::blueprint::ast {

auto coords::print(std::ostream &stream, int indent) -> void {
	(" "_zs.repeat(indent * 2) + "coords {" + x + ", " + y + "}").writeln(stream);
}

} // namespace parser::blueprint::ast
