#pragma once

#include "../../ast_node.hpp"
#include "node_decl.hpp"
#include "tag_decl.hpp"

namespace parser::node::ast {

struct program : public ast_node {
	z::core::array<ref<tag_decl>> tags;
	z::core::array<ref<node_decl>> nodes;

	auto print(std::ostream &stream, int indent) -> void override;
	auto validate() const -> void override;

private:
	auto get_parent_tag_index(const token &tag) const -> int;
};

} // namespace parser::node::ast
