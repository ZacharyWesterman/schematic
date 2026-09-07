#pragma once

#include "../../ast_node.hpp"
#include "../../token.hpp"
#include "constraint.hpp"

namespace parser::node::ast {

struct event : public ast_node {
	z::core::array<token> triggers;
	token code_block;

	auto print(std::ostream &stream, int indent) -> void;
};

} // namespace parser::node::ast
