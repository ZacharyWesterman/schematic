#pragma once

#include "../../ast_node.hpp"
#include "../../token.hpp"
#include "constraint.hpp"

namespace parser::node::ast {

struct variable : public ast_node {
	token name;
	token type;
	token description;
	z::core::array<ref<constraint>> constraints;

	auto print(std::ostream &stream, int indent) -> void;
};

} // namespace parser::node::ast
