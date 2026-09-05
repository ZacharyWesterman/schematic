#include "parse.hpp"
#include "../parse_error.hpp"
#include "tokens.hpp"
#include <memory>

#include "ast/program.hpp"
#include "ast/tag_decl.hpp"

namespace parser::node {

typedef std::function<std::optional<ast_ref>(tokenizer &)> parse_rule;

auto accept(tokenizer &lexer, int token_id) -> std::optional<token> {
	auto tok = lexer.has_token() ? lexer.get_token() : lexer.next();

	if (tok && tok.value().id == token_id) {
		lexer.next();
		return tok;
	}

	return {};
}

auto expect(tokenizer &lexer, int token_id, const zstring &expected_symbol) -> token {
	auto result = accept(lexer, token_id);

	if (!result) {
		zstring found_sym = lexer.has_token() ? lexer.get_token().text : "EOF";
		throw parse_error("Expected "_zs + expected_symbol + ", but found " + found_sym, lexer.get_span());
	}

	return result.value();
}

auto expect(tokenizer &lexer, parse_rule rule, const zstring &expected_symbol) -> ast_ref {
	auto result = rule(lexer);

	if (!result) {
		zstring found_sym = lexer.has_token() ? lexer.get_token().text : "EOF";
		throw parse_error("Expected "_zs + expected_symbol + ", but found " + found_sym, lexer.get_span());
	}

	return result.value();
}

auto tag_decl(tokenizer &lexer) -> std::optional<ast_ref> {
	auto tok = accept(lexer, tokens::TAG);
	if (!tok) {
		return {};
	}

	auto node = ref<ast::tag_decl>();
	node->range = tok.value().range;

	expect(lexer, tokens::KWD_AS, "`as`");
	auto name = expect(lexer, tokens::STRING, "a string");

	node->name = name;
	node->range.end = name.range.end;

	if (accept(lexer, tokens::LBRACE)) {
		auto desc = accept(lexer, tokens::STRING);
		if (desc) {
			node->description = desc.value();
		}

		auto brace = expect(lexer, tokens::RBRACE, "`}`");
		node->range.end = brace.range.end;
	}

	return node;
}

auto program(tokenizer &lexer) -> ast_ref {
	(void)lexer;

	auto node = ref<ast::program>();
	node->range = lexer.get_span();

	std::optional<ast_ref> child;
	do {
		child = tag_decl(lexer);

		if (!child && lexer.has_token()) {
			throw parse_error(("Unexpected symbol `"_zs + lexer.get_token().text + "`. Expected a tag or node definition."), lexer.get_span());
		}

		if (!child) {
			break;
		}

		node->children.push(child.value());
	} while (true);

	node->range.end = lexer.get_span().end;
	return node;
}

auto parse(tokenizer &lexer) -> ast_ref {
	return program(lexer);
}

} // namespace parser::node
