#pragma once
#include "../../ast_node.hpp"
#include "value.hpp"

namespace parser::blueprint::ast {

struct field : public ast_node {
	token name;
	ref<value> args;

	auto print(std::ostream &stream, int indent) -> void override;
};

} // namespace parser::blueprint::ast
