#include "parse.hpp"
#include "../astgen.hpp"
#include "../parse_error.hpp"
#include "tokens.hpp"
#include <memory>

#include "ast/include.hpp"
#include "ast/node_decl.hpp"
#include "ast/program.hpp"
#include "ast/tag_decl.hpp"

#define EXPECT(token_id) expect(lexer, tokens::token_id, tokens::map[tokens::token_id])

namespace parser::node {

auto include(tokenizer &lexer) -> std::optional<ast_ref> {
	auto tok = accept(lexer, tokens::KWD_INCLUDE);
	if (!tok) {
		return {};
	}

	auto node = ref<ast::include>();
	node->filename = EXPECT(STRING);

	node->range.begin = tok.value().range.begin;
	node->range.end = node->filename.range.end;

	return node;
}

auto tag_decl(tokenizer &lexer) -> std::optional<ast_ref> {
	auto tok = accept(lexer, tokens::TAG);
	if (!tok) {
		return {};
	}

	auto node = ref<ast::tag_decl>();
	node->range = tok.value().range;

	EXPECT(KWD_AS);
	auto name = EXPECT(STRING);

	node->name = name;
	node->range.end = name.range.end;

	if (accept(lexer, tokens::LBRACE)) {
		auto desc = accept(lexer, tokens::STRING);
		if (desc) {
			node->description = desc.value();
		}

		auto brace = EXPECT(RBRACE);
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
		auto tag = EXPECT(TAG);
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
	if (!tok) {
		return {};
	}

	auto node = ref<ast::node_decl>();
	node->range = tok.value().range;

	if (tags) {
		node->tags = tags.value();
	}

	node->name = EXPECT(IDENTIFIER);
	EXPECT(KWD_AS);

	node->description = EXPECT(STRING);
	EXPECT(LBRACE);

	node->help_text = accept(lexer, tokens::STRING);
	auto tok2 = EXPECT(RBRACE);
	node->range.end = tok2.range.end;

	return node;
}

auto program(tokenizer &lexer) -> ast_ref {
	auto node = ref<ast::program>();
	node->range = lexer.get_span();

	std::optional<ast_ref> child;
	do {
		child = accept(lexer, {tag_decl, node_decl, include});

		if (!child) {
			if (lexer.empty()) {
				// End of program.
				break;
			}

			// Unexpected token
			auto tok = lexer.existing_token();
			throw parse_error(("Expected a node definition, tag definition or include, but found "_zs + symbol(tok) + "."), lexer.get_span());
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
