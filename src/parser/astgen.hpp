#pragma once

#include "ast_node.hpp"
#include "tokenizer.hpp"
#include <functional>
#include <optional>

namespace parser {

typedef std::function<std::optional<ast_ref>(tokenizer &)> parse_rule;

auto symbol(const std::optional<token> &tok) -> zstring;

auto accept(tokenizer &lexer, int token_id) -> std::optional<token>;

auto accept(tokenizer &lexer, std::initializer_list<parse_rule> rules) -> std::optional<ast_ref>;

auto expect(tokenizer &lexer, int token_id, const zstring &expected_symbol) -> token;

auto expect(tokenizer &lexer, parse_rule rule, const zstring &expected_symbol) -> ast_ref;

auto expect_if(tokenizer &lexer, parse_rule rule, const zstring &expected_symbol, bool condition) -> std::optional<ast_ref>;

auto expect_if(tokenizer &lexer, int token_id, const zstring &expected_symbol, bool condition) -> std::optional<token>;

} // namespace parser
