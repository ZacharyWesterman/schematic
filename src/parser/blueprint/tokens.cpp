#include "tokens.hpp"

namespace parser::blueprint::tokens {

/* clang-format off */
const char *const map[] = {
	"<unknown>", // UNKNOWN
	"<identifier>", // IDENTIFIER
    "<text>", // STRING
    "<number>", // NUMBER
    "<color>", // COLOR
    "<boolean>", // BOOLEAN

    "`{`", // LBRACE
    "`}`", // RBRACE
    "`[`", // LBRACKET
    "`]`", // RBRACKET
    "`(`", // LPAREN
    "`)`", // RPAREN
    "`=`", // EQUALS
    "`,`", // COMMA
    "`->`", // ARROW
    "`.`", // DOT
    "`|`", // BAR
};
/* clang-format on */

} // namespace parser::blueprint::tokens
