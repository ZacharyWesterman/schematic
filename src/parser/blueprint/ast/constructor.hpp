#pragma once
#include "../../ast_node.hpp"
#include "../../token.hpp"
#include "coords.hpp"
#include "field.hpp"

namespace parser::blueprint::ast {

struct constructor : public ast_node {
	token name;
	std::optional<token> title;
	ref<coords> position;
	token node;
	z::core::array<ref<field>> inputs;
	z::core::array<ref<field>> outputs;

	auto print(std::ostream &stream, int indent) -> void override;
};

} // namespace parser::blueprint::ast
