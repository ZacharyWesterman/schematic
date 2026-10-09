#pragma once

#include "../../ast_node.hpp"
#include "../../token.hpp"

namespace parser::node::ast {

struct tag_decl : public ast_node {
	token name;
	token description;
	std::optional<token> help_text;
	std::optional<token> parent;

	auto print(std::ostream &stream, int indent) -> void override;
};

} // namespace parser::node::ast
