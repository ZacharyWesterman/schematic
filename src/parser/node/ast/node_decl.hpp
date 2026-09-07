#pragma once

#include "../../ast_node.hpp"
#include "../../token.hpp"
#include "event.hpp"
#include "variable.hpp"
#include <optional>
#include <z/core/array.hpp>

namespace parser::node::ast {

struct node_decl : public ast_node {
	token name;
	token description;
	std::optional<token> help_text;
	z::core::array<token> tags;
	z::core::array<ref<variable>> inputs;
	z::core::array<ref<variable>> outputs;
	z::core::array<ast_ref> includes;
	z::core::array<token> code_blocks;
	z::core::array<ref<event>> events;

	auto print(std::ostream &stream, int indent) -> void;
};

} // namespace parser::node::ast
