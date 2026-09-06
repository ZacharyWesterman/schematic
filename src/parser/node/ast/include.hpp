#pragma once

#include "../../ast_node.hpp"
#include "../../token.hpp"

namespace parser::node::ast {

struct include : public ast_node {
	token filename;

	auto print(std::ostream &stream, int indent) -> void;
};

} // namespace parser::node::ast
