#pragma once

#include "../../ast_node.hpp"
#include "../../token.hpp"

namespace parser::node::ast {

struct tag_decl : public ast_node {
	token name;
	token description;

	auto print(std::ostream &stream, int indent) -> void;
};

} // namespace parser::node::ast
