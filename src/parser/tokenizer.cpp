#include "tokenizer.hpp"

namespace parser {

auto tokenizer::next() -> std::optional<token> {
	tok = z::core::generator<token, programText>::next();
	return tok;
}

auto tokenizer::get_span() -> span {
	return {
		prev_index,
		state.index,
	};
}

auto tokenizer::has_token() -> bool {
	return (bool)tok;
}

auto tokenizer::get_token() -> token {
	return tok.value();
}

} // namespace parser
