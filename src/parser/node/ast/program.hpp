#pragma once

#include "../../ast_node.hpp"

namespace parser::node::ast {

struct program : public ast_node {
	z::core::array<std::shared_ptr<ast_node>> children;

	auto print(std::ostream &stream, int indent) -> void;
};

} // namespace parser::node::ast
