#include "tokens.hpp"

namespace parser::node::tokens {

/* clang-format off */
const char *const map[] = {
	"<unknown>", // UNKNOWN
	"<identifier>", // IDENTIFIER
    "<text>", // STRING
    "<number>", // NUMBER

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

} // namespace parser::node::tokens
