#include "parse.hpp"
#include "../astgen.hpp"
#include "../parse_error.hpp"
#include "tokens.hpp"
#include <memory>

#include "ast/blueprint.hpp"
#include "ast/comment.hpp"
#include "ast/constructor.hpp"
#include "ast/coords.hpp"
#include "ast/field.hpp"
#include "ast/value.hpp"

#define M(token_id) tokens::map[token_id]
#define ACCEPT(token_id) accept(lexer, tokens::token_id)
#define EXPECT(token_id) expect(lexer, tokens::token_id, tokens::map[tokens::token_id])
#define EXPECT_EITHER(token1_id, token2_id) expect(lexer, {tokens::token1_id, tokens::token2_id}, {tokens::map[tokens::token1_id], tokens::map[tokens::token2_id]})
#define EXPECT_IF(token_id, condition) expect_if(lexer, tokens::token_id, tokens::map[tokens::token_id], (bool)(condition))

using std::optional;
using z::core::array;

namespace parser::blueprint {

auto coords(tokenizer &lexer) -> ref<ast::coords> {
	auto tok = EXPECT(LBRACE);

	auto node = create<ast::coords>();
	node->filename = tok.filename;
	node->range = tok.range;

	node->x = EXPECT(NUMBER).value;
	EXPECT(COMMA);
	node->y = EXPECT(NUMBER).value;

	node->range.end = EXPECT(RBRACE).range.end;

	return node;
}

auto comment(tokenizer &lexer) -> opt_ref<ast::comment> {
	auto text = ACCEPT(STRING);
	if (!text) {
		return {};
	}

	auto node = create<ast::comment>();
	node->filename = text.value().filename;
	node->range = text.value().range;
	node->title = text.value().text;

	node->position = coords(lexer);
	EXPECT(EQUALS);
	auto body = EXPECT(STRING);

	node->body = body.text;
	node->range.end = body.range.end;

	return node;
}

auto array_value(tokenizer &lexer) -> optional<token> {
	return accept(lexer, {tokens::NUMBER, tokens::STRING, tokens::BOOLEAN, tokens::COLOR});
}

auto field_args(tokenizer &lexer) -> ref<ast::value> {
	auto node = create<ast::value>();

	auto type = ACCEPT(IDENTIFIER);
	node->type = type;
	node->range = type->range;
	node->array_type = false;

	if (type) {
		// The only time a type name would be used is when type cannot be deduced.
		// That is, BOTH of the following are true:
		// 1. The array has zero elements.
		// 2. The field type is `any`.
		// So, it makes sense that using an explicit type name requires an empty array.
		EXPECT(LBRACKET);
		node->array_type = true;
		node->range.end = EXPECT(RBRACKET).range.end;
	} else if (ACCEPT(LBRACKET)) {
		node->array_type = true;
		// If `[...]` is used, multiple values can be put between the brackets.
		while (optional<token> val = array_value(lexer)) {
			node->values.push(val.value());
		}
		EXPECT(RBRACKET);
	} else if (auto val = array_value(lexer)) {
		// Otherwise, only a single value is allowed.
		node->values.push(val.value());
	}

	if (!type && !node->array_type && !node->values.length()) {
		auto tok = lexer.existing_token();
		throw parse_error("Expected a value, but found "_zs + symbol(tok), lexer.filename(), lexer.get_span());
	}

	return node;
}

auto field(tokenizer &lexer) -> opt_ref<ast::field> {
	auto tok = ACCEPT(IDENTIFIER);
	if (!tok) {
		return {};
	}

	EXPECT(EQUALS);
	auto node = create<ast::field>();
	node->name = tok.value();
	node->args = field_args(lexer);
	node->range = {
		tok.value().range.begin,
		node->args->range.end,
	};

	return node;
}

auto constructor(tokenizer &lexer) -> ref<ast::constructor> {
	auto tok = lexer.existing_token().value();

	auto node = create<ast::constructor>();
	node->name = tok;
	node->range = tok.range;
	node->position = coords(lexer);
	EXPECT(EQUALS);
	node->node = EXPECT(IDENTIFIER);
	node->range.end = node->node.range.end;
	node->title = ACCEPT(STRING);

	// Input values
	if (ACCEPT(LPAREN)) {
		while (auto child = field(lexer)) {
			node->inputs.push(child.value());
		}
		node->range.end = EXPECT(RPAREN).range.end;
	}

	// Output defaults
	if (ACCEPT(ARROW)) {
		EXPECT(LPAREN);
		while (auto child = field(lexer)) {
			node->outputs.push(child.value());
		}
		node->range.end = EXPECT(RPAREN).range.end;
	}

	return node;
}

auto blueprint(tokenizer &lexer) -> ref<ast::blueprint> {
	auto blue = create<ast::blueprint>();
	blue->filename = lexer.filename();
	blue->range = lexer.get_span();

	do {
		auto cmt = comment(lexer);
		if (cmt) {
			blue->comments.push(cmt.value());
			continue;
		}

		// Only other top-levels start with an identifier.
		if (ACCEPT(IDENTIFIER)) {

			// If not a route, it must be a constructor.
			blue->constructors.push(constructor(lexer));
			continue;
		}

		if (lexer.empty()) {
			// End of file.
			break;
		}

		// Unexpected token
		auto tok = lexer.existing_token();
		throw parse_error(("Expected a node constructor, route, or comment, but found "_zs + symbol(tok) + "."), lexer.filename(), lexer.get_span());

	} while (true);

	blue->range.end = lexer.get_span().end;
	return blue;
}

auto parse(tokenizer &lexer) -> ref<ast::blueprint> {
	return blueprint(lexer);
}

} // namespace parser::blueprint
