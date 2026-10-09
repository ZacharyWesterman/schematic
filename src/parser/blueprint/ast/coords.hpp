#pragma once
#include "../../ast_node.hpp"

namespace parser::blueprint::ast {

struct coords : public ast_node {
	double x;
	double y;

	auto print(std::ostream &stream, int indent) -> void override;
};

} // namespace parser::blueprint::ast
