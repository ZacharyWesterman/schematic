#include "parser/node/lex.hpp"
#include "parser/parse_error.hpp"
#include "version.hpp"
#include <iostream>
#include <z/all.hpp>

int main() {
	("Node Parser version "_zs + VERSION).writeln(std::cout);
	auto program_text = z::file::read("tests/nodes/math1.node");

	auto tokens = parser::node::lex(program_text);

	try {
		for (auto token : tokens) {
			token.text.writeln(std::cout);
		}
	} catch (const parser::parse_error &e) {
		("ERROR: "_zs + e.context.start + ", " + e.context.end + ": " + e.message).writeln(std::cout);
	}
}
