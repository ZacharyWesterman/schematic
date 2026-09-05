#include "tokenizer.hpp"

namespace parser {

auto tokenizer::next() -> std::optional<token> {
	token_pulled = true;
	if (tok) {
		prev_index = tok.value().range.end;
	}
	tok = z::core::generator<token, programText>::next();
	return tok;
}

auto tokenizer::get_span() -> span {
	return {
		prev_index,
		state.index,
	};
}

auto tokenizer::existing_token() -> std::optional<token> {
	return tok;
}

auto tokenizer::get_token() -> std::optional<token> {
	return token_pulled ? tok : next();
}

auto tokenizer::started() const -> bool {
	return token_pulled;
}

auto tokenizer::empty() const -> bool {
	return token_pulled && !tok;
}

} // namespace parser
