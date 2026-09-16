#include "ast_node.hpp"

namespace parser {

std::shared_ptr<ast_node> ast_node::get_shared() {
	return shared_from_this();
}

auto ast_node::validate() const -> bool {
	return true;
}

} // namespace parser
