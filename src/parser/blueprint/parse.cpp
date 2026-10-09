#include "parse.hpp"
#include "../astgen.hpp"
#include "../parse_error.hpp"
#include "tokens.hpp"
#include <memory>

#include "ast/blueprint.hpp"
#include "ast/comment.hpp"
#include "ast/coords.hpp"

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
