#include "parser/node/lexer.hpp"
#include "parser/parse_error.hpp"
#include <iostream>

int main() {
	auto node_parser = parser::node_parser(R"(
		@math as "Arithmetic"

		[@math]
		node add as "Add" {
			"Add two numbers"

			//in val1: number as "V1"
			//in val2: number as "V2"
		}
		0b0101
	)");

	try {
		for (auto token : node_parser) {
			token.text.writeln(std::cout);
		}
	} catch (const parser::parse_error &e) {
		("ERROR: "_zs + e.context.start + ", " + e.context.end + ": " + e.message).writeln(std::cout);
	}
}
