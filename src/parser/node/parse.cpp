#include "parse.hpp"
#include "../astgen.hpp"
#include "../parse_error.hpp"
#include "tokens.hpp"
#include <memory>

#include "ast/program.hpp"
#include "ast/tag_decl.hpp"

namespace parser::node {

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

auto tag_list(tokenizer &lexer) -> std::optional<z::core::array<token>> {
	if (!accept(lexer, tokens::LBRACKET)) {
		return {};
	}

	z::core::array<token> results;
	do {
		auto tag = expect(lexer, tokens::TAG, "<@tag>");
		results.push(tag);
		auto comma = accept(lexer, tokens::COMMA);

		if (accept(lexer, tokens::RBRACKET)) {
			break;
		}

		if (!comma) {
			throw parse_error("Expected `,`, `]` or <@tag> but found "_zs + symbol(lexer.existing_token()), lexer.get_span());
		}
	} while (true);

	return results;
}

auto node_decl(tokenizer &lexer) -> std::optional<ast_ref> {
	auto tags = tag_list(lexer);

	auto tok = expect_if(lexer, tokens::KWD_NODE, "`node`", (bool)tags);
	return {};
}

auto program(tokenizer &lexer) -> ast_ref {
	auto node = ref<ast::program>();
	node->range = lexer.get_span();

	std::optional<ast_ref> child;
	do {
		child = accept(lexer, {tag_decl, node_decl});

		if (!child) {
			if (lexer.empty()) {
				// End of program.
				break;
			}

			// Unexpected token
			auto tok = lexer.existing_token();
			throw parse_error(("Expected a node or tag definition but found "_zs + symbol(tok) + "."), lexer.get_span());
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
