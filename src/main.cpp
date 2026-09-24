#include "parser/context.hpp"
#include "parser/node/lex.hpp"
#include "parser/node/parse.hpp"
#include "parser/parse_error.hpp"
#include "parser/print_error.hpp"
#include "version.hpp"
#include <fstream>
#include <iostream>
#include <z/all.hpp>

int main() {
	("Node Parser version "_zs + VERSION).writeln(std::cout);

	zstring dirname = "tests/nodes";
	auto gen = z::file::listFiles(dirname, "node").map<zstring>([&dirname](auto i) { return dirname + "/" + i; });
	for (auto filename : gen) {
		auto program_text = z::file::read(filename);
		auto file = std::ifstream(filename.cstring());
		auto lexer = parser::node::lex(program_text);

		try {
			auto ast = parser::node::parse(lexer);
			std::cout << "\nPARSED AST:\n" << std::endl;
			ast->print(std::cout, 1);
		} catch (const parser::parse_error &error) {
			parser::print_error(file, error);
		}
	}
}
