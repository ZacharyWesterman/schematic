#include "parser/context.hpp"
#include "parser/node/lex.hpp"
#include "parser/node/parse.hpp"
#include "parser/parse_error.hpp"
#include "parser/print_error.hpp"
#include "version.hpp"
#include <fstream>
#include <iostream>
#include <z/all.hpp>

auto parse_files(const z::core::array<zstring> &filenames) -> void {
	auto program = parser::node::new_program();

	bool errored = false;
	for (auto filename : filenames) {
		auto program_text = z::file::read(filename);
		auto lexer = parser::node::lex(program_text, filename);

		try {
			auto ast = parser::node::parse(lexer);
			program->tags.push(ast->tags);
			program->nodes.push(ast->nodes);
		} catch (const parser::parse_error &error) {
			errored = true;
			parser::print_error(error);
		}
	}

	if (errored) {
		return;
	}

	try {
		program->validate();
	} catch (const parser::parse_error &error) {
		parser::print_error(error);
		return;
	}

	std::cout << "\nPARSED AST:\n" << std::endl;
	program->print(std::cout, 1);
}

int main(int argc, const char **argv) {
	("Node Parser version "_zs + VERSION).writeln(std::cout);

	z::core::array<zstring> files;
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			files.push(argv[i]);
		}
	} else {
		zstring dirname = "tests/nodes";
		auto gen = z::file::listFiles(dirname, "node").forEach([&dirname, &files](auto i) { files.push(dirname + "/" + i); });
		gen.consume();
	}

	parse_files(files);
}
