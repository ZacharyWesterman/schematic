#include "astgen.hpp"
#include "parse_error.hpp"

namespace parser {

auto symbol(const std::optional<token> &tok) -> zstring {
	return tok.has_value() ? ("`"_zs + tok.value().text + "`") : "EOF";
}

auto accept(tokenizer &lexer, int token_id) -> std::optional<token> {
	auto tok = lexer.get_token();

	if (tok && tok.value().id == token_id) {
		lexer.next();
		return tok;
	}

	return {};
}

auto accept(tokenizer &lexer, std::initializer_list<parse_rule> rules) -> std::optional<ast_ref> {
	for (auto rule : rules) {
		auto node = rule(lexer);
		if (node) {
			return node;
		}
	}
	return {};
}

auto expect(tokenizer &lexer, int token_id, const zstring &expected_symbol) -> token {
	auto result = accept(lexer, token_id);

	if (!result) {
		auto tok = lexer.existing_token();
		throw parse_error("Expected "_zs + expected_symbol + ", but found " + symbol(tok), lexer.get_span());
	}

	return result.value();
}

auto expect(tokenizer &lexer, parse_rule rule, const zstring &expected_symbol) -> ast_ref {
	auto result = rule(lexer);

	if (!result) {
		auto tok = lexer.existing_token();
		throw parse_error("Expected "_zs + expected_symbol + ", but found " + symbol(tok), lexer.get_span());
	}

	return result.value();
}

auto expect_if(tokenizer &lexer, parse_rule rule, const zstring &expected_symbol, bool condition) -> std::optional<ast_ref> {
	if (condition) {
		return expect(lexer, rule, expected_symbol);
	}
	return rule(lexer);
}

auto expect_if(tokenizer &lexer, int token_id, const zstring &expected_symbol, bool condition) -> std::optional<token> {
	if (condition) {
		return expect(lexer, token_id, expected_symbol);
	}
	return accept(lexer, token_id);
}

} // namespace parser
