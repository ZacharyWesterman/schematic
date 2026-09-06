#include "parse.hpp"
#include "../astgen.hpp"
#include "../parse_error.hpp"
#include "tokens.hpp"
#include <memory>

#include "ast/constraint.hpp"
#include "ast/include.hpp"
#include "ast/node_decl.hpp"
#include "ast/program.hpp"
#include "ast/tag_decl.hpp"
#include "ast/variable.hpp"

#define ACCEPT(token_id) accept(lexer, tokens::token_id)
#define EXPECT(token_id) expect(lexer, tokens::token_id, tokens::map[tokens::token_id])
#define EXPECT_IF(token_id, condition) expect_if(lexer, tokens::token_id, tokens::map[tokens::token_id], (bool)(condition))

using std::optional;
using z::core::array;

namespace parser::node {

auto include(tokenizer &lexer) -> optional<ast_ref> {
	auto tok = accept(lexer, tokens::KWD_INCLUDE);
	if (!tok) {
		return {};
	}

	auto node = create<ast::include>();
	node->filename = EXPECT(STRING);

	node->range.begin = tok.value().range.begin;
	node->range.end = node->filename.range.end;

	return node;
}

auto tag_decl(tokenizer &lexer) -> optional<ast_ref> {
	auto tok = accept(lexer, tokens::TAG);
	if (!tok) {
		return {};
	}

	auto node = create<ast::tag_decl>();
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

auto tag_list(tokenizer &lexer) -> optional<array<token>> {
	if (!accept(lexer, tokens::LBRACKET)) {
		return {};
	}

	array<token> results;
	do {
		auto tag = EXPECT(TAG);
		results.push(tag);
		auto comma = accept(lexer, tokens::COMMA);

		if (accept(lexer, tokens::RBRACKET)) {
			break;
		}

		if (!comma) {
			const auto t1 = tokens::map[tokens::COMMA];
			const auto t2 = tokens::map[tokens::RBRACKET];
			const auto t3 = tokens::map[tokens::TAG];

			throw parse_error("Expected "_zs + t1 + ", " + t2 + " or " + t3 + " but found " + symbol(lexer.existing_token()), lexer.get_span());
		}
	} while (true);

	return results;
}

auto constraint(tokenizer &lexer) -> opt_ref<ast::constraint> {
	auto name = accept(lexer, tokens::IDENTIFIER);
	if (!name) {
		return {};
	}

	auto node = create<ast::constraint>();
	node->name = name.value();
	EXPECT(COLON);

	node->args.push(EXPECT(IDENTIFIER)); // Constraints must have at LEAST 1 value!
	while (auto arg = ACCEPT(IDENTIFIER)) {
		node->args.push(arg.value());
	}

	node->range.begin = name.value().range.begin;
	node->range.end = node->args[node->args.length() - 1].range.end;
	return node;
}

auto variable(tokenizer &lexer) -> ref<ast::variable> {
	auto node = create<ast::variable>();
	node->name = EXPECT(IDENTIFIER);
	EXPECT(COLON);
	node->type = EXPECT(IDENTIFIER);
	EXPECT(KWD_AS);
	node->description = EXPECT(STRING);

	node->range.begin = node->name.range.begin;
	node->range.end = node->description.range.end;

	// No constraints
	if (!accept(lexer, tokens::LBRACE)) {
		return node;
	}

	optional<token> close_brace;
	while (!(close_brace = accept(lexer, tokens::RBRACE))) {
		auto child = constraint(lexer);

		if (child) {
			node->constraints.push(child.value());
			ACCEPT(COMMA); // Allows for trailing comma.
			continue;
		}

		const auto t1 = tokens::map[tokens::RBRACE];
		throw parse_error("Expected constraint or "_zs + t1 + " but found " + symbol(lexer.existing_token()), lexer.get_span());
	}

	node->range.end = close_brace.value().range.end;
	return node;
}

auto input(tokenizer &lexer) -> opt_ref<ast::variable> {
	auto tok = accept(lexer, tokens::KWD_IN);
	if (!tok) {
		return {};
	}

	return variable(lexer);
}

auto output(tokenizer &lexer) -> opt_ref<ast::variable> {
	auto tok = accept(lexer, tokens::KWD_OUT);
	if (!tok) {
		return {};
	}

	return variable(lexer);
}

auto node_decl(tokenizer &lexer) -> optional<ast_ref> {
	auto tags = tag_list(lexer);

	auto tok = EXPECT_IF(KWD_NODE, tags);
	if (!tok) {
		return {};
	}

	auto node = create<ast::node_decl>();
	node->range = tok.value().range;

	if (tags) {
		node->tags = tags.value();
	}

	node->name = EXPECT(IDENTIFIER);
	EXPECT(KWD_AS);

	node->description = EXPECT(STRING);
	EXPECT(LBRACE);

	node->help_text = accept(lexer, tokens::STRING);

	// Get inputs, outputs, includes, and code.
	optional<token> close_brace;
	while (!(close_brace = accept(lexer, tokens::RBRACE))) {
		auto var = input(lexer);
		if (var) {
			node->inputs.push(var.value());
			continue;
		}

		var = output(lexer);
		if (var) {
			node->outputs.push(var.value());
			continue;
		}

		auto icl = include(lexer);
		if (icl) {
			node->includes.push(icl.value());
		}

		// Unexpected token
		auto tok = lexer.existing_token();
		throw parse_error(("Expected an input, output, code block or include, but found "_zs + symbol(tok) + "."), lexer.get_span());
	}

	node->range.end = close_brace.value().range.end;
	return node;
}

auto program(tokenizer &lexer) -> ast_ref {
	auto node = create<ast::program>();
	node->range = lexer.get_span();

	optional<ast_ref> child;
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
