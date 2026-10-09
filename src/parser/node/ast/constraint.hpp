#pragma once

#include "../../ast_node.hpp"
#include "../../token.hpp"

namespace parser::node::ast {

struct constraint : public ast_node {
	token name;
	z::core::array<token> args;

	auto print(std::ostream &stream, int indent) -> void override;
	auto validate_type(const zstring &type, bool is_array) const -> void;
};

} // namespace parser::node::ast
