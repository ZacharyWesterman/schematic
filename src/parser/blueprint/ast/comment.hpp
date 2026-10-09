#pragma once
#include "../../ast_node.hpp"
#include "coords.hpp"

namespace parser::blueprint::ast {

struct comment : public ast_node {
	zstring title;
	zstring body;
	ref<coords> position;

	auto print(std::ostream &stream, int indent) -> void override;
};

} // namespace parser::blueprint::ast
