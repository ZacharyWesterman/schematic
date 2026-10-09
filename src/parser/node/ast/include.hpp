#pragma once

#include "../../ast_node.hpp"
#include "../../token.hpp"

namespace parser::node::ast {

struct include : public ast_node {
	token source;

	auto print(std::ostream &stream, int indent) -> void;
	auto validate() const -> void override;
};

} // namespace parser::node::ast
