#include "lex.hpp"
#include "../parse_error.hpp"
#include "tokens.hpp"
#include <regex>

const std::regex IDENTIFIER("^[a-zA-Z_]\\w*");
const std::regex WHITESPACE("^\\s+");
const std::regex TAG("^@\\w+");
const std::regex EVENT("^!\\w+");
const std::regex NUMBER_DEC("^(\\.[0-9_]+|[0-9][0-9_]*(\\.[0-9_]+)?)\\b");
const std::regex NUMBER_HEX("^\\b0x[0-9a-fA-F_]*\\b");
const std::regex NUMBER_OCT("^0c[0-7]*\\b");
const std::regex NUMBER_BIN("^0b[01]+\\b");
const std::regex KWD_NODE("^node\\b");
const std::regex KWD_AS("^as\\b");
const std::regex KWD_IN("^in\\b");
const std::regex KWD_OUT("^out\\b");
const std::regex KWD_ON("^on\\b");
const std::regex KWD_INCLUDE("^include\\b");
const std::regex KWD_PARTIAL("^partial\\b");
const std::regex KWD_EXTENDS("^extends\\b");
const std::regex COMMENT("^//[^\n]*");
const std::regex COMMENT_MULTILINE("^/\\*.*($|\\*/)");

#define CHAR_TOKEN(chr, id) \
	if (c == chr) { \
		return token{tokens::id, span{state.index, state.index++}, c}; \
	}

#define CHECK_TOKEN(id) CHECK_TOKEN_WITH(id, match.str())

#define CHECK_TOKEN_WITH(id, match_expr) \
	if (std::regex_search(str, match, id)) { \
		const int old_index = state.index; \
		state.index += match.length(); \
		return token{ \
			tokens::id, \
			span{old_index, state.index - 1}, \
			match_expr, \
		}; \
	}

#define CHECK_TOKEN_WITH_VALUE(id, out_id, match_expr, parse_expr) \
	if (std::regex_search(str, match, id)) { \
		const int old_index = state.index; \
		state.index += match.length(); \
		return token{ \
			tokens::out_id, \
			span{old_index, state.index - 1}, \
			match_expr, \
			match_expr.parse_expr, \
		}; \
	}

namespace parser::node {

auto get_token(programText &state) -> std::optional<token> {
	const auto &text = state.text;

	char read_until = '\0';
	int read_until_index = 0;

	while (text.length() > state.index) {
		char c = text[state.index];

		if (read_until) {
			state.index++;
			if (c == read_until) {
				return token{
					c == '"' ? tokens::STRING : tokens::CODE,
					span{read_until_index, state.index - 1},
					text.substr(read_until_index + 1, state.index - read_until_index - 2),
				};
			}
			continue;
		}

		if (c == '`' || c == '"') {
			read_until = c;
			read_until_index = state.index++;
			continue;
		}

		CHAR_TOKEN('[', LBRACKET)
		CHAR_TOKEN(']', RBRACKET)
		CHAR_TOKEN('{', LBRACE)
		CHAR_TOKEN('}', RBRACE)
		CHAR_TOKEN(':', COLON)
		CHAR_TOKEN(',', COMMA)

		const char *str = text.cstring() + state.index;
		std::cmatch match;

		// Skip whitespace and comments
		if (std::regex_search(str, match, WHITESPACE) || std::regex_search(str, match, COMMENT) || std::regex_search(str, match, COMMENT_MULTILINE)) {
			state.index += match.length();
			continue;
		}

		CHECK_TOKEN_WITH(TAG, match.str().substr(1))
		CHECK_TOKEN_WITH(EVENT, match.str().substr(1))
		CHECK_TOKEN(KWD_NODE)
		CHECK_TOKEN(KWD_AS)
		CHECK_TOKEN(KWD_IN)
		CHECK_TOKEN(KWD_OUT)
		CHECK_TOKEN(KWD_ON)
		CHECK_TOKEN(KWD_INCLUDE)
		CHECK_TOKEN(KWD_PARTIAL)
		CHECK_TOKEN(KWD_EXTENDS)
		CHECK_TOKEN(IDENTIFIER)
		CHECK_TOKEN_WITH_VALUE(NUMBER_DEC, NUMBER, zstring(match.str()), replace("_", "").floating())
		CHECK_TOKEN_WITH_VALUE(NUMBER_HEX, NUMBER, zstring(match.str()), substr(2).replace("_", "").floating(16))
		CHECK_TOKEN_WITH_VALUE(NUMBER_OCT, NUMBER, zstring(match.str()), substr(2).replace("_", "").floating(8))
		CHECK_TOKEN_WITH_VALUE(NUMBER_BIN, NUMBER, zstring(match.str()), substr(2).replace("_", "").floating(2))

		throw parse_error("Unknown character `"_zs + c + "`", {state.index, state.index++});
	}

	// Spit out any remaining string or code segments
	if (read_until) {
		state.index = text.length();
		return token{
			read_until == '"' ? tokens::STRING : tokens::CODE,
			span{read_until_index, state.index - 1},
			text.substr(read_until_index + 1, state.index - read_until_index - 1),
		};
	}

	return {};
}

auto lex(const zstring &text) -> tokenizer {
	return tokenizer({text, 0}, get_token);
}

} // namespace parser::node
