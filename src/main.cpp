#include "parser/node/lexer.hpp"
#include "parser/parse_error.hpp"
#include <iostream>

int main() {
	auto node_parser = parser::node_parser(R"(
		@math as "Arithmetic"

		[@math]
		node add as "Add" {
			"Add two numbers"
		}
	)");

	try {
		for (auto token : node_parser) {
			token.text.writeln(std::cout);
		}
	} catch (const parser::parse_error &e) {
		("ERROR: "_zs + e.context.start + ", " + e.context.end + ": " + e.message).writeln(std::cout);
	}
}
