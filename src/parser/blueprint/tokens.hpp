#pragma once

namespace parser::blueprint::tokens {

enum {
	UNKNOWN,
	IDENTIFIER,
	STRING,
	NUMBER,

	LBRACE,
	RBRACE,
	LBRACKET,
	RBRACKET,
	LPAREN,
	RPAREN,
	EQUALS,
	COMMA,
	ARROW,
	DOT,
	BAR,
};

extern const char *const map[];

} // namespace parser::blueprint::tokens
