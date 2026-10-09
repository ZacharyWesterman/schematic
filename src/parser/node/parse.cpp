#include "parse.hpp"
#include "../astgen.hpp"
#include "../parse_error.hpp"
#include "tokens.hpp"
#include <memory>

#include "ast/constraint.hpp"
#include "ast/event.hpp"
#include "ast/include.hpp"
#include "ast/node_decl.hpp"
#include "ast/program.hpp"
#include "ast/tag_decl.hpp"
#include "ast/variable.hpp"

#define M(token_id) tokens::map[token_id]
#define ACCEPT(token_id) accept(lexer, tokens::token_id)
#define EXPECT(token_id) expect(lexer, tokens::token_id, tokens::map[tokens::token_id])
#define EXPECT_EITHER(token1_id, token2_id) expect(lexer, {tokens::token1_id, tokens::token2_id}, {tokens::map[tokens::token1_id], tokens::map[tokens::token2_id]})
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
	node->source = EXPECT(STRING);
	node->filename = lexer.filename();

	node->range.begin = tok.value().range.begin;
	node->range.end = node->source.range.end;

	return node;
}

auto tag_decl(tokenizer &lexer) -> opt_ref<ast::tag_decl> {
	auto tok = accept(lexer, tokens::TAG);
	if (!tok) {
		return {};
	}

	auto node = create<ast::tag_decl>();
	node->range = tok.value().range;
	node->name = tok.value();

	if (ACCEPT(KWD_IN)) {
		node->parent = EXPECT(TAG);
	}

	EXPECT(KWD_AS);
	node->description = EXPECT(STRING);
	node->filename = lexer.filename();
	node->range.end = node->description.range.end;

	if (accept(lexer, tokens::LBRACE)) {
		auto desc = accept(lexer, tokens::STRING);
		if (desc) {
			node->help_text = desc.value();
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

			throw parse_error("Expected "_zs + t1 + ", " + t2 + " or " + t3 + " but found " + symbol(lexer.existing_token()), lexer.filename(), lexer.get_span());
		}
	} while (true);

	return results;
}

auto event(tokenizer &lexer) -> opt_ref<ast::event> {
	auto tok = ACCEPT(KWD_ON);
	if (!tok) {
		return {};
	}

	auto node = create<ast::event>();
	node->filename = lexer.filename();
	node->range = tok.value().range;
	node->triggers.push(EXPECT_EITHER(IDENTIFIER, EVENT));

	// Does NOT allow trailing comma.
	while (ACCEPT(COMMA)) {
		node->triggers.push(EXPECT_EITHER(IDENTIFIER, EVENT));
	}

	node->code_block = EXPECT(CODE);
	node->range.end = node->code_block.range.end;
	return node;
}

auto constraint(tokenizer &lexer) -> opt_ref<ast::constraint> {
	auto name = accept(lexer, tokens::IDENTIFIER);
	if (!name) {
		return {};
	}

	auto node = create<ast::constraint>();
	node->name = name.value();
	node->filename = lexer.filename();
	EXPECT(COLON);

	// Constraints must have at LEAST 1 value!
	std::initializer_list<int> ok_tokens = {tokens::IDENTIFIER, tokens::NUMBER, tokens::STRING, tokens::COLOR, tokens::BOOLEAN};
	node->args.push(expect(lexer, ok_tokens, {M(tokens::IDENTIFIER), M(tokens::NUMBER), M(tokens::STRING), M(tokens::COLOR), M(tokens::BOOLEAN)}));
	std::optional<token> arg;
	while ((arg = accept(lexer, ok_tokens))) {
		node->args.push(arg.value());
	}

	node->range.begin = name.value().range.begin;
	node->range.end = node->args[node->args.length() - 1].range.end;
	return node;
}

auto variable(tokenizer &lexer) -> ref<ast::variable> {
	auto node = create<ast::variable>();
	node->name = EXPECT(IDENTIFIER);
	node->filename = lexer.filename();
	EXPECT(COLON);
	node->type = EXPECT(IDENTIFIER);
	node->array_type = false;

	// `ident[]` indicates array type
	if (ACCEPT(LBRACKET)) {
		EXPECT(RBRACKET);
		node->array_type = true;
	}

	EXPECT(KWD_AS);
	node->description = EXPECT(STRING);

	node->range.begin = node->name.range.begin;
	node->range.end = node->description.range.end;

	// Without constraints
	if (!accept(lexer, tokens::LBRACE)) {
		return node;
	}

	// With constraints
	optional<token> close_brace;
	while (!(close_brace = accept(lexer, tokens::RBRACE))) {
		auto child = constraint(lexer);

		if (child) {
			node->constraints.push(child.value());
			ACCEPT(COMMA); // Allows for trailing comma.
			continue;
		}

		const auto t1 = tokens::map[tokens::RBRACE];
		throw parse_error("Expected constraint or "_zs + t1 + " but found " + symbol(lexer.existing_token()), lexer.filename(), lexer.get_span());
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

auto node_decl(tokenizer &lexer) -> opt_ref<ast::node_decl> {
	auto tags = tag_list(lexer);

	auto tok = EXPECT_IF(KWD_NODE, tags);
	if (!tok) {
		return {};
	}

	auto node = create<ast::node_decl>();
	node->filename = lexer.filename();
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

		if ((var = output(lexer))) {
			node->outputs.push(var.value());
			continue;
		}

		if (auto icl = include(lexer)) {
			node->includes.push(icl.value());
			continue;
		}

		if (auto code_block = ACCEPT(CODE)) {
			node->code_blocks.push(code_block.value());
			continue;
		}

		if (auto evt = event(lexer)) {
			node->events.push(evt.value());
			continue;
		}

		// Unexpected token
		auto tok = lexer.existing_token();
		throw parse_error(("Expected an input, output, event, code block or include, but found "_zs + symbol(tok) + "."), lexer.filename(), lexer.get_span());
	}

	node->range.end = close_brace.value().range.end;
	return node;
}

auto program(tokenizer &lexer) -> ref<ast::program> {
	auto pgm = create<ast::program>();
	pgm->filename = lexer.filename();
	pgm->range = lexer.get_span();

	do {
		auto tag = tag_decl(lexer);
		if (tag) {
			pgm->tags.push(tag.value());
			continue;
		}

		auto node = node_decl(lexer);
		if (node) {
			pgm->nodes.push(node.value());
			continue;
		}

		if (lexer.empty()) {
			// End of file.
			break;
		}

		// Unexpected token
		auto tok = lexer.existing_token();
		throw parse_error(("Expected a node or tag definition, but found "_zs + symbol(tok) + "."), lexer.filename(), lexer.get_span());

	} while (true);

	pgm->range.end = lexer.get_span().end;
	return pgm;
}

auto parse(tokenizer &lexer) -> ref<ast::program> {
	return program(lexer);
}

auto new_program() -> ref<ast::program> {
	return create<ast::program>();
}

} // namespace parser::node
