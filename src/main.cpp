#include "parser/context.hpp"
#include "parser/node/lex.hpp"
#include "parser/node/parse.hpp"
#include "parser/parse_error.hpp"
#include "parser/print_error.hpp"
#include "version.hpp"
#include <fstream>
#include <iostream>
#include <z/all.hpp>

auto parse_file(const zstring &filename) -> void {
	auto program_text = z::file::read(filename);
	auto file = std::ifstream(filename.cstring());
	auto lexer = parser::node::lex(program_text);

	try {
		auto ast = parser::node::parse(lexer);
		std::cout << "\nPARSED AST:\n" << std::endl;
		ast->print(std::cout, 1);
		ast->validate();
	} catch (const parser::parse_error &error) {
		parser::print_error(file, error);
	}
}

int main(int argc, const char **argv) {
	("Node Parser version "_zs + VERSION).writeln(std::cout);

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			parse_file(argv[i]);
		}

		return 0;
	}

	zstring dirname = "tests/nodes";
	auto gen = z::file::listFiles(dirname, "node").map<zstring>([&dirname](auto i) { return dirname + "/" + i; });
	for (auto filename : gen) {
		parse_file(filename);
	}
}
