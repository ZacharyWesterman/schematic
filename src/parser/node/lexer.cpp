#include "lexer.hpp"
#include "tokens.hpp"
#include <regex>

const std::regex IDENTIFIER("^[a-zA-Z_]\\w*");
const std::regex WHITESPACE("^\\s+");
const std::regex TAG("^@\\w+");
const std::regex EVENT("^!\\w+");
const std::regex NUMBER("^!\\w+");
const std::regex KWD_NODE("^node\\b");
const std::regex KWD_AS("^as\\b");
const std::regex KWD_IN("^in\\b");
const std::regex KWD_OUT("^out\\b");
const std::regex KWD_ON("^on\\b");
const std::regex KWD_INCLUDE("^include\\b");
const std::regex KWD_PARTIAL("^partial\\b");
const std::regex KWD_EXTENDS("^extends\\b");

#define CHECK_TOKEN(id) \
	if (std::regex_search(str, match, id)) { \
		const int old_index = state.index; \
		state.index += match.length(); \
		return token{ \
			node::id, \
			span{old_index, state.index - 1}, \
			match.str(), \
		}; \
	}

namespace parser {

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
					c == '"' ? node::STRING : node::CODE,
					span{read_until_index, state.index - 1},
					text.substr(read_until_index, state.index - read_until_index),
				};
			}
			continue;
		}

		if (c == '`' || c == '"') {
			read_until = c;
			read_until_index = state.index++;
			continue;
		}

		if (c == '[') {
			return token{node::LBRACKET, span{state.index, state.index++}, c};
		}
		if (c == ']') {
			return token{node::RBRACKET, span{state.index, state.index++}, c};
		}
		if (c == '{') {
			return token{node::LBRACE, span{state.index, state.index++}, c};
		}
		if (c == '}') {
			return token{node::RBRACE, span{state.index, state.index++}, c};
		}

		const char *str = text.cstring() + state.index;
		std::cmatch match;

		// Skip whitespace
		if (std::regex_search(str, match, WHITESPACE)) {
			state.index += match.length();
			continue;
		}

		CHECK_TOKEN(KWD_NODE)
		CHECK_TOKEN(KWD_AS)
		CHECK_TOKEN(KWD_IN)
		CHECK_TOKEN(KWD_OUT)
		CHECK_TOKEN(KWD_ON)
		CHECK_TOKEN(KWD_INCLUDE)
		CHECK_TOKEN(KWD_PARTIAL)
		CHECK_TOKEN(KWD_EXTENDS)
		CHECK_TOKEN(IDENTIFIER)
		CHECK_TOKEN(NUMBER)
		CHECK_TOKEN(TAG)
		CHECK_TOKEN(EVENT)

		state.index++; // Skip unknown chars
	}

	return {};
}

auto node_parser(const zstring &text) -> tokenizer {
	return tokenizer({text, 0}, get_token);
}

} // namespace parser
