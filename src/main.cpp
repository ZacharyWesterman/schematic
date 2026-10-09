#include "parser/blueprint/lex.hpp"
#include "parser/context.hpp"
#include "parser/node/lex.hpp"
#include "parser/node/parse.hpp"
#include "parser/parse_error.hpp"
#include "parser/print_error.hpp"
#include "version.hpp"
#include <fstream>
#include <iostream>
#include <z/all.hpp>

auto parse_nodes(const z::core::array<zstring> &filenames) -> void {
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

	std::cout << "\nNODES AST:\n" << std::endl;
	program->print(std::cout, 1);
}

auto parse_blueprints(const z::core::array<zstring> &filenames) -> void {
	for (auto filename : filenames) {
		auto program_text = z::file::read(filename);
		auto lexer = parser::blueprint::lex(program_text, filename);

		try {
			for (auto token : lexer) {
				std::cout << token.text << std::endl;
			}
		} catch (const parser::parse_error &error) {
			parser::print_error(error);
		}
	}
}

int main(int argc, const char **argv) {
	("Node Parser version "_zs + VERSION).writeln(std::cout);

	z::core::array<zstring> node_files;
	z::core::array<zstring> blueprint_files;

	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			zstring filename = argv[i];
			if (filename.endsWith(".node")) {
				node_files.push(filename);
			} else if (filename.endsWith(".blueprint")) {
				blueprint_files.push(filename);
			}
		}
	} else {
		zstring dirname = "tests/nodes";
		z::file::listFiles(dirname, "node").forEach([&dirname, &node_files](auto i) { node_files.push(dirname + "/" + i); }).consume();

		dirname = "tests/blueprints";
		z::file::listFiles(dirname, "blueprint").forEach([&dirname, &blueprint_files](auto i) { blueprint_files.push(dirname + "/" + i); }).consume();
	}

	parse_nodes(node_files);
	parse_blueprints(blueprint_files);
}
