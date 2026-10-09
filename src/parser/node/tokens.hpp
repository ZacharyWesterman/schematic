#pragma once

namespace parser::node::tokens {

enum {
	UNKNOWN,
	IDENTIFIER,
	TAG,
	EVENT,
	STRING,
	CODE,
	NUMBER,
	COLOR,
	BOOLEAN,

	LBRACE,
	RBRACE,
	LBRACKET,
	RBRACKET,
	COLON,
	COMMA,

	KWD_NODE,
	KWD_AS,
	KWD_IN,
	KWD_OUT,
	KWD_ON,
	KWD_INCLUDE,
};

extern const char *const map[];

} // namespace parser::node::tokens
