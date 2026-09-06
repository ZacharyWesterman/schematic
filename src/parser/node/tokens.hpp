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
	KWD_PARTIAL,
	KWD_EXTENDS,
};

extern const char *const map[];

} // namespace parser::node::tokens
