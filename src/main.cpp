#include "parser/context.hpp"
#include "parser/node/lex.hpp"
#include "parser/node/parse.hpp"
#include "parser/parse_error.hpp"
#include "version.hpp"
#include <fstream>
#include <iostream>
#include <z/all.hpp>

int main() {
	("Node Parser version "_zs + VERSION).writeln(std::cout);
	auto program_text = z::file::read("tests/nodes/math1.node");
	auto lexer = parser::node::lex(program_text);

	auto file = std::ifstream("tests/nodes/math1.node");

	try {
		auto ast = parser::node::parse(lexer);
		std::cout << "\nPARSED AST:\n" << std::endl;
		ast->print(std::cout, 1);
	} catch (const parser::parse_error &e) {
		auto ctx = parser::context(file, e.range);

		std::cerr << "\nERROR: " << ctx.line.begin << ", " << ctx.col.begin;
		std::cerr << " to " << ctx.line.end << ", " << ctx.col.end;
		std::cerr << ": " << e.message << std::endl;

		std::cerr << "CONTEXT: \n" << ctx.text << std::endl;
	}
}
