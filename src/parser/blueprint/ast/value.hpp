#pragma once
#include "../../ast_node.hpp"
#include "../../token.hpp"

namespace parser::blueprint::ast {

struct value : public ast_node {
	std::optional<token> type;
	z::core::array<token> values;
	bool array_type;

	auto print(std::ostream &stream, int indent) -> void override;
};

} // namespace parser::blueprint::ast
