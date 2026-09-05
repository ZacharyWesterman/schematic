#pragma once

#include "span.hpp"
#include <memory>
#include <ostream>
#include <z/core/array.hpp>
#include <z/core/string.hpp>

namespace parser {

template <typename T, typename... Args>
inline auto ref(Args &&...args) -> std::shared_ptr<T> {
	return std::make_shared<T>(args...);
}

struct ast_node : public std::enable_shared_from_this<ast_node> {
	span range;

	virtual ~ast_node() = default;

	std::shared_ptr<ast_node> get_shared() {
		return shared_from_this();
	}

	virtual auto print(std::ostream &stream, int indent) -> void = 0;
};

typedef std::shared_ptr<ast_node> ast_ref;

// template <typename T, typename U>
// inline auto base(const U &shared_ptr) -> T {
// 	return std::dynamic_pointer_cast<T, U>(shared_ptr);
// }

} // namespace parser
