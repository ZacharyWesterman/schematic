#include "parser/node/lex.hpp"
#include "parser/node/parse.hpp"
#include "parser/parse_error.hpp"
#include "version.hpp"
#include <iostream>
#include <z/all.hpp>

int main() {
	("Node Parser version "_zs + VERSION).writeln(std::cout);
	auto program_text = z::file::read("tests/nodes/math1.node");

	auto lexer = parser::node::lex(program_text);
	auto ast = parser::node::parse(lexer);

	std::cout << "\nPARSED AST:\n" << std::endl;
	ast->print(std::cout, 1);
}
