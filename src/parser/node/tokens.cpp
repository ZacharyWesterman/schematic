#include "tokens.hpp"

namespace parser::node::tokens {

/* clang-format off */
const char *const map[] = {
	"<unknown>", // UNKNOWN
	"<identifier>", // IDENTIFIER
	"<@tag>", // TAG
    "<!event>", // EVENT
    "<text>", // STRING
    "<code block>", // CODE
    "<number>", // NUMBER

    "`{`", // LBRACE
    "`}`", // RBRACE
    "`[`", // LBRACKET
    "`]`", // RBRACKET
    "`:`", // COLON
    "`,`", // COMMA

    "`node`", // KWD_NODE
    "`as`", // KWD_AS
    "`in`", // KWD_IN
    "`out`", // KWD_OUT
    "`on`", // KWD_ON
    "`include`", // KWD_INCLUDE
    "`partial`", // KWD_PARTIAL
    "`extends`", // KWD_EXTENDS
};
/* clang-format on */

} // namespace parser::node::tokens
