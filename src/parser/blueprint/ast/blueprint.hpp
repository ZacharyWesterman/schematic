#pragma once

#include "../../ast_node.hpp"
#include "comment.hpp"

namespace parser::blueprint::ast {

struct blueprint : public ast_node {
	z::core::array<ref<comment>> comments;

	auto print(std::ostream &stream, int indent) -> void override;
};

} // namespace parser::blueprint::ast
